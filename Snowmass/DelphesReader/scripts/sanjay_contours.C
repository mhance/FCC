#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstring>

#include "TFile.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TAxis.h"
#include "TLine.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Utility function
//
void myText(Double_t x,Double_t y,Color_t color,const char *text, int font=-1, double tsize=.05) {

  TLatex* l = new TLatex; //l.SetTextAlign(12); 
  l->SetTextSize(tsize);
  l->SetNDC(kTRUE);
  l->SetTextColor(color);
  if(font>0) l->SetTextFont(font);
  l->DrawLatex(x,y,text);
}
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
//
// GOGO grid
//
void go_final_summary_GOGO(string label="disc",
			   string xaxis="m_{#tilde{g}} [TeV]",
			   string yaxis="m_{#tilde{#chi}_{1}^{0}} [TeV]",
			   string process="pp#rightarrow#tilde{g}#tilde{g}#rightarrowq#bar{q}#tilde{#chi}_{1}^{0}q#bar{q}#tilde{#chi}_{1}^{0}"){

  TFile* contours = new TFile("contours.root","RO");
  TGraph* GOGO_14_000_0300 = (TGraph*)contours->Get(Form("GO_decoupledSQ_14TeV_LHE_QQALL_14TeV_0Pileup_300ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));
  TGraph* GOGO_14_000_3000 = (TGraph*)contours->Get(Form("GO_decoupledSQ_14TeV_LHE_QQALL_14TeV_0Pileup_3000ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));
  TGraph* GOGO_33_000_3000 = (TGraph*)contours->Get(Form("GO_decoupledSQ_33TeV_LHE_QQALL_33TeV_0Pileup_3000ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));
  TGraph* GOGO_100_000_3000 = (TGraph*)contours->Get(Form("GO_decoupledSQ_100TeV_100TeV_0Pileup_3000ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));

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
  TLegend* leg = new TLegend(0.20,(legymax-0.2),0.55,legymax);
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
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
//
// SQSQ grid
// basically the same as the GOGO grid above.
//
void go_final_summary_SQSQ(string label="disc",
			   string xaxis="m_{#tilde{q}} [TeV]",
			   string yaxis="m_{#tilde{#chi}_{1}^{0}} [TeV]",
			   string process="pp#rightarrow#tilde{q}#tilde{q}#rightarrowq#tilde{#chi}_{1}^{0}q#tilde{#chi}_{1}^{0}"){

  TFile* contours = new TFile("contours.root","RO");

  TGraph* GOGO_14_000_0300;
  TGraph* GOGO_14_000_3000;
  TGraph* GOGO_33_000_3000;
  TGraph* GOGO_100_000_3000;

  GOGO_14_000_0300 = (TGraph*)contours->Get(Form("SQ_decoupledGO_14TeV_14TeV_0Pileup_300ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));
  GOGO_14_000_3000 = (TGraph*)contours->Get(Form("SQ_decoupledGO_14TeV_14TeV_0Pileup_3000ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));
  GOGO_33_000_3000 = (TGraph*)contours->Get(Form("SQ_decoupledGO_33TeV_33TeV_0Pileup_3000ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));
  GOGO_100_000_3000= (TGraph*)contours->Get(Form("SQ_decoupledGO_100TeV_100TeV_0Pileup_3000ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));

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
  TLegend* leg = new TLegend(0.20,(legymax-0.2),0.55,legymax);
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
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
//
// GOSQ grid
// this will look a little bit different from the other two
// also I include dashed lines to indicate the range of the grid that we simulated
//
void go_final_summary_GOSQ(string label="disc",
			   string xaxis="m_{#tilde{g}} [TeV]",
			   string yaxis="m_{#tilde{q}} [TeV]",
			   string process="pp#rightarrow#tilde{g}#tilde{g},#tilde{g}#tilde{q},#tilde{q}#tilde{q}"){

  TFile* contours = new TFile("contours.root","RO");
  TGraph* GOGO_14_000_0300 = (TGraph*)contours->Get(Form("GOSQ_masslessN0_14TeV_14TeV_0Pileup_300ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));
  TGraph* GOGO_14_000_3000 = (TGraph*)contours->Get(Form("GOSQ_masslessN0_14TeV_14TeV_0Pileup_3000ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));
  TGraph* GOGO_33_000_3000 = (TGraph*)contours->Get(Form("GOSQ_masslessN0_33TeV_33TeV_0Pileup_3000ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));
  TGraph* GOGO_100_000_3000 = (TGraph*)contours->Get(Form("GOSQ_masslessN0_100TeV_100TeV_0Pileup_3000ifb_MET___HTjets_30_3p5_unc20_%s",label.c_str()));


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
  TLegend* leg = new TLegend(0.20,(legymax-0.2),0.55,legymax);
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

  leg->Draw();

  myText(0.21,0.87,kBlack,process.c_str(),-1,0.05);

  if(label=="limit")
    myText(0.21,0.80,kBlack,"95% CL exclusion",-1,0.05);
  else if(label=="disc")
    myText(0.21,0.80,kBlack,"5 #sigma discovery",-1,0.05);


  c->Print(((string)c->GetName()+".pdf").c_str());
}
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
//
// Convenience function.
//
void go_final_summaries(){
  go_final_summary_GOGO("limit");
  go_final_summary_GOGO("disc");
  go_final_summary_SQSQ("limit");
  go_final_summary_SQSQ("disc");
  go_final_summary_GOSQ("limit");
  go_final_summary_GOSQ("disc");
}
////////////////////////////////////////////////////////////////////////////////////////////////
