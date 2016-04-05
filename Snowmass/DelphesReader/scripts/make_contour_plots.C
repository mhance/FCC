#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>

#include "TFile.h"
#include "TH1F.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TList.h"
#include "TGraphAsymmErrors.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TMath.h"
#include "TLegend.h"
#include "TGraph2D.h"
#include "TROOT.h"
#include "TObjArray.h"
#include "TLine.h"
#include "TGraphSmooth.h"
#include "TStyle.h"
#include "TColor.h"

#include "utilities.h"


using namespace std;

TGraph* GetTGraph(TFile* contours,
		  string grid,
		  int energy,
		  int pileup,
		  int lumi,
		  string variable,
		  float uncertainty,
		  string label,
		  int leadjetcut=1,
		  int metsignifcut=15){
  string name=PlotName(grid,energy,pileup,lumi,variable,uncertainty,label,leadjetcut,metsignifcut);
  TGraph* g = (TGraph*)contours->Get(name.c_str());
  return g;
}

void go_final_summary_GOGO(string label="disc",
			   string xaxis="m_{#tilde{g}} [TeV]",
			   string yaxis="m_{#tilde{#chi}_{1}^{0}} [TeV]",
			   string process="pp#rightarrow#tilde{g}#tilde{g}#rightarrowq#bar{q}#tilde{#chi}_{1}^{0}q#bar{q}#tilde{#chi}_{1}^{0}"){

  TFile* contours = new TFile("contours.root","RO");
  string gridname_14="GO_decoupledSQ_14TeV_LHE_QQALL";
  TGraph* GOGO_14_000_0300  = GetTGraph(contours,"GO_decoupledSQ_14TeV_LHE_QQALL", 14,0, 300,"MET___HTjets_30_3p5",20,label,1,15);
  TGraph* GOGO_14_000_3000  = GetTGraph(contours,"GO_decoupledSQ_14TeV_LHE_QQALL", 14,0,3000,"MET___HTjets_30_3p5",20,label,1,15);
  TGraph* GOGO_33_000_3000  = GetTGraph(contours,"GO_decoupledSQ_33TeV_LHE_QQALL", 33,0,3000,"MET___HTjets_30_3p5",20,label,1,15);
  TGraph* GOGO_100_000_3000 = GetTGraph(contours,"GO_decoupledSQ_100TeV"         ,100,0,3000,"MET___HTjets_30_3p5",20,label,1,15);

  int ngraphs=2;

  GOGO_100_000_3000->SetLineColor(kGreen+2);GOGO_100_000_3000->SetLineStyle(kSolid);
  GOGO_33_000_3000->SetLineColor(kRed);     GOGO_33_000_3000->SetLineStyle(kSolid);
  GOGO_14_000_0300->SetLineColor(kBlack);   GOGO_14_000_0300->SetLineStyle(kSolid);
  GOGO_14_000_3000->SetLineColor(kBlue);    GOGO_14_000_3000->SetLineStyle(kSolid);

  TGraph* base=GOGO_100_000_3000;

  TCanvas* c(0);
  if(label == "disc")
    c = new TCanvas("summary_GOGO_discovery","summary",800,600);
  else if(label == "limit")
    c = new TCanvas("summary_GOGO_limit","summary",800,600);
  float legymax=0.78;
  TLegend* leg = new TLegend(0.20,(legymax-0.1*ngraphs),0.55,legymax);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  GOGO_100_000_3000->Draw("AL");   leg->AddEntry(GOGO_100_000_3000,"100 TeV, 3000 fb^{-1}" ,"l");
  GOGO_33_000_3000->Draw("L");   leg->AddEntry(GOGO_33_000_3000,"33 TeV, 3000 fb^{-1}" ,"l");
  GOGO_14_000_3000->Draw("L");    leg->AddEntry(GOGO_14_000_3000,"14 TeV, 3000 fb^{-1}","l");
  GOGO_14_000_0300->Draw("L");    leg->AddEntry(GOGO_14_000_0300,"14 TeV, 300 fb^{-1}" ,"l");

  int xmax=12;
  int ndiv=506;
  if(label=="limit"){
    xmax=15;
    ndiv=505;
  }

  base->GetXaxis()->SetLimits(0,xmax);
  base->GetYaxis()->SetLimits(0,xmax);
  base->SetMinimum(0);
  base->SetMaximum(xmax);
  base->GetXaxis()->SetNdivisions(ndiv);
  base->GetYaxis()->SetNdivisions(ndiv);
  base->GetXaxis()->SetTitle(xaxis.c_str());
  base->GetYaxis()->SetTitle(yaxis.c_str());
  base->GetXaxis()->SetTitleSize(0.06);
  base->GetYaxis()->SetTitleSize(0.06);
  base->GetXaxis()->SetTitleOffset(1.0);
  base->GetYaxis()->SetTitleOffset(1.0);


  TLine* l = new TLine(0,0,xmax,xmax);
  l->SetLineColor(kGray);
  l->SetLineStyle(kDotted);
  l->SetLineWidth(2);
  l->Draw();
  
  leg->Draw();

  myText(0.21,0.87,kBlack,process.c_str(),-1,0.05);

  if(label=="limit")
    myText(0.21,0.80,kBlack,"95% CL exclusion",-1,0.05);
  else if(label=="disc")
    myText(0.21,0.80,kBlack,"5 #sigma discovery",-1,0.05);


  c->Print(((string)c->GetName()+".pdf").c_str());
}

