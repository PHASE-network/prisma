#include <complex>
#include <vector>
#include <fstream>
#include <string>


#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "dMatrixAmp.h"

#include "Math/GSLIntegrator.h"
#include "Math/IFunction.h"
#include <complex>
#include "TCanvas.h"
#include "TF1.h"
#include "TGraph.h"

using namespace std;
 #include <boost/numeric/ublas/vector.hpp>
 #include <boost/numeric/ublas/vector_proxy.hpp>
 #include <boost/numeric/ublas/matrix.hpp>
 #include <boost/numeric/ublas/triangular.hpp>
 #include <boost/numeric/ublas/lu.hpp>
 #include <boost/numeric/ublas/io.hpp>

 namespace ublas = boost::numeric::ublas;
 typedef complex<double> cnum;
 typedef ublas::matrix<cnum> cmatrix;

using namespace ublas;

double twoBodyPS(double* x, double* par){
  // x[0]=m
  // par[0]=mu1; par[1]=mu2; : masses
  double s=x[0]*x[0];
  double mup=par[0]+par[1];
  if(x[0]<mup)return 0;
  double mum=par[1]-par[0];
  return sqrt( (s-mup*mup)*(s-mum*mum) ) / (16*TMath::Pi()*s);
}

int 
main(int argc, char** argv){
  
  
  TApplication app("", 0, 0);
  //gROOT->SetStyle("Plain");
  gStyle->SetGridStyle(1);

  double mpi=0.13957018;

  double m1= 1.8;
  double m2= 1.95;
  // double bre=0.4;
  // double bim=0.4;
  // double gamma1=0.3;
  // double gamma2=0.3;
  // double Bre=bre; double Bim=bim;

  // setup dMatrix:
  
  TF1* fpipi=new TF1("pipi_ps",twoBodyPS,0,10,2);
  fpipi->SetParameters(mpi,mpi);

  TF1* f4pi=new TF1("4pi_ps",twoBodyPS,0,10,2);
  f4pi->SetParameters(2*mpi,2*mpi);

 
  dMatrixAmp damp;
 
  damp.setNPoles(2);
  damp.setNBkg(1);
  damp.addChannel(fpipi); 
  damp.addChannel(f4pi); 

  // Parameters of amplitude:
  // bare pole masses
  matrix<double> mbare(1,damp.nPoles()+damp.nBkg());
  mbare(0,0)=m1;mbare(0,1)=m2;mbare(0,2)=m1;

  // decay width
  matrix<double> gamma(damp.nPoles()+damp.nBkg(),damp.nChannels());
  gamma(0,0)=0.3;gamma(0,1)=0.2;
  gamma(1,0)=0.38;gamma(1,1)=0.25;
  gamma(2,0)=0.3;gamma(2,1)=0.05;
  

  // production amplitudes (1 per pole)
  cmatrix production(damp.nPoles()+damp.nBkg(),1);
  production(0,0)=cnum(1,0);
  production(1,0)=cnum(0,0);
  production(2,0)=cnum(1,0);

  // mixing parameters (usually symmetric!)
  matrix<double> mixing(damp.nPoles()+damp.nBkg(),damp.nPoles()+damp.nBkg());
  mixing(0,0)=1;mixing(0,1)=0.5;mixing(0,2)=0.2;
  mixing(1,0)=0.5;mixing(1,1)=1;mixing(1,2)=0;
  mixing(2,0)=0.2;mixing(2,1)=0;mixing(2,2)=1;

  damp.Setup(mbare,gamma,production, mixing);

  cerr << " Setup sucessfull" << endl;
  
  unsigned int nPar=damp.getNPar();
  double par1[nPar];
  double par2[nPar];
  damp.getPar(par1);

  cerr << "got par1" << endl;

  damp.setPar(par1);

  cerr << "set par1" << endl;

  damp.getPar(par2);

 cerr << "got par2" << endl;

  for(unsigned int ip=0;ip<nPar;++ip){
    if(par1[ip]!=par2[ip]){
      cerr << "Problem with parameter setter/getter i=" << ip << endl;
      cerr << "par1=" << par1[ip] << "    par2="<<par2[ip] << endl;
    }
  }



  unsigned int n=200;
  TGraph* gIntens=new TGraph(n);
  TGraph* gPhase=new TGraph(n);
 

  double m0=-0.5;
  double mstep=0.01;
  for(unsigned int i=0;i<n;++i){
    double m=m0+i*mstep;
    cnum a=damp.amp(m,0);
    gIntens->SetPoint(i,m,norm(a));
    gPhase->SetPoint(i,m,arg(a));

  }



  TCanvas* c=new TCanvas("Mixing","Mixing",10,10,800,800);
  c->Divide(2,4);
  c->cd(1);gIntens->Draw("APL");
  c->cd(2);gPhase->Draw("APL");


  gApplication->SetReturnFromRun(kFALSE);
  gSystem->Run();

}
