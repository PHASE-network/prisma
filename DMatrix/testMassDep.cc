///////////////////////////////////////////////////////////////////////////
//
//    Copyright 2009 Sebastian Neubert
//
//    This file is part of rootpwa
//
//    rootpwa is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    rootpwa is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with rootpwa.  If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////

/** @brief test application for dynamic width resonance parameterizations
 */




#include <complex>
#include <vector>
#include <iostream>
#include <iomanip>
#include "TGraph.h"
#include "TSystem.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TMath.h"
#include "TApplication.h"
#include "mcPhaseSpace.h"
#include "TROOT.h"
#include "cauchyIntegral.h"
#include "dynMassDep.h"
#include "decayChannels.h"

using namespace std;
using namespace rpwa;

typedef complex<double> cd;

//const double gChargedPionMass = 0.13957018;  // charged pion rest mass [GeV/c^2] [PDG08]

// analytic solution to
// /int x/(x+a)/(x+b) dx
double testIntegral(double x, double a, double b){
  double d1 = a*TMath::Log(x+a);
  double d2 = b*TMath::Log(x+b);
  double dd = 1./(a-b);
  return dd * (d1 - d2);
}



int
main(int argc, char** argv)
{

  TApplication app("", 0, 0);
  gROOT->SetStyle("Plain");

  // double mass=1.0;
  //double q=2.0
    
  
  //  double masses[4] = {gChargedPionMass,gChargedPionMass,
  //	      gChargedPionMass,gChargedPionMass};
  //mcPhaseSpace* ps=new mcPhaseSpace(4,masses,0,4,100,100000);
  //ps->rho(1);

  double blow = -2;
  double bhigh = 20;
    
  TF1* f=new TF1("f","x/(x-10)/(x-6)",blow,bhigh);
  //f->SetParameter(0,1);
  vector<realPole> p;
  p.push_back(realPole(10,25));
  p.push_back(realPole(6,-9));
  
  //p.push_back(realPole(6,1));

 //  for(double k=0;k<=1;k+=1){
//     double res=pow(-1.,k)*TMath::Exp(k-0.5)/TMath::Pi();
//     p.push_back(realPole(k-0.5,res));
    
//   }

 

  cout << "ROOT-Integral = " << f->Integral(blow,bhigh) << endl;
  cauchyIntegral cauchy(f,p,blow,bhigh);
  cout << "Calculating hunter: " << setprecision(12);
  cout << cauchy.eval_Hunter(4) << endl; 
  cout << "Analytic solution: " << testIntegral(bhigh,-10,-6)-testIntegral(blow,-10,-6);


  f->Draw("ACP");

  gApplication->SetReturnFromRun(kFALSE);
  gSystem->Run();


  return 0;

  double mr=1.275;
  //double mr=1.7;
  double wr=0.185;
  //double wr=0.30;

  //double mr=0.77549;
  //double wr=0.1462;
  //double mr=0.5;
  //double wr=0.6;

  double masses[4] = {gChargedPionMass,gChargedPionMass,
		      gChargedPionMass,gChargedPionMass};
   dynMassDep* rho1=new dynMassDep(mr,wr,2,masses);
   rho1->setFixedChannel(0);
   rho1->phasespace()->doCalc();
   rho1->store_ms(4,20000);
   // mr=1.269;
   // wr=0.25;
   // dynMassDep* a1=new dynMassDep(mr,wr,3,masses);
//    decay21* ch1=new decay21(rho1,1,1);
//    a1->setFixedChannel(0);
//    a1->addDecayChannel(ch1);
//    a1->phasespace()->doCalc();

// //    a1->phasespace()->setSubSystems21(rho1);
// //    a1->phasespace()->doCalc(0);
//    a1->store_ms(10,1000);
   dynMassDep* rho2=new dynMassDep(mr+0.05,wr*2,2,masses);
   rho2->setFixedChannel(0);
   rho2->phasespace()->doCalc();
   rho2->store_ms(10,1000);


   mr=1.275;
   //mr=1.7;
    wr=0.109;
   //wr=0.3;
  //mr=0.77549;
  //wr=0.1462;
    dynMassDep* myBW=rho1;
 //  dynMassDep* myBW=new dynMassDep(mr,wr,4,masses);
//   decay22* ch2=new decay22(rho1,rho1,0,1);
//   //decay23* ch3=new decay23(rho1,a1,1,0.75);
//   //myBW->addDecayChannel(ch3);
//   myBW->addDecayChannel(ch2);
//   myBW->phasespace()->doCalc();
//   //myBW->phasespace()->setSubSystems132(rho1,a1);
//   //myBW->phasespace()->setSubSystems22(rho1,rho1);
//   //myBW->phasespace()->setSubSystems21(rho1);
//   //myBW->phasespace()->doCalc();
//   myBW->store_ms(10,1000);


  unsigned int nsteps=5000;
  double step=0.0005;
  double m0=0.28;

  double mtwopi=2*gChargedPionMass;

  TF1* twopi=new TF1("twopipsp","1./TMath::Pi()/16*TMath::Sqrt(x*x-[0]*[0])/x",mtwopi,10);
  twopi->SetParameter(0,mtwopi);

  TGraph* intens=new TGraph(nsteps); // intensity
  TGraph* intens_static=new TGraph(nsteps); // intensity static breitwigner
  TGraph* intens_nodisp=new TGraph(nsteps); // intensity without dispersive term
  TGraph* intens_bnl=new TGraph(nsteps); // intensity

  TGraph* rho=new TGraph(nsteps);    // phase space
  TGraph* argand=new TGraph(nsteps); // argand plot
  TGraph* argand_static=new TGraph(nsteps); // argand plot
  TGraph* argand_nodisp=new TGraph(nsteps); // argand plot

  TGraph* ms=new TGraph(nsteps); // dispersion

  TGraph* phase=new TGraph(nsteps);
  TGraph* phase_static=new TGraph(nsteps);
  TGraph* phase_nodisp=new TGraph(nsteps);
  TGraph* phase_bnl=new TGraph(nsteps);

  TGraph* phase_diff=new TGraph(nsteps);


  double rho0=myBW->get_rho0(0);
  cout << "rho0="<<rho0<<endl;;

  //TF1* f2=new TF1("ms2",myBW,&dynMassDep::disperse,myBW->phasespace()->thres()*myBW->phasespace()->thres(),400,1,"dynMassDep","disperse");

  for(unsigned int i=0; i<nsteps; ++i){
    double m=m0+i*step;
    std::cout << m << ".." << flush;
    complex<double> amp=myBW->val(m,0);
    complex<double> amp_static=myBW->val_static(m,0);
    complex<double> amp_nodisp=myBW->val_nodisperse(m,0);
    complex<double> amp_bnl=myBW->val_bnl(m,0);

    double r=myBW->get_rho(m,0)/rho0;

    intens->SetPoint(i,m,norm(amp));
    intens_static->SetPoint(i,m,norm(amp_static));
    intens_nodisp->SetPoint(i,m,norm(amp_nodisp));
    intens_bnl->SetPoint(i,m,norm(amp_bnl));

    phase->SetPoint(i,m,arg(amp));
    phase_static->SetPoint(i,m,arg(amp_static));
    phase_nodisp->SetPoint(i,m,arg(amp_nodisp));
    phase_bnl->SetPoint(i,m,arg(amp_bnl));

    rho->SetPoint(i,m,r);
    argand->SetPoint(i,amp.real(),amp.imag());
    argand_static->SetPoint(i,amp_static.real(),amp_static.imag());
    argand_nodisp->SetPoint(i,amp_nodisp.real(),amp_nodisp.imag());

    complex<double> amp2=rho2->val(m,0);

    double phdiff=arg(amp)-arg(amp2);
    phase_diff->SetPoint(i,m,phdiff);

    ms->SetPoint(i,m,myBW->get_ms(m*m,0));

    // calculate dispersion integral m(s)
    // dispersion part:
   //  TF1* f=new TF1("f",ps,&mcPhaseSpace::Evaluate,0.6,15,0,"mcPhaseSpace","Evaluate");
//     vector<realPole> p;
//     p.push_back(realPole(0,1));
    
//     cauchyIntegral cauchy(f,p,0.6,15);
//     cout << "Calculating hunter: " << setprecision(12);
//     cout << cauchy.eval_Hunter(4) << endl; 
  }

  std::cout << std::endl;

  
  



  TCanvas* c=new TCanvas("c","Mass Dep",10,10,600,600);
  c->Divide(2,3);
  c->cd(1);
  intens_static->SetTitle("Intensity");
  intens_static->Draw("AC");
  intens_nodisp->SetLineColor(kBlue);
  intens_nodisp->Draw("SAME C");
  intens_bnl->SetLineColor(kMagenta);
  intens_bnl->Draw("SAME C");
  intens->SetLineColor(kRed);
  intens->Draw("SAME C");
  ///intens->Print();
  c->cd(2);
 
  argand->SetTitle("Argand plot");
  
  argand_static->Draw("AP");
  argand_nodisp->SetMarkerColor(kBlue);argand_nodisp->Draw("SAME P");
  argand->SetMarkerColor(kRed);
  argand->Draw("SAME P");

  c->cd(3);
  phase_static->Draw("AC");
  phase_nodisp->SetLineColor(kBlue);phase_nodisp->Draw("SAME C");
  phase_bnl->SetLineColor(kMagenta);phase_bnl->Draw("SAME C");
  phase->SetLineColor(kRed);phase->Draw("SAME C");  

  c->cd(4);
  
  phase_diff->Draw("APC");


  c->cd(6);
  ms->Draw("AC");

  c->cd(5);
  myBW->phasespace()->getGraph(0)->Draw("AC");
  //myBW->phasespace()->getGraph()->Print();
  twopi->SetLineColor(kRed);
  twopi->Draw("SAME C");
 
  c->ForceUpdate();
  c->Flush();


  TFile* file=TFile::Open("parmeterize.root","RECREATE");
  intens_static->Write("StaticIntensity");
  intens_nodisp->Write("DynamicNoDispIntensity");
  intens->Write("DynIntensity");
  phase_static->Write("StaticPhase");
  phase_nodisp->Write("DynamicNoDispPhase");
  phase->Write("DynPhase");

  myBW->phasespace()->getGraph(0)->Write("PhaseSpace");
  argand->Write("Argand");
  


  gApplication->SetReturnFromRun(kFALSE);
  gSystem->Run();

  file->Close();


  return 0;

}