void go_final_summary_SQSQ(string label="disc",
			   string xaxis="m_{#tilde{q}} [TeV]",
			   string yaxis="m_{#tilde{#chi}_{1}^{0}} [TeV]",
			   string process="pp#rightarrow#tilde{q}#tilde{q}#rightarrowq#tilde{#chi}_{1}^{0}q#tilde{#chi}_{1}^{0}"){

  TFile* contours = new TFile("contours.root","RO");

  TGraph* GOGO_14_000_0300;
  TGraph* GOGO_14_000_3000;
  TGraph* GOGO_33_000_3000;
  TGraph* GOGO_100_000_3000;

  bool smooth=false;
  if(smooth){
    TGraph* GOGO_14_000_0300_orig  = GetTGraph(contours,"SQ_decoupledGO_14TeV"  , 14,0, 300,"MET___HTjets_30_3p5",20,label,1,15);
    TGraph* GOGO_14_000_3000_orig  = GetTGraph(contours,"SQ_decoupledGO_14TeV"  , 14,0,3000,"MET___HTjets_30_3p5",20,label,1,15);
    TGraph* GOGO_33_000_3000_orig  = GetTGraph(contours,"SQ_decoupledGO_33TeV"  , 33,0,3000,"MET___HTjets_30_3p5",20,label,1,15);
    TGraph* GOGO_100_000_3000_orig = GetTGraph(contours,"SQ_decoupledGO_100TeV" ,100,0,3000,"MET___HTjets_30_3p5",20,label,1,15);

    TGraphSmooth* GOGO_14_000_0300_smooth = new TGraphSmooth(Form("normal_SQ_decoupledGO_14TeV_14TeV_0Pileup_300ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));
    TGraphSmooth* GOGO_14_000_3000_smooth = new TGraphSmooth(Form("normal_SQ_decoupledGO_14TeV_14TeV_0Pileup_3000ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));
    TGraphSmooth* GOGO_33_000_3000_smooth = new TGraphSmooth(Form("normal_SQ_decoupledGO_33TeV_14TeV_0Pileup_3000ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));
    TGraphSmooth* GOGO_100_000_3000_smooth = new TGraphSmooth(Form("normal_SQ_decoupledGO_100TeV_100TeV_0Pileup_3000ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));

    bool smoothlowess=false;
    if(false){
      GOGO_14_000_0300 = GOGO_14_000_0300_smooth ->Approx(GOGO_14_000_0300_orig ,"linear");
      GOGO_14_000_3000 = GOGO_14_000_3000_smooth ->Approx(GOGO_14_000_3000_orig ,"linear");
      GOGO_33_000_3000 = GOGO_33_000_3000_smooth ->Approx(GOGO_33_000_3000_orig ,"linear");
      GOGO_100_000_3000= GOGO_100_000_3000_smooth->Approx(GOGO_100_000_3000_orig,"linear");
    }
    else if(smoothlowess){
      float bandwidth=1.0;
      GOGO_14_000_0300 = GOGO_14_000_0300_smooth ->SmoothLowess(GOGO_14_000_0300_orig ,"normal",bandwidth);
      GOGO_14_000_3000 = GOGO_14_000_3000_smooth ->SmoothLowess(GOGO_14_000_3000_orig ,"normal",bandwidth);
      GOGO_33_000_3000 = GOGO_33_000_3000_smooth ->SmoothLowess(GOGO_33_000_3000_orig ,"normal",bandwidth);
      GOGO_100_000_3000= GOGO_100_000_3000_smooth->SmoothLowess(GOGO_100_000_3000_orig,"normal",bandwidth);
    }
    else{
      float bandwidth=0;
      GOGO_14_000_0300 = GOGO_14_000_0300_smooth ->SmoothSuper(GOGO_14_000_0300_orig ,"normal",bandwidth);
      GOGO_14_000_3000 = GOGO_14_000_3000_smooth ->SmoothSuper(GOGO_14_000_3000_orig ,"normal",bandwidth);
      GOGO_33_000_3000 = GOGO_33_000_3000_smooth ->SmoothSuper(GOGO_33_000_3000_orig ,"normal",bandwidth);
      GOGO_100_000_3000= GOGO_100_000_3000_smooth->SmoothSuper(GOGO_100_000_3000_orig,"normal",bandwidth);
    }
  }
  else{
    GOGO_14_000_0300  = GetTGraph(contours,"SQ_decoupledGO_14TeV"  , 14,0, 300,"MET___HTjets_30_3p5",20,label,1,15);
    GOGO_14_000_3000  = GetTGraph(contours,"SQ_decoupledGO_14TeV"  , 14,0,3000,"MET___HTjets_30_3p5",20,label,1,15);
    GOGO_33_000_3000  = GetTGraph(contours,"SQ_decoupledGO_33TeV"  , 33,0,3000,"MET___HTjets_30_3p5",20,label,1,15);
    GOGO_100_000_3000 = GetTGraph(contours,"SQ_decoupledGO_100TeV" ,100,0,3000,"MET___HTjets_30_3p5",20,label,1,15);
  }


  int ngraphs=2;

  GOGO_100_000_3000->SetLineColor(kGreen+2);GOGO_100_000_3000->SetLineStyle(kSolid); GOGO_100_000_3000->SetLineWidth(3);
  GOGO_33_000_3000->SetLineColor(kRed);     GOGO_33_000_3000->SetLineStyle(kSolid);  GOGO_33_000_3000->SetLineWidth(3);
  GOGO_14_000_0300->SetLineColor(kBlack);   GOGO_14_000_0300->SetLineStyle(kSolid);  GOGO_14_000_0300->SetLineWidth(3);
  GOGO_14_000_3000->SetLineColor(kBlue);    GOGO_14_000_3000->SetLineStyle(kSolid);  GOGO_14_000_3000->SetLineWidth(3);

  TGraph* base=GOGO_100_000_3000;

  TCanvas* c(0);
  if(label == "limit")
    c = new TCanvas("summary_SQSQ_limit","summary",800,600);
  else if(label == "disc")
    c = new TCanvas("summary_SQSQ_discovery","summary",800,600);
  float legymax=0.78;
  TLegend* leg = new TLegend(0.20,(legymax-0.1*ngraphs),0.55,legymax);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  GOGO_100_000_3000->Draw("AL");   leg->AddEntry(GOGO_100_000_3000,"100 TeV, 3000 fb^{-1}" ,"l");
  GOGO_33_000_3000->Draw("L");   leg->AddEntry(GOGO_33_000_3000,"33 TeV, 3000 fb^{-1}" ,"l");
  GOGO_14_000_3000->Draw("L");    leg->AddEntry(GOGO_14_000_3000,"14 TeV, 3000 fb^{-1}","l");
  GOGO_14_000_0300->Draw("L");    leg->AddEntry(GOGO_14_000_0300,"14 TeV, 300 fb^{-1}" ,"l");

  int xmax=3;
  int ndiv=506;
  if(label=="limit"){
    xmax=10;
    ndiv=505;
  }

  base->GetXaxis()->SetLimits(0,xmax);
  base->GetYaxis()->SetLimits(0,xmax);
  base->SetMinimum(0);
  base->SetMaximum(xmax);
  base->GetXaxis()->SetNdivisions(ndiv);
  base->GetYaxis()->SetNdivisions(ndiv);
  base->GetXaxis()->SetTitle(xaxis.c_str());
  base->GetYaxis()->SetTitle(yaxis.c_str());
  base->GetXaxis()->SetTitleSize(0.06);
  base->GetYaxis()->SetTitleSize(0.06);
  base->GetXaxis()->SetTitleOffset(1.0);
  base->GetYaxis()->SetTitleOffset(1.0);


  TLine* l = new TLine(0,0,xmax,xmax);
  l->SetLineColor(kGray);
  l->SetLineStyle(kDotted);
  l->SetLineWidth(2);
  l->Draw();
  
  leg->Draw();

  myText(0.21,0.87,kBlack,process.c_str(),-1,0.05);

  if(label=="limit")
    myText(0.21,0.80,kBlack,"95% CL exclusion",-1,0.05);
  else if(label=="disc")
    myText(0.21,0.80,kBlack,"5 #sigma discovery",-1,0.05);


  c->Print(((string)c->GetName()+".pdf").c_str());
}

void go_final_summary_GOSQ(string label="disc",
			   string xaxis="m_{#tilde{g}} [TeV]",
			   string yaxis="m_{#tilde{q}} [TeV]",
			   string process="pp#rightarrow#tilde{g}#tilde{g},#tilde{g}#tilde{q},#tilde{q}#tilde{q}"){

  TFile* contours = new TFile("contours.root","RO");
  TGraph* GOGO_14_000_0300  = GetTGraph(contours,"GOSQ_masslessN0_14TeV"  , 14,0, 300,"MET___HTjets_30_3p5",20,label,1,15);
  TGraph* GOGO_14_000_3000  = GetTGraph(contours,"GOSQ_masslessN0_14TeV"  , 14,0,3000,"MET___HTjets_30_3p5",20,label,1,15);
  TGraph* GOGO_33_000_3000  = GetTGraph(contours,"GOSQ_masslessN0_33TeV"  , 33,0,3000,"MET___HTjets_30_3p5",20,label,1,15);
  TGraph* GOGO_100_000_3000 = GetTGraph(contours,"GOSQ_masslessN0_100TeV" ,100,0,3000,"MET___HTjets_30_3p5",20,label,1,15);

  int ngraphs=2;

  GOGO_100_000_3000->SetLineColor(kGreen+2);GOGO_100_000_3000->SetLineStyle(kSolid);
  GOGO_33_000_3000->SetLineColor(kRed);     GOGO_33_000_3000->SetLineStyle(kSolid);
  GOGO_14_000_0300->SetLineColor(kBlack);   GOGO_14_000_0300->SetLineStyle(kSolid);
  GOGO_14_000_3000->SetLineColor(kBlue);    GOGO_14_000_3000->SetLineStyle(kSolid);

  TGraph* base=GOGO_100_000_3000;

  TCanvas* c(0);
  if(label == "limit")
    c = new TCanvas("summary_GOSQ_limit","summary",800,600);
  else if(label == "disc")
    c = new TCanvas("summary_GOSQ_discovery","summary",800,600);
  float legymax=0.78;
  TLegend* leg = new TLegend(0.20,(legymax-0.1*ngraphs),0.55,legymax);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  GOGO_100_000_3000->Draw("AL");  leg->AddEntry(GOGO_100_000_3000,"100 TeV, 3000 fb^{-1}" ,"l");
  GOGO_33_000_3000->Draw("L");    leg->AddEntry(GOGO_33_000_3000,"33 TeV, 3000 fb^{-1}" ,"l");
  GOGO_14_000_3000->Draw("L");    leg->AddEntry(GOGO_14_000_3000,"14 TeV, 3000 fb^{-1}","l");
  GOGO_14_000_0300->Draw("L");    leg->AddEntry(GOGO_14_000_0300,"14 TeV, 300 fb^{-1}" ,"l");

  base->GetXaxis()->SetLimits(0,24);
  base->GetYaxis()->SetLimits(0,24);
  base->SetMinimum(0);
  base->SetMaximum(24);
  base->GetXaxis()->SetNdivisions(506);
  base->GetYaxis()->SetNdivisions(506);
  base->GetXaxis()->SetTitle(xaxis.c_str());
  base->GetYaxis()->SetTitle(yaxis.c_str());
  base->GetXaxis()->SetTitleSize(0.06);
  base->GetYaxis()->SetTitleSize(0.06);
  base->GetXaxis()->SetTitleOffset(1.0);
  base->GetYaxis()->SetTitleOffset(1.0);


  TLine* l_33_x = new TLine(9.981,0,9.981,9.981);
  l_33_x->SetLineColor(kRed);
  l_33_x->SetLineStyle(kDotted);
  l_33_x->SetLineWidth(1);
  l_33_x->Draw();
  TLine* l_33_y = new TLine(0,9.981,9.981,9.981);
  l_33_y->SetLineColor(kRed);
  l_33_y->SetLineStyle(kDotted);
  l_33_y->SetLineWidth(1);
  l_33_y->Draw();
  

  TLine* l_14_3000_x = new TLine(4.489,0,4.489,4.489);
  l_14_3000_x->SetLineColor(kBlue);
  l_14_3000_x->SetLineStyle(kDotted);
  l_14_3000_x->SetLineWidth(1);
  l_14_3000_x->Draw();
  TLine* l_14_3000_y = new TLine(0,4.489,4.489,4.489);
  l_14_3000_y->SetLineColor(kBlue);
  l_14_3000_y->SetLineStyle(kDotted);
  l_14_3000_y->SetLineWidth(1);
  l_14_3000_y->Draw();
  

  // TLine* l_100_x = new TLine(19.944,0,19.944,19.944);
  // l_100_x->SetLineColor(kGreen-2);
  // l_100_x->SetLineStyle(kDotted);
  // l_100_x->SetLineWidth(1);
  // l_100_x->Draw();
  // TLine* l_100_y = new TLine(10,19.944,19.944,19.944);
  // l_100_y->SetLineColor(kGreen-2);
  // l_100_y->SetLineStyle(kDotted);
  // l_100_y->SetLineWidth(1);
  // l_100_y->Draw();

  leg->Draw();

  myText(0.21,0.87,kBlack,process.c_str(),-1,0.05);

  if(label=="limit")
    myText(0.21,0.80,kBlack,"95% CL exclusion",-1,0.05);
  else if(label=="disc")
    myText(0.21,0.80,kBlack,"5 #sigma discovery",-1,0.05);


  c->Print(((string)c->GetName()+".pdf").c_str());
}

void go_final_summaries(){
  go_final_summary_GOGO("limit");
  go_final_summary_GOGO("disc");
  go_final_summary_SQSQ("limit");
  go_final_summary_SQSQ("disc");
  go_final_summary_GOSQ("limit");
  go_final_summary_GOSQ("disc");
}

void go_pileup_comp(int energy=14,
		    int lumi=3000,
		    string label="disc",
		    string xaxis="m_{#tilde{g}} [TeV]",
		    string yaxis="m_{#tilde{#chi}_{1}^{0}} [TeV]",
		    string process="pp#rightarrow#tilde{g}#tilde{g}#rightarrowq#bar{q}#tilde{#chi}_{1}^{0}q#bar{q}#tilde{#chi}_{1}^{0}"){

  TFile* contours = new TFile("contours.root","RO");
  string gridname;
  if(energy==14){
    gridname="GO_decoupledSQ_14TeV_LHE_QQALL";
  }
  else if(energy==33){
    gridname="GO_decoupledSQ_33TeV_LHE_QQALL";
  }
  else if(energy==100){
    gridname="GO_decoupledSQ_100TeV";
  }

  TGraph* GOGO_000 = GetTGraph(contours,gridname,energy,  0,lumi,"MET___HTjets_30_3p5",20,label,1,15);
  TGraph* GOGO_140 = GetTGraph(contours,gridname,energy,140,lumi,"MET___HTjets_30_3p5",20,label,1,15);

  int ngraphs=2;

  GOGO_000->SetLineColor(kBlack);  GOGO_000->SetLineStyle(kSolid);
  GOGO_140->SetLineColor(kRed);    GOGO_140->SetLineStyle(kDashed);

  TGraph* base=GOGO_000;

  TCanvas* c = new TCanvas(Form("c_pileup_%dTeV_%difb_%s",energy,lumi,label.c_str()),"pileup comparison",800,600);
  float legymax=0.78;
  TLegend* leg = new TLegend(0.20,(legymax-0.1*ngraphs),0.55,legymax);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  GOGO_000->Draw("AL");   leg->AddEntry(GOGO_000,"0 Extra int/xing" ,"l");
  GOGO_140->Draw("L");    leg->AddEntry(GOGO_140,"140 Extra int/xing","l");

  base->GetXaxis()->SetLimits(0,3);
  base->GetYaxis()->SetLimits(0,3);
  base->SetMinimum(0);
  base->SetMaximum(3);
  base->GetXaxis()->SetNdivisions(506);
  base->GetYaxis()->SetNdivisions(506);
  base->GetXaxis()->SetTitle(xaxis.c_str());
  base->GetYaxis()->SetTitle(yaxis.c_str());
  base->GetXaxis()->SetTitleSize(0.06);
  base->GetYaxis()->SetTitleSize(0.06);
  base->GetXaxis()->SetTitleOffset(1.0);
  base->GetYaxis()->SetTitleOffset(1.0);


  TLine* l = new TLine(0,0,3,3);
  l->SetLineColor(kGray);
  l->SetLineStyle(kDotted);
  l->SetLineWidth(2);
  l->Draw();
  
  leg->Draw();

  myText(0.21,0.87,kBlack,process.c_str(),-1,0.05);

  if(label=="limit")
    myText(0.21,0.80,kBlack,"95% CL exclusion",-1,0.05);
  else if(label=="disc")
    myText(0.21,0.80,kBlack,"5 #sigma discovery",-1,0.05);

  myText(0.55,0.87,kBlack,Form("#scale[.75]{#int}Ldt = %d fb^{-1}",lumi));
  myText(0.55,0.80,kBlack,Form("#sqrt{s}=%d TeV",energy));

  c->Print(((string)c->GetName()+".pdf").c_str());
}

void go_uncertainty_comp(int energy=14,
			 int lumi=3000,
			 string label="disc",
			 string xaxis="m_{#tilde{g}} [TeV]",
			 string yaxis="m_{#tilde{#chi}_{1}^{0}} [TeV]",
			 string process="pp#rightarrow#tilde{g}#tilde{g}#rightarrowq#bar{q}#tilde{#chi}_{1}^{0}q#bar{q}#tilde{#chi}_{1}^{0}"){

  TFile* contours = new TFile("contours.root","RO");

  string gridname;
  if(energy==14){
    gridname="GO_decoupledSQ_14TeV_LHE_QQALL";
  }
  else if(energy==33){
    gridname="GO_decoupledSQ_33TeV_LHE_QQALL";
  }
  else if(energy==100){
    gridname="GO_decoupledSQ_100TeV";
  }

  TGraph* unc30 = GetTGraph(contours,gridname,energy,0,lumi,"MET___HTjets_30_3p5",30,label,1,15);
  TGraph* unc20 = GetTGraph(contours,gridname,energy,0,lumi,"MET___HTjets_30_3p5",20,label,1,15);
  TGraph* unc10 = GetTGraph(contours,gridname,energy,0,lumi,"MET___HTjets_30_3p5",10,label,1,15);
  TGraph* unc05 = GetTGraph(contours,gridname,energy,0,lumi,"MET___HTjets_30_3p5", 5,label,1,15);

  int ngraphs=4;

  unc30->SetLineColor(kBlue);     unc30->SetLineStyle(kSolid);
  unc20->SetLineColor(kBlack);    unc20->SetLineStyle(kSolid);
  unc10->SetLineColor(kRed);      unc10->SetLineStyle(kSolid);
  unc05->SetLineColor(kGreen-2);  unc05->SetLineStyle(kSolid);

  TGraph* base=unc05;

  TCanvas* c = new TCanvas(Form("c_uncertainty_%dTeV_%difb_%s",energy,lumi,label.c_str()),"uncertainty comparison",800,600);
  float legymax=0.78;
  TLegend* leg = new TLegend(0.20,(legymax-0.06*ngraphs),0.55,legymax);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  unc05->Draw("AL");   leg->AddEntry(unc05,"5% syst." ,"l");
  unc10->Draw("L");   leg->AddEntry(unc10,"10% syst." ,"l");
  unc20->Draw("L");   leg->AddEntry(unc20,"20% syst." ,"l");
  unc30->Draw("L");   leg->AddEntry(unc30,"30% syst." ,"l");

  int xmax=3;
  if(energy==33) xmax=8;
  if(energy==100) xmax=15;

  base->GetXaxis()->SetLimits(0,xmax);
  base->GetYaxis()->SetLimits(0,xmax);
  base->SetMaximum(xmax);
  base->SetMinimum(0);
  base->GetXaxis()->SetNdivisions(506);
  base->GetYaxis()->SetNdivisions(506);
  base->GetXaxis()->SetTitle(xaxis.c_str());
  base->GetYaxis()->SetTitle(yaxis.c_str());
  base->GetXaxis()->SetTitleSize(0.06);
  base->GetYaxis()->SetTitleSize(0.06);
  base->GetXaxis()->SetTitleOffset(1.0);
  base->GetYaxis()->SetTitleOffset(1.0);


  TLine* l = new TLine(0,0,xmax,xmax);
  l->SetLineColor(kGray);
  l->SetLineStyle(kDotted);
  l->SetLineWidth(2);
  l->Draw();
  
  leg->Draw();

  myText(0.21,0.87,kBlack,process.c_str(),-1,0.05);

  if(label=="limit")
    myText(0.21,0.80,kBlack,"95% CL exclusion",-1,0.05);
  else if(label=="disc")
    myText(0.21,0.80,kBlack,"5 #sigma discovery",-1,0.05);

  myText(0.55,0.87,kBlack,Form("#scale[.75]{#int}Ldt = %d fb^{-1}",lumi));
  myText(0.55,0.80,kBlack,Form("#sqrt{s}=%d TeV",energy));

  c->Print(((string)c->GetName()+".pdf").c_str());
}

void go_1d2d_comp(int energy=14,
		  int lumi=3000,
		  string label="disc",
		  string xaxis="m_{#tilde{g}} [TeV]",
		  string yaxis="m_{#tilde{#chi}_{1}^{0}} [TeV]",
		  string process="pp#rightarrow#tilde{g}#tilde{g}#rightarrowq#bar{q}#tilde{#chi}_{1}^{0}q#bar{q}#tilde{#chi}_{1}^{0}"){

  TFile* contours = new TFile("contours.root","RO");
  string gridname;
  if(energy==14){
    gridname="GO_decoupledSQ_14TeV_LHE_QQALL";
  }
  else if(energy==33){
    gridname="GO_decoupledSQ_33TeV_LHE_QQALL";
  }
  else if(energy==100){
    gridname="GO_decoupledSQ_100TeV";
  }

  TGraph* GOGO_000 = GetTGraph(contours,gridname,energy,0,lumi,"MET___HTjets_30_3p5",20,label,1,15);
  TGraph* GOGO_140 = GetTGraph(contours,gridname,energy,0,lumi,"m_eff"              ,20,label,1,15);

  int ngraphs=2;

  GOGO_000->SetLineColor(kBlack);  GOGO_000->SetLineStyle(kSolid);
  GOGO_140->SetLineColor(kRed);    GOGO_140->SetLineStyle(kDashed);

  TGraph* base=GOGO_000;

  TCanvas* c = new TCanvas(Form("c_1d2d_%dTeV_%difb_%s",energy,lumi,label.c_str()),"optimization comparison",800,600);
  float legymax=0.78;
  TLegend* leg = new TLegend(0.20,(legymax-0.06*ngraphs),0.55,legymax);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  GOGO_000->Draw("AL");   leg->AddEntry(GOGO_000,"E_{T}^{miss} and H_{T}" ,"l");
  GOGO_140->Draw("L");    leg->AddEntry(GOGO_140,"m_{eff}","l");

  int xmax=3;
  if(energy==33) xmax=8;
  if(energy==100) xmax=15;

  base->GetXaxis()->SetLimits(0,xmax);
  base->GetYaxis()->SetLimits(0,xmax);
  base->SetMinimum(0);
  base->SetMaximum(xmax);
  base->GetXaxis()->SetNdivisions(506);
  base->GetYaxis()->SetNdivisions(506);
  base->GetXaxis()->SetTitle(xaxis.c_str());
  base->GetYaxis()->SetTitle(yaxis.c_str());
  base->GetXaxis()->SetTitleSize(0.06);
  base->GetYaxis()->SetTitleSize(0.06);
  base->GetXaxis()->SetTitleOffset(1.0);
  base->GetYaxis()->SetTitleOffset(1.0);


  TLine* l = new TLine(0,0,xmax,xmax);
  l->SetLineColor(kGray);
  l->SetLineStyle(kDotted);
  l->SetLineWidth(2);
  l->Draw();
  
  leg->Draw();

  myText(0.21,0.87,kBlack,process.c_str(),-1,0.05);

  if(label=="limit")
    myText(0.21,0.80,kBlack,"95% CL exclusion",-1,0.05);
  else if(label=="disc")
    myText(0.21,0.80,kBlack,"5 #sigma discovery",-1,0.05);

  myText(0.55,0.87,kBlack,Form("#scale[.75]{#int}Ldt = %d fb^{-1}",lumi));
  myText(0.55,0.80,kBlack,Form("#sqrt{s}=%d TeV",energy));

  c->Print(((string)c->GetName()+".pdf").c_str());
}

void go_metsignif_comp(int energy=14,
		       int lumi=3000,
		       string label="disc",
		       string xaxis="m_{#tilde{g}} [TeV]",
		       string yaxis="m_{#tilde{#chi}_{1}^{0}} [TeV]",
		       string process="pp#rightarrow#tilde{g}#tilde{g}#rightarrowq#bar{q}#tilde{#chi}_{1}^{0}q#bar{q}#tilde{#chi}_{1}^{0}"){

  int newmetsignif=0;
  if(energy == 14) return;
  if(energy == 33) newmetsignif=23;
  if(energy == 100) newmetsignif=40;

  TFile* contours = new TFile("contours.root","RO");
  string gridname;
  if(energy==14){
    gridname="GO_decoupledSQ_14TeV_LHE_QQALL";
  }
  else if(energy==33){
    gridname="GO_decoupledSQ_33TeV_LHE_QQALL";
  }
  else if(energy==100){
    gridname="GO_decoupledSQ_100TeV";
  }

  TGraph* GOGO_000 = GetTGraph(contours,gridname,energy,0,lumi,"MET___HTjets_30_3p5",20,label,1,15);
  TGraph* GOGO_140 = GetTGraph(contours,gridname,energy,0,lumi,"MET___HTjets_30_3p5",20,label,1,newmetsignif);

  int ngraphs=2;

  GOGO_000->SetLineColor(kBlack);  GOGO_000->SetLineStyle(kSolid);
  GOGO_140->SetLineColor(kRed);    GOGO_140->SetLineStyle(kDashed);

  TGraph* base=GOGO_000;

  TCanvas* c = new TCanvas(Form("c_metsignif_%dTeV_%difb_%s",energy,lumi,label.c_str()),"metsignif comparison",800,600);
  float legymax=0.78;
  TLegend* leg = new TLegend(0.20,(legymax-0.06*ngraphs),0.55,legymax);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  GOGO_000->Draw("AL");   leg->AddEntry(GOGO_000,"E_{T}^{miss}/#sqrt{H_{T}} > 15" ,"l");
  GOGO_140->Draw("L");    leg->AddEntry(GOGO_140,Form("E_{T}^{miss}/#sqrt{H_{T}} > %d",newmetsignif),"l");

  int xmax=3;
  if(energy==33) xmax=8;
  if(energy==100) xmax=15;

  base->GetXaxis()->SetLimits(0,xmax);
  base->GetYaxis()->SetLimits(0,xmax);
  base->SetMinimum(0);
  base->SetMaximum(xmax);
  base->GetXaxis()->SetNdivisions(506);
  base->GetYaxis()->SetNdivisions(506);
  base->GetXaxis()->SetTitle(xaxis.c_str());
  base->GetYaxis()->SetTitle(yaxis.c_str());
  base->GetXaxis()->SetTitleSize(0.06);
  base->GetYaxis()->SetTitleSize(0.06);
  base->GetXaxis()->SetTitleOffset(1.0);
  base->GetYaxis()->SetTitleOffset(1.0);


  TLine* l = new TLine(0,0,xmax,xmax);
  l->SetLineColor(kGray);
  l->SetLineStyle(kDotted);
  l->SetLineWidth(2);
  l->Draw();
  
  leg->Draw();

  myText(0.21,0.87,kBlack,process.c_str(),-1,0.05);

  if(label=="limit")
    myText(0.21,0.80,kBlack,"95% CL exclusion",-1,0.05);
  else if(label=="disc")
    myText(0.21,0.80,kBlack,"5 #sigma discovery",-1,0.05);

  myText(0.55,0.87,kBlack,Form("#scale[.75]{#int}Ldt = %d fb^{-1}",lumi));
  myText(0.55,0.80,kBlack,Form("#sqrt{s}=%d TeV",energy));

  c->Print(((string)c->GetName()+".pdf").c_str());
}

void go_leadjet_comp(int energy=14,
		     int lumi=3000,
		     string label="disc",
		     string xaxis="m_{#tilde{q}} [TeV]",
		     string yaxis="m_{#tilde{#chi}_{1}^{0}} [TeV]",
		     string process="pp#rightarrow#tilde{q}#tilde{q}#rightarrowq#tilde{#chi}_{1}^{0}#bar{q}#tilde{#chi}_{1}^{0}"){

  TFile* contours = new TFile("contours.root","RO");
  string gridname;
  if(energy==14){
    gridname="SQ_decoupledGO_14TeV";
  }
  else if(energy==33){
    gridname="SQ_decoupledGO_33TeV";
  }
  else if(energy==100){
    gridname="SQ_decoupledGO_100TeV";
  }

  TGraph* GOGO_000 = GetTGraph(contours,gridname,energy,0,lumi,"MET___HTjets_30_3p5",20,label,1,15);
  TGraph* GOGO_140 = GetTGraph(contours,gridname,energy,0,lumi,"MET___HTjets_30_3p5",20,label,0,15);

  int ngraphs=2;

  GOGO_000->SetLineColor(kBlack);  GOGO_000->SetLineStyle(kSolid);
  GOGO_140->SetLineColor(kRed);    GOGO_140->SetLineStyle(kDashed);

  TGraph* base=GOGO_000;

  TCanvas* c = new TCanvas(Form("c_leadjet_%dTeV_%difb_%s",energy,lumi,label.c_str()),"leadjet cut comparison",800,600);
  float legymax=0.78;
  TLegend* leg = new TLegend(0.20,(legymax-0.06*ngraphs),0.55,legymax);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  GOGO_000->Draw("AL");   leg->AddEntry(GOGO_000,"p_{T}^{lead jet}/H_{T} < 0.4" ,"l");
  GOGO_140->Draw("L");    leg->AddEntry(GOGO_140,"No cut","l");

  int xmax=3;
  if(energy==33) xmax=8;
  if(energy==100) xmax=15;

  base->GetXaxis()->SetLimits(0,xmax);
  base->GetYaxis()->SetLimits(0,xmax);
  base->SetMinimum(0);
  base->SetMaximum(xmax);
  base->GetXaxis()->SetNdivisions(506);
  base->GetYaxis()->SetNdivisions(506);
  base->GetXaxis()->SetTitle(xaxis.c_str());
  base->GetYaxis()->SetTitle(yaxis.c_str());
  base->GetXaxis()->SetTitleSize(0.06);
  base->GetYaxis()->SetTitleSize(0.06);
  base->GetXaxis()->SetTitleOffset(1.0);
  base->GetYaxis()->SetTitleOffset(1.0);


  TLine* l = new TLine(0,0,xmax,xmax);
  l->SetLineColor(kGray);
  l->SetLineStyle(kDotted);
  l->SetLineWidth(2);
  l->Draw();
  
  leg->Draw();

  myText(0.21,0.87,kBlack,process.c_str(),-1,0.05);

  if(label=="limit")
    myText(0.21,0.80,kBlack,"95% CL exclusion",-1,0.05);
  else if(label=="disc")
    myText(0.21,0.80,kBlack,"5 #sigma discovery",-1,0.05);

  myText(0.55,0.87,kBlack,Form("#scale[.75]{#int}Ldt = %d fb^{-1}",lumi));
  myText(0.55,0.80,kBlack,Form("#sqrt{s}=%d TeV",energy));

  c->Print(((string)c->GetName()+".pdf").c_str());
}

void make_all_comp(){
  go_pileup_comp(14,3000,"disc");
  go_pileup_comp(14,3000,"limit");

  go_uncertainty_comp(14,3000,"disc");
  go_uncertainty_comp(14,3000,"limit");
  go_uncertainty_comp(100,3000,"disc");
  go_uncertainty_comp(100,3000,"limit");

  go_1d2d_comp(14,3000,"disc");
  go_1d2d_comp(14,3000,"limit");

  go_metsignif_comp(33,3000,"disc");
  go_metsignif_comp(33,3000,"limit");
  go_metsignif_comp(100,3000,"disc");
  go_metsignif_comp(100,3000,"limit");

  go_leadjet_comp(14,3000,"disc");
  go_leadjet_comp(14,3000,"limit");
  go_leadjet_comp(33,3000,"disc");
  go_leadjet_comp(33,3000,"limit");
  go_leadjet_comp(100,3000,"disc");
  go_leadjet_comp(100,3000,"limit");
}


void ForceAscending(TGraph* g,int startpoint=0){
  double prev=-1.,prevprev=-1.;
  double x,y;
  int n=g->GetN();
  for(int i=0; i<=n; i++){
    g->GetPoint(i,x,y);
    //cout << g->GetN() << "\t" << i << "\t" << prev << "\t" << y << "\t" << x << endl;
    if(i!=0 && y < prev && y < prevprev && i>startpoint){
      for(int j=i; j<=n; j++){
	//cout << "--- " << j << "\t" << i << endl;
	g->RemovePoint(i);
      }
      break;
    }
    prevprev=prev;
    prev=y;
  }
}

void go_2dcut_summary(string var="masscut",
		      string yaxis="Optimal cut [TeV]",
		      string xaxis="m_{#tilde{g}} [TeV]",
		      string process="pp#rightarrow#tilde{g}#tilde{g}#rightarrowq#bar{q}#tilde{#chi}_{1}^{0}q#bar{q}#tilde{#chi}_{1}^{0}"){

  bool force300fb=false;

  TFile* contours = new TFile("meffcuts.root","RO");

  TGraph* GOGO_14_0300  = (TGraph*)contours->Get(Form("%s_mlsp1",
						      PlotName("GO_decoupledSQ_14TeV_LHE_QQALL", 14,0, 300,"MET___HTjets_30_3p5",20,var,1,15).c_str()));
  TGraph* GOGO_14_3000  = (TGraph*)contours->Get(Form("%s_mlsp1",
						      PlotName("GO_decoupledSQ_14TeV_LHE_QQALL", 14,0,3000,"MET___HTjets_30_3p5",20,var,1,15).c_str()));
  TGraph* GOGO_33_3000  = (TGraph*)contours->Get(Form("%s_mlsp1",
						      PlotName("GO_decoupledSQ_33TeV_LHE_QQALL", 33,0,3000,"MET___HTjets_30_3p5",20,var,1,15).c_str()));
  TGraph* GOGO_100_3000 = (TGraph*)contours->Get(Form("%s_mlsp1",
						      PlotName("GO_decoupledSQ_100TeV"         ,100,0,3000,"MET___HTjets_30_3p5",20,var,1,15).c_str()));
  if(force300fb)
    GOGO_100_3000 = (TGraph*)contours->Get(Form("%s_mlsp1",
						PlotName("GO_decoupledSQ_100TeV"            ,100,0, 300,"MET___HTjets_30_3p5",20,var,1,15).c_str()));

  int ngraphs=4;

  cout << GOGO_14_0300 << "\t" << GOGO_14_3000 << "\t" << GOGO_33_3000 << "\t" << GOGO_100_3000 << endl;

  GOGO_14_0300  ->SetLineColor(kBlack);   GOGO_14_0300->SetLineStyle(kSolid);  GOGO_14_0300->SetLineWidth(3);   
  GOGO_14_3000  ->SetLineColor(kBlue);    GOGO_14_3000->SetLineStyle(kSolid);  GOGO_14_3000->SetLineWidth(3);   
  GOGO_33_3000  ->SetLineColor(kRed );    GOGO_33_3000->SetLineStyle(kSolid);  GOGO_33_3000->SetLineWidth(3);   
  GOGO_100_3000 ->SetLineColor(kGreen-2); GOGO_100_3000->SetLineStyle(kSolid);  GOGO_100_3000->SetLineWidth(3);   

  ForceAscending(GOGO_14_3000,9);
  ForceAscending(GOGO_14_0300,8);
  ForceAscending(GOGO_33_3000,12);
  ForceAscending(GOGO_100_3000,11);

  for(int i=0; i<4; i++)
    GOGO_14_3000->RemovePoint(0);
  for(int i=0; i<4; i++)
    GOGO_14_0300->RemovePoint(0);
  GOGO_33_3000->RemovePoint(0);

  TGraph* base=GOGO_100_3000;

  base->GetXaxis()->SetNdivisions(506);
  base->GetYaxis()->SetNdivisions(506);
  base->GetXaxis()->SetTitle(xaxis.c_str());
  base->GetYaxis()->SetTitle(yaxis.c_str());

  base->GetHistogram()->SetMinimum(0);
  int xhigh=16;
  int yhigh=16;
  if(var=="masscut2")
    yhigh=30;
  else if(var=="masscut2")
    yhigh=40;
  
  if(force300fb){
    xhigh=14;
    if(var=="masscut")
      yhigh=10;
    else if(var=="masscut2")
      yhigh=16;
    else if(var=="masscutsum")
      yhigh=24;
  }
  base->GetHistogram()->SetMaximum(yhigh);
  base->GetHistogram()->GetXaxis()->SetLimits(0,xhigh);
  base->GetHistogram()->GetXaxis()->SetRangeUser(0,xhigh);


  TCanvas* c = new TCanvas(Form("c_summary_cuts_%s",var.c_str()),Form("cuts on %s",var.c_str()),800,600);
  float legymax=0.78;
  TLegend* leg = new TLegend(0.20,(legymax-0.06*ngraphs),0.55,legymax);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);

  if(!force300fb){
    GOGO_100_3000 ->Draw("AL");   leg->AddEntry(GOGO_100_3000,"100 TeV, 3000 fb^{-1}","l");
  }
  else{
    GOGO_100_3000 ->Draw("AL");   leg->AddEntry(GOGO_100_3000,"100 TeV, 300 fb^{-1}","l");
  }
  GOGO_33_3000  ->Draw("L");    leg->AddEntry(GOGO_33_3000 ,"33 TeV, 3000 fb^{-1}" ,"l");
  GOGO_14_3000  ->Draw("L");    leg->AddEntry(GOGO_14_3000 ,"14 TeV, 3000 fb^{-1}" ,"l");
  GOGO_14_0300  ->Draw("L");    leg->AddEntry(GOGO_14_0300 ,"14 TeV, 300 fb^{-1}"  ,"l");

  leg->Draw();

  myText(0.21,0.87,kBlack,process.c_str(),-1,0.05);

  myText(0.21,0.80,kBlack,"m_{#tilde{#chi}_{1}^{0}}=1 GeV",-1,0.05);


  c->Print(((string)c->GetName()+".pdf").c_str());
  c->Print(((string)c->GetName()+".png").c_str());
}


void go_2dcut_summary2(bool drawSum=true,
		       string yaxis="Cut Value [TeV]",
		       string xaxis="m_{#tilde{g}} [TeV]",
		       string process="pp#rightarrow#tilde{g}#tilde{g}#rightarrowq#bar{q}#tilde{#chi}_{1}^{0}q#bar{q}#tilde{#chi}_{1}^{0}"){

  bool force300fb=false;

  TFile* contours = new TFile("meffcuts.root","RO");

  string var="masscut";
  TGraph* GOGO_14_0300_met  = (TGraph*)contours->Get(Form("%s_mlsp1",
							  PlotName("GO_decoupledSQ_14TeV_LHE_QQALL", 14,0, 300,"MET___HTjets_30_3p5",20,var,1,15).c_str()));
  TGraph* GOGO_14_3000_met  = (TGraph*)contours->Get(Form("%s_mlsp1",
							  PlotName("GO_decoupledSQ_14TeV_LHE_QQALL", 14,0,3000,"MET___HTjets_30_3p5",20,var,1,15).c_str()));
  TGraph* GOGO_33_3000_met  = (TGraph*)contours->Get(Form("%s_mlsp1",
							  PlotName("GO_decoupledSQ_33TeV_LHE_QQALL", 33,0,3000,"MET___HTjets_30_3p5",20,var,1,15).c_str()));
  TGraph* GOGO_100_3000_met = (TGraph*)contours->Get(Form("%s_mlsp1",
							  PlotName("GO_decoupledSQ_100TeV"            ,100,0,3000,"MET___HTjets_30_3p5",20,var,1,15).c_str()));
  var="masscut2";
  TGraph* GOGO_14_0300_ht  = (TGraph*)contours->Get(Form("%s_mlsp1",
							  PlotName("GO_decoupledSQ_14TeV_LHE_QQALL", 14,0, 300,"MET___HTjets_30_3p5",20,var,1,15).c_str()));
  TGraph* GOGO_14_3000_ht  = (TGraph*)contours->Get(Form("%s_mlsp1",
							  PlotName("GO_decoupledSQ_14TeV_LHE_QQALL", 14,0,3000,"MET___HTjets_30_3p5",20,var,1,15).c_str()));
  TGraph* GOGO_33_3000_ht  = (TGraph*)contours->Get(Form("%s_mlsp1",
							  PlotName("GO_decoupledSQ_33TeV_LHE_QQALL", 33,0,3000,"MET___HTjets_30_3p5",20,var,1,15).c_str()));
  TGraph* GOGO_100_3000_ht = (TGraph*)contours->Get(Form("%s_mlsp1",
							  PlotName("GO_decoupledSQ_100TeV"            ,100,0,3000,"MET___HTjets_30_3p5",20,var,1,15).c_str()));

  var="masscutsum";
  TGraph* GOGO_14_0300_sum  = (TGraph*)contours->Get(Form("%s_mlsp1",
							  PlotName("GO_decoupledSQ_14TeV_LHE_QQALL", 14,0, 300,"MET___HTjets_30_3p5",20,var,1,15).c_str()));
  TGraph* GOGO_14_3000_sum  = (TGraph*)contours->Get(Form("%s_mlsp1",
							  PlotName("GO_decoupledSQ_14TeV_LHE_QQALL", 14,0,3000,"MET___HTjets_30_3p5",20,var,1,15).c_str()));
  TGraph* GOGO_33_3000_sum  = (TGraph*)contours->Get(Form("%s_mlsp1",
							  PlotName("GO_decoupledSQ_33TeV_LHE_QQALL", 33,0,3000,"MET___HTjets_30_3p5",20,var,1,15).c_str()));
  TGraph* GOGO_100_3000_sum = (TGraph*)contours->Get(Form("%s_mlsp1",
							  PlotName("GO_decoupledSQ_100TeV"            ,100,0,3000,"MET___HTjets_30_3p5",20,var,1,15).c_str()));

  if(force300fb){
    GOGO_100_3000_met = (TGraph*)contours->Get(Form("%s_mlsp1",
						PlotName("GO_decoupledSQ_100TeV"            ,100,0, 300,"MET___HTjets_30_3p5",20,"masscut",1,15).c_str()));
    GOGO_100_3000_ht  = (TGraph*)contours->Get(Form("%s_mlsp1",
						PlotName("GO_decoupledSQ_100TeV"            ,100,0, 300,"MET___HTjets_30_3p5",20,"masscut2",1,15).c_str()));
    GOGO_100_3000_sum = (TGraph*)contours->Get(Form("%s_mlsp1",
						PlotName("GO_decoupledSQ_100TeV"            ,100,0, 300,"MET___HTjets_30_3p5",20,"masscutsum",1,15).c_str()));
  }    

  int ngraphs=4;

  GOGO_14_0300_met  ->SetLineColor(kBlack);   GOGO_14_0300_met->SetLineStyle(kDotted);  GOGO_14_0300_met->SetLineWidth(3);   
  GOGO_14_3000_met  ->SetLineColor(kBlue);    GOGO_14_3000_met->SetLineStyle(kDotted);  GOGO_14_3000_met->SetLineWidth(3);   
  GOGO_33_3000_met  ->SetLineColor(kRed );    GOGO_33_3000_met->SetLineStyle(kDotted);  GOGO_33_3000_met->SetLineWidth(3);   
  GOGO_100_3000_met ->SetLineColor(kGreen-2); GOGO_100_3000_met->SetLineStyle(kDotted);  GOGO_100_3000_met->SetLineWidth(3);   

  GOGO_14_0300_ht  ->SetLineColor(kBlack);   GOGO_14_0300_ht->SetLineStyle(kDashed);  GOGO_14_0300_ht->SetLineWidth(3);   
  GOGO_14_3000_ht  ->SetLineColor(kBlue);    GOGO_14_3000_ht->SetLineStyle(kDashed);  GOGO_14_3000_ht->SetLineWidth(3);   
  GOGO_33_3000_ht  ->SetLineColor(kRed );    GOGO_33_3000_ht->SetLineStyle(kDashed);  GOGO_33_3000_ht->SetLineWidth(3);   
  GOGO_100_3000_ht ->SetLineColor(kGreen-2); GOGO_100_3000_ht->SetLineStyle(kDashed);  GOGO_100_3000_ht->SetLineWidth(3);   

  if(!drawSum){
    GOGO_14_0300_ht->SetLineStyle(kSolid); 
    GOGO_14_3000_ht->SetLineStyle(kSolid); 
    GOGO_33_3000_ht->SetLineStyle(kSolid); 
    GOGO_100_3000_ht->SetLineStyle(kSolid);
  }

  GOGO_14_0300_sum  ->SetLineColor(kBlack);   GOGO_14_0300_sum->SetLineStyle(kSolid);  GOGO_14_0300_sum->SetLineWidth(3);   
  GOGO_14_3000_sum  ->SetLineColor(kBlue);    GOGO_14_3000_sum->SetLineStyle(kSolid);  GOGO_14_3000_sum->SetLineWidth(3);   
  GOGO_33_3000_sum  ->SetLineColor(kRed );    GOGO_33_3000_sum->SetLineStyle(kSolid);  GOGO_33_3000_sum->SetLineWidth(3);   
  GOGO_100_3000_sum ->SetLineColor(kGreen-2); GOGO_100_3000_sum->SetLineStyle(kSolid);  GOGO_100_3000_sum->SetLineWidth(3);   

  ForceAscending(GOGO_14_3000_met,9);
  ForceAscending(GOGO_14_0300_met,8);
  ForceAscending(GOGO_33_3000_met,12);
  ForceAscending(GOGO_100_3000_met,11);

  ForceAscending(GOGO_14_3000_ht,9);
  ForceAscending(GOGO_14_0300_ht,8);
  ForceAscending(GOGO_33_3000_ht,12);
  ForceAscending(GOGO_100_3000_ht,11);

  ForceAscending(GOGO_14_3000_sum,9);
  ForceAscending(GOGO_14_0300_sum,8);
  ForceAscending(GOGO_33_3000_sum,12);
  ForceAscending(GOGO_100_3000_sum,11);

  for(int i=0; i<4; i++){
    GOGO_14_3000_met->RemovePoint(0);
    GOGO_14_3000_ht ->RemovePoint(0);
    GOGO_14_3000_sum->RemovePoint(0);
  }
  for(int i=0; i<4; i++){
    GOGO_14_0300_met->RemovePoint(0);
    GOGO_14_0300_ht ->RemovePoint(0);
    GOGO_14_0300_sum->RemovePoint(0);
  }
  GOGO_33_3000_met->RemovePoint(0);
  GOGO_33_3000_ht ->RemovePoint(0);
  GOGO_33_3000_sum->RemovePoint(0);

  TGraph* base=GOGO_100_3000_sum;
  if(!drawSum)
    base=GOGO_100_3000_ht;

  base->GetXaxis()->SetNdivisions(506);
  base->GetYaxis()->SetNdivisions(506);
  base->GetXaxis()->SetTitle(xaxis.c_str());
  base->GetYaxis()->SetTitle(yaxis.c_str());

  base->GetHistogram()->SetMinimum(0);
  int xhigh=16;
  int yhigh=25;
  
  if(force300fb){
    xhigh=15;
  }

  if(!drawSum){
    yhigh=16;
  }

  base->GetHistogram()->SetMaximum(yhigh);
  base->GetHistogram()->GetXaxis()->SetLimits(0,xhigh);
  base->GetHistogram()->GetXaxis()->SetRangeUser(0,xhigh);


  TCanvas* c = new TCanvas(Form("c_summary_cuts_%s", (drawSum ? "wsum":"wosum")),"cuts",800,600);
  float legymax=0.78;
  TLegend* leg = new TLegend(0.20,(legymax-0.06*ngraphs),0.55,legymax);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);

  if(drawSum){
    if(!force300fb){
      GOGO_100_3000_sum ->Draw("AL");   leg->AddEntry(GOGO_100_3000_sum,"100 TeV, 3000 fb^{-1}","l");
    }
    else{
      GOGO_100_3000_sum ->Draw("AL");   leg->AddEntry(GOGO_100_3000_sum,"100 TeV, 300 fb^{-1}","l");
    }
    GOGO_33_3000_sum  ->Draw("L");    leg->AddEntry(GOGO_33_3000_sum ,"33 TeV, 3000 fb^{-1}" ,"l");
    GOGO_14_3000_sum  ->Draw("L");    leg->AddEntry(GOGO_14_3000_sum ,"14 TeV, 3000 fb^{-1}" ,"l");
    GOGO_14_0300_sum  ->Draw("L");    leg->AddEntry(GOGO_14_0300_sum ,"14 TeV, 300 fb^{-1}"  ,"l");

    GOGO_100_3000_ht ->Draw("L");
    GOGO_33_3000_ht  ->Draw("L");
    GOGO_14_3000_ht  ->Draw("L");
    GOGO_14_0300_ht  ->Draw("L");
  }
  else{
    if(!force300fb){
      GOGO_100_3000_ht ->Draw("AL");   leg->AddEntry(GOGO_100_3000_ht,"100 TeV, 3000 fb^{-1}","l");
    }
    else{
      GOGO_100_3000_ht ->Draw("AL");   leg->AddEntry(GOGO_100_3000_ht,"100 TeV, 300 fb^{-1}","l");
    }
    GOGO_33_3000_ht  ->Draw("L");    leg->AddEntry(GOGO_33_3000_ht ,"33 TeV, 3000 fb^{-1}" ,"l");
    GOGO_14_3000_ht  ->Draw("L");    leg->AddEntry(GOGO_14_3000_ht ,"14 TeV, 3000 fb^{-1}" ,"l");
    GOGO_14_0300_ht  ->Draw("L");    leg->AddEntry(GOGO_14_0300_ht ,"14 TeV, 300 fb^{-1}"  ,"l");
  }

  GOGO_100_3000_met ->Draw("L");
  GOGO_33_3000_met  ->Draw("L");
  GOGO_14_3000_met  ->Draw("L");
  GOGO_14_0300_met  ->Draw("L");

  leg->Draw();

  myText(0.21,0.87,kBlack,process.c_str(),-1,0.05);

  myText(0.21,0.80,kBlack,"m_{#tilde{#chi}_{1}^{0}}=1 GeV",-1,0.05);


  if(drawSum){
    myText(0.69,0.70,kBlack,"H_{T}+E_{T}^{miss}",-1,0.06);
    myText(0.79,0.50,kBlack,"H_{T}",-1,0.06);
    myText(0.79,0.30,kBlack,"E_{T}^{miss}",-1,0.06);
  }
  else{
    myText(0.79,0.70,kBlack,"H_{T}",-1,0.06);
    myText(0.79,0.30,kBlack,"E_{T}^{miss}",-1,0.06);
  }

  c->Print(((string)c->GetName()+".pdf").c_str());
  c->Print(((string)c->GetName()+".png").c_str());
}


void go_2dcut_summary_energy(int energy=14,
			     int lumi=3000,
			     int relunc=20,
			     string xaxis="m_{#tilde{g}} [TeV]",
			     string yaxis="Optimal cut [TeV]",
			     string process="pp#rightarrow#tilde{g}#tilde{g}#rightarrowq#bar{q}#tilde{#chi}_{1}^{0}q#bar{q}#tilde{#chi}_{1}^{0}"){

  TFile* contours = new TFile("meffcuts.root","RO");

  string gridname="";
  if(energy==14){
    gridname="GO_decoupledSQ_14TeV_LHE_QQALL";
  }
  else if(energy==33){
    gridname="GO_decoupledSQ_33TeV_LHE_QQALL";
  }
  else if(energy==100){
    gridname="GO_decoupledSQ_100TeV";
  }

  TGraph* meff = (TGraph*)contours->Get(Form("%s_mlsp1",
					     PlotName(gridname,energy,0,lumi,"m_eff",relunc,"masscut",1,15).c_str()));
  TGraph* met  = (TGraph*)contours->Get(Form("%s_mlsp1",
					     PlotName(gridname,energy,0,lumi,"MET___HTjets_30_3p5",relunc,"masscut",1,15).c_str()));
  TGraph* ht   = (TGraph*)contours->Get(Form("%s_mlsp1",
					     PlotName(gridname,energy,0,lumi,"MET___HTjets_30_3p5",relunc,"masscut2",1,15).c_str()));
  TGraph* comb = (TGraph*)contours->Get(Form("%s_mlsp1",
					     PlotName(gridname,energy,0,lumi,"MET___HTjets_30_3p5",relunc,"masscutsum",1,15).c_str()));

  if(energy==14){
    meff->SetLineColor(kBlack);  meff->SetLineStyle(kDashed); meff->SetLineWidth(3);   
  }
  met ->SetLineColor(kBlue);   met ->SetLineStyle(kSolid);  met ->SetLineWidth(3);   
  ht  ->SetLineColor(kRed );   ht  ->SetLineStyle(kSolid);  ht  ->SetLineWidth(3);   
  comb->SetLineColor(kGray+1); comb->SetLineStyle(kSolid);  comb->SetLineWidth(3);   

  if(energy==100 && lumi==3000){
    ForceAscending(met,12);
    ForceAscending(ht,11);
    ForceAscending(comb,12);
  }
  else if(energy==100){
    ForceAscending(met,11);
    ForceAscending(ht,10);
    ForceAscending(comb,10);
  }
  else{
    if(meff) ForceAscending(meff);
    ForceAscending(met,10);
    ForceAscending(ht);
    ForceAscending(comb);
  }

  if(energy==33){
    // for some reason the first points have optimal cuts that are very high
    met ->RemovePoint(0);
    ht  ->RemovePoint(0);
    comb->RemovePoint(0);
  }

  TGraph* base=meff;
  if(energy!=14)
    base=comb;

  base->GetXaxis()->SetNdivisions(506);
  base->GetYaxis()->SetNdivisions(510);
  base->GetXaxis()->SetTitle(xaxis.c_str());
  base->GetYaxis()->SetTitle(yaxis.c_str());

  int xmax=4,max=10;
  if(energy==33){
    xmax=7;
    max=15;
  }
  if(energy==100){
    xmax=18;
    max=40;
  }

  base->GetHistogram()->SetMinimum(0);
  base->GetHistogram()->SetMaximum(max);
  base->GetHistogram()->GetXaxis()->SetLimits(0,xmax);
  base->GetHistogram()->GetXaxis()->SetRangeUser(0,xmax);


  TCanvas* c = new TCanvas(Form("c_summary_%dTeV_masscuts",energy),"summary",800,600);
  float legymax=0.78;
  float legysize=0.24;
  if(energy!=14) legysize=0.18;
  TLegend* leg = new TLegend(0.20,(legymax-legysize),0.55,legymax);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);

  if(energy==14){
    meff->Draw("AL");   leg->AddEntry(meff,"m_{eff}","l");
    comb->Draw("L");    leg->AddEntry(comb,"E_{T}^{miss}+H_{T}" ,"l");
  }
  else{
    comb->Draw("AL");    leg->AddEntry(comb,"E_{T}^{miss}+H_{T}" ,"l");
  }
  met ->Draw("L");    leg->AddEntry(met ,"E_{T}^{miss}" ,"l");
  ht  ->Draw("L");    leg->AddEntry(ht  ,"H_{T}" ,"l");

  leg->Draw();

  myText(0.21,0.87,kBlack,process.c_str(),-1,0.05);

  myText(0.21,0.80,kBlack,"m_{#tilde{#chi}_{1}^{0}}=1 GeV",-1,0.05);


  c->Print(((string)c->GetName()+".pdf").c_str());
  c->Print(((string)c->GetName()+".png").c_str());
}

void make_all_summaries(){
  make_all_comp();
  go_final_summaries();
  //go_meff_summary_14();
  go_2dcut_summary_energy(14);
  go_2dcut_summary_energy(33);
  go_2dcut_summary_energy(100,300);
  go_2dcut_summary_energy(100,3000);
  
  go_2dcut_summary("masscut", "Optimal E_{T}^{miss} Cut [TeV]");
  go_2dcut_summary("masscut2", "Optimal H_{T} Cut [TeV]");
  go_2dcut_summary("masscutsum", "Optimal E_{T}^{miss} Cut + Optimal H_{T} Cut [TeV]");
  go_2dcut_summary2(0);
  go_2dcut_summary2(1);
}
