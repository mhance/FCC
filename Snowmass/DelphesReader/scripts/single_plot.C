#include <iostream>
#include <iomanip>
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TChain.h"
#include "THStack.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TH2F.h"

bool scaleTo3ab=true;

using namespace std;

void myText(Double_t x,Double_t y,Color_t color,const char *text, int font=-1, double tsize=.05) {

  TLatex* l = new TLatex; //l.SetTextAlign(12); 
  l->SetTextSize(tsize);
  l->SetNDC(kTRUE);
  l->SetTextColor(color);
  if(font>0) l->SetTextFont(font);
  l->DrawLatex(x,y,text);
}

TCanvas* make_plot(string variable="m_eff",
		   string extracut="", 
		   string varlabel="m_{eff} [GeV]", 
		   string yaxislabel="fb/200 GeV", 
		   float min=0, 
		   float max=10000, 
		   float xscale=1.){

  if(scaleTo3ab && yaxislabel == "fb/200 GeV")
    yaxislabel = "Events / 200 GeV";

  std::vector<int> signal_colors;
  signal_colors.push_back(kRed-4);
  signal_colors.push_back(kRed-2);
  signal_colors.push_back(kRed);
  signal_colors.push_back(kRed+2);
  signal_colors.push_back(kRed+4);
  std::vector<TFile*> signal_files;
  std::vector<string> signal_labels;
    signal_files.push_back(new TFile("LHE_GOGO_SQ/delphes.LHE_GOGO_SQ.NoPileUpKiel.GO00500.qq.N00001_14TEV.root")); signal_labels.push_back("m_{#tilde{#chi}_{1}^{0}}=1 GeV, m_{#tilde{g}}=500  GeV");
  //signal_files.push_back(new TFile("LHE_GOGO_SQ/delphes.LHE_GOGO_SQ.NoPileUpKiel.GO00629.qq.N00001_14TEV.root")); signal_labels.push_back("m_{#tilde{#chi}_{1}^{0}}=1 GeV, m_{#tilde{g}}=629  GeV");
  //signal_files.push_back(new TFile("LHE_GOGO_SQ/delphes.LHE_GOGO_SQ.NoPileUpKiel.GO00792.qq.N00001_14TEV.root")); signal_labels.push_back("m_{#tilde{#chi}_{1}^{0}}=1 GeV, m_{#tilde{g}}=792  GeV");
  //signal_files.push_back(new TFile("LHE_GOGO_SQ/delphes.LHE_GOGO_SQ.NoPileUpKiel.GO00997.qq.N00001_14TEV.root")); signal_labels.push_back("m_{#tilde{#chi}_{1}^{0}}=1 GeV, m_{#tilde{g}}=997  GeV");
    signal_files.push_back(new TFile("LHE_GOGO_SQ/delphes.LHE_GOGO_SQ.NoPileUpKiel.GO01255.qq.N00001_14TEV.root")); signal_labels.push_back("m_{#tilde{#chi}_{1}^{0}}=1 GeV, m_{#tilde{g}}=1255 GeV");
  //signal_files.push_back(new TFile("LHE_GOGO_SQ/delphes.LHE_GOGO_SQ.NoPileUpKiel.GO01580.qq.N00001_14TEV.root")); signal_labels.push_back("m_{#tilde{#chi}_{1}^{0}}=1 GeV, m_{#tilde{g}}=1580 GeV");
  //signal_files.push_back(new TFile("LHE_GOGO_SQ/delphes.LHE_GOGO_SQ.NoPileUpKiel.GO01989.qq.N00001_14TEV.root")); signal_labels.push_back("m_{#tilde{#chi}_{1}^{0}}=1 GeV, m_{#tilde{g}}=1989 GeV");
    signal_files.push_back(new TFile("LHE_GOGO_SQ/delphes.LHE_GOGO_SQ.NoPileUpKiel.GO02489.qq.N00001_14TEV.root")); signal_labels.push_back("m_{#tilde{#chi}_{1}^{0}}=1 GeV, m_{#tilde{g}}=2489 GeV");
    signal_files.push_back(new TFile("LHE_GOGO_SQ/delphes.LHE_GOGO_SQ.NoPileUpKiel.GO03489.qq.N00001_14TEV.root")); signal_labels.push_back("m_{#tilde{#chi}_{1}^{0}}=1 GeV, m_{#tilde{g}}=3489 GeV");

  std::vector<TTree*> signal_trees;
  for(unsigned int i=0; i<signal_files.size(); i++){
    signal_trees.push_back((TTree*)signal_files[i]->Get("OutputTree"));
  }


  std::vector<TFile*> backgr_files;
  std::vector<string> backgr_labels;
  std::vector<int> backgr_colors;
  //backgr_files.push_back(new TFile("backgrounds/delphes.B-4p-0-1-v1510_14TEV.NoPileUp.root")); 
  //backgr_labels.push_back("W/Z");
  //backgr_colors.push_back(kBlue-8);
  backgr_files.push_back(new TFile("backgrounds/delphes.Bjj-4p.14TEV.NoPileUp.root")); 
  backgr_labels.push_back("VBF W/Z");
  backgr_colors.push_back(kBlue-10);
  backgr_files.push_back(new TFile("backgrounds/delphes.tj-4p.14TEV.NoPileUp.root")); 
  backgr_labels.push_back("Single-top");
  backgr_colors.push_back(kGreen+2);
  backgr_files.push_back(new TFile("backgrounds/delphes.tt-4p.14TEV.NoPileUp.root")); 
  backgr_labels.push_back("t#bar{t}");
  backgr_colors.push_back(kGreen-6);
  backgr_files.push_back(new TFile("backgrounds/delphes.Bj-4p.14TEV.NoPileUp.root")); 
  backgr_labels.push_back("W/Z+jets");
  backgr_colors.push_back(kBlue-6);

  string printvariable=variable;
  if(variable.find("/") != string::npos)
    printvariable = printvariable.replace(variable.find("/"),1,"_");

  std::vector<TTree*> backgr_trees;
  for(unsigned int i=0; i<backgr_files.size(); i++){
    backgr_trees.push_back((TTree*)backgr_files[i]->Get("OutputTree"));
  }

  string cut="nJets60_eta3p5>3&&HTleptons==0";
  if(extracut!="" && cut!="")
    cut+="&&"+extracut;
  else if(extracut!="")
    cut=extracut;


  std::vector<TH1F*> signal_hists;
  for(unsigned int i=0; i<signal_files.size(); i++){
    if(!signal_trees[i]) continue;
    signal_hists.push_back(new TH1F(Form("h_%s_sig_%d",printvariable.c_str(),i),
				    Form("%s;%s;%s",signal_labels[i].c_str(),varlabel.c_str(),yaxislabel.c_str()),
				    50,min,max));
    signal_hists[i]->SetLineColor(signal_colors[i]);
    signal_hists[i]->SetFillStyle(0);
    signal_hists[i]->SetLineWidth(3);
    signal_hists[i]->SetMarkerStyle(1);
    signal_hists[i]->SetMarkerColor(signal_colors[i]);
    signal_trees[i]->Draw(Form("%f*%s>>h_%s_sig_%d",xscale,variable.c_str(),printvariable.c_str(),i),
			  Form("weight*(%s)",cut.c_str()),
			  "goff");
    if(scaleTo3ab) signal_hists[i]->Scale(3000.);
  }


  std::vector<TH1F*> backgr_hists;
  THStack* backgr_stack = new THStack("backgr_stack","stack");
  for(unsigned int i=0; i<backgr_files.size(); i++){
    if(!backgr_trees[i]) continue;
    backgr_hists.push_back(new TH1F(Form("h_%s_bak_%d",printvariable.c_str(),i),
				    Form("%s;%s;%s",backgr_labels[i].c_str(),varlabel.c_str(),yaxislabel.c_str()),
				    50,min,max));
    backgr_hists[i]->SetFillColor(backgr_colors[i]);
    backgr_hists[i]->SetLineColor(kBlack);
    backgr_hists[i]->SetFillStyle(1001);
    backgr_hists[i]->SetLineWidth(1);
    backgr_hists[i]->SetMarkerStyle(1);
    backgr_hists[i]->SetMarkerColor(backgr_colors[i]);
    backgr_trees[i]->Draw(Form("%f*%s>>h_%s_bak_%d",xscale,variable.c_str(),printvariable.c_str(),i),
			  Form("weight*(%s)",cut.c_str()),
			  "goff");
    if(scaleTo3ab) backgr_hists[i]->Scale(3000.);

    backgr_stack->Add(backgr_hists[i]);
  }

  

  TCanvas* c = new TCanvas(Form("c_%s",printvariable.c_str()),
			   Form("c_%s",printvariable.c_str()),
			   800,600);
  c->SetLogy(1);
  //c->SetGridy(1);
  gStyle->SetGridColor(kGray);
  gStyle->SetGridWidth(1);

  backgr_stack->Draw();
  if(!scaleTo3ab){
    backgr_stack->SetMinimum(0.00001);
    backgr_stack->SetMaximum(1000000000000.);
  }
  else{
    backgr_stack->SetMinimum(.5);
    backgr_stack->SetMaximum(1000000000000.);
  }
  
  if(backgr_stack->GetMaximum() < 10000){
    backgr_stack->SetMinimum(0.01);
    backgr_stack->SetMaximum(1000000000);
  }

  backgr_stack->GetHistogram()->GetYaxis()->SetTitle(backgr_hists[0]->GetYaxis()->GetTitle());
  backgr_stack->GetHistogram()->GetXaxis()->SetTitle(backgr_hists[0]->GetXaxis()->GetTitle());
  backgr_stack->GetHistogram()->GetXaxis()->SetTitleSize(0.06);
  backgr_stack->GetHistogram()->GetYaxis()->SetTitleSize(0.06);
  backgr_stack->GetHistogram()->GetXaxis()->SetTitleOffset(1.0);
  backgr_stack->GetHistogram()->GetYaxis()->SetTitleOffset(1.0);

  //backgr_hists[2]->Draw();
  //backgr_hists[2]->GetYaxis()->SetRangeUser(0.0000001,100000000000.);
  for(unsigned int i=0; i<signal_files.size(); i++){
    if(signal_hists[i]) signal_hists[i]->Draw("sames");
  }

  TLegend* l = new TLegend(0.5,0.58,0.85,0.93);
  l->SetFillColor(0);
  l->SetShadowColor(0);
  for(unsigned int i=backgr_files.size(); i>0; i--){
    l->AddEntry(backgr_hists[i-1],backgr_hists[i-1]->GetTitle(),"F");
  }  
  for(unsigned int i=0; i<signal_files.size(); i++){
    l->AddEntry(signal_hists[i],signal_hists[i]->GetTitle(),"L");
  }
  l->Draw();
  if(false){
    TLegend* ll = c->BuildLegend(0.5,0.60,0.85,0.91);
    //ll->SetNColumns(2);
    ll->SetFillColor(0);
    ll->SetShadowColor(0);
  }
  
  c->Print(Form("%s.pdf",printvariable.c_str()));
  return c;
}


void wrap(){
  float textsize=0.04;
  TCanvas* c;
  /*
  c = make_plot("METsignif","MET>100&&leadJetPt/HTjets_30_3p5<0.4","E_{T}^{miss}/#sqrt{H_{T}^{jets}} [GeV^{1/2}]",
		(scaleTo3ab ? "Events/(0.4 GeV^{1/2}) in 3 ab^{-1}":"fb/(0.4 GeV^{1/2})"),
		 0,50);
  myText(0.2, 0.87, kBlack, "n_{jets}^{p_{T} > 60 GeV} #geq 4", -1, textsize);
  myText(0.2, 0.80, kBlack, "p_{T}^{lead jet}/H_{T}^{jets} < 0.4", -1, textsize);
  myText(0.2, 0.73, kBlack, "E_{T}^{miss} > 100 GeV", -1, textsize);
  myText(0.2, 0.66, kBlack, "0 Extra Int/Crossing", -1, textsize);
  c->Print("METsignif.pdf");

  c = make_plot("m_eff","METsignif>15&&leadJetPt/HTjets_30_3p5<0.4","m_{eff} [TeV]",
		(scaleTo3ab ? "Events/(0.2 TeV) in 3 ab^{-1}":"fb/(0.2 TeV)"),
		0,10,1./1000.);
  myText(0.2, 0.87, kBlack, "n_{jets}^{p_{T} > 60 GeV} #geq 4", -1, textsize);
  myText(0.2, 0.80, kBlack, "p_{T}^{lead jet}/H_{T}^{jets} < 0.4", -1, textsize);
  myText(0.2, 0.73, kBlack, "E_{T}^{miss}/#sqrt{H_{T}^{jets}} > 15 GeV^{1/2}", -1, textsize);
  myText(0.2, 0.66, kBlack, "0 Extra Int/Crossing", -1, textsize);
  c->Print("m_eff.pdf");

  
  c = make_plot("leadJetPt","METsignif>15&&m_eff>3000","p_{T}^{lead jet} [GeV]",
		(scaleTo3ab ? "Events/(50 GeV) in 3 ab^{-1}":"fb/(50 GeV)"),
		0,2500);
  myText(0.2, 0.87, kBlack, "n_{jets}^{p_{T} > 60 GeV} #geq 4", -1, textsize);
  myText(0.2, 0.80, kBlack, "E_{T}^{miss}/#sqrt{H_{T}^{jets}} > 15 GeV^{1/2}", -1, textsize);
  myText(0.2, 0.73, kBlack, "m_{eff} > 3000 GeV", -1, textsize);
  myText(0.2, 0.66, kBlack, "0 Extra Int/Crossing", -1, textsize);
  c->Print("leadJetPt.pdf");
  

  */

  c = make_plot("leadJetPt/HTjets_30_3p5","METsignif>15&&m_eff>3000","p_{T}^{lead jet}/H_{T}^{jets}",
		(scaleTo3ab ? "Events/0.02 in 3 ab^{-1}":"fb/0.02"),
		0,1);
  myText(0.2, 0.87, kBlack, "n_{jets}^{p_{T} > 60 GeV} #geq 4", -1, textsize);
  myText(0.2, 0.80, kBlack, "E_{T}^{miss}/#sqrt{H_{T}^{jets}} > 15 GeV^{1/2}", -1, textsize);
  myText(0.2, 0.73, kBlack, "m_{eff} > 3000 GeV", -1, textsize);
  myText(0.2, 0.66, kBlack, "0 Extra Int/Crossing", -1, textsize);
  c->Print("leadJetPt_HT.pdf");
  
}


TH1F* make_hist(TFile* f,
		std::string samplelabel,
		std::string variable,
		std::string varlabel,
		std::string cut,
		std::string cutlabel,
		int nbins, float xlow, float xhigh){

  TH1F* h = new TH1F(Form("h_%s_%s_%s",samplelabel.c_str(),cutlabel.c_str(),varlabel.c_str()),
		     Form("h_%s_%s_%s",samplelabel.c_str(),cutlabel.c_str(),varlabel.c_str()),
		     nbins, xlow, xhigh);

  TTree* t = (TTree*)f->Get("OutputTree");
  string drawstr=Form("(%s)>>%s",variable.c_str(),h->GetName());
  cout << samplelabel << "\t" << varlabel << "\t" << cutlabel << endl;
  // cout << drawstr << endl << cut << endl;
  t->Draw(drawstr.c_str(),cut.c_str(), "goff");
  delete t;

  return h;
}
		

TH2F* make_hist(TFile* f,
		std::string samplelabel,
		std::string variable,
		std::string varlabel,
		std::string cut,
		std::string cutlabel,
		int nbinsx, float xlow, float xhigh,
		int nbinsy, float ylow, float yhigh){

  TH2F* h = new TH2F(Form("h_%s_%s_%s",samplelabel.c_str(),cutlabel.c_str(),varlabel.c_str()),
		     Form("h_%s_%s_%s",samplelabel.c_str(),cutlabel.c_str(),varlabel.c_str()),
		     nbinsx, xlow, xhigh,
		     nbinsy, ylow, yhigh);

  TTree* t = (TTree*)f->Get("OutputTree");
  string drawstr=Form("%s>>%s",(variable.replace(variable.find("___"),3,":")).c_str(),h->GetName());
  cout << samplelabel << "\t" << varlabel << "\t" << cutlabel << endl;
  //cout << drawstr << endl << cut << endl;
  t->Draw(drawstr.c_str(),cut.c_str(), "goff");
  delete t;

  return h;
}
		

void make_hists(bool test=false){
  TFile* hists = new TFile("various_hists.root","UPDATE");
  
  std::vector<string> samples; std::vector<string> labels;
  std::map<string,float> kfacts;

  // samples.push_back("backgrounds/delphes.Bj-4p.14TEV.NoPileUp.root");  labels.push_back("Bj_14_000");
  if(!test){
    // // 14 TeV backgrounds
    // samples.push_back("backgrounds/delphes.B-4p-0-1-v1510_14TEV.NoPileUp.root");   labels.push_back("B_14_000");
    // samples.push_back("backgrounds/delphes.Bjj-4p.14TEV.NoPileUp.root"); labels.push_back("Bjj_14_000");
    // samples.push_back("backgrounds/delphes.tt-4p.14TEV.NoPileUp.root");  labels.push_back("tt_14_000");
    // samples.push_back("backgrounds/delphes.tj-4p.14TEV.NoPileUp.root");  labels.push_back("tj_14_000");

    // // 14 TeV signals
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.NoPileUp.GO00500.qq.N00001_14TEV.root");  labels.push_back("GOGO_14_000_0500_0001"); kfacts["GOGO_14_000_0500_0001"] = 1.61;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.NoPileUp.GO01255.qq.N00001_14TEV.root");  labels.push_back("GOGO_14_000_1255_0001"); kfacts["GOGO_14_000_1255_0001"] = 1.78;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.NoPileUp.GO01255.qq.N00251_14TEV.root");  labels.push_back("GOGO_14_000_1255_0251"); kfacts["GOGO_14_000_1255_0251"] = 1.78;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.NoPileUp.GO01255.qq.N00502_14TEV.root");  labels.push_back("GOGO_14_000_1255_0502"); kfacts["GOGO_14_000_1255_0502"] = 1.78;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.NoPileUp.GO01255.qq.N00753_14TEV.root");  labels.push_back("GOGO_14_000_1255_0753"); kfacts["GOGO_14_000_1255_0753"] = 1.78;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.NoPileUp.GO01255.qq.N00879_14TEV.root");  labels.push_back("GOGO_14_000_1255_0879"); kfacts["GOGO_14_000_1255_0879"] = 1.78;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.NoPileUp.GO01255.qq.N01004_14TEV.root");  labels.push_back("GOGO_14_000_1255_1004"); kfacts["GOGO_14_000_1255_1004"] = 1.78;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.NoPileUp.GO01255.qq.N01130_14TEV.root");  labels.push_back("GOGO_14_000_1255_1130"); kfacts["GOGO_14_000_1255_1130"] = 1.78;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.NoPileUp.GO01255.qq.N01155_14TEV.root");  labels.push_back("GOGO_14_000_1255_1155"); kfacts["GOGO_14_000_1255_1155"] = 1.78;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.NoPileUp.GO01255.qq.N01205_14TEV.root");  labels.push_back("GOGO_14_000_1255_1205"); kfacts["GOGO_14_000_1255_1205"] = 1.78;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.NoPileUp.GO01255.qq.N01240_14TEV.root");  labels.push_back("GOGO_14_000_1255_1240"); kfacts["GOGO_14_000_1255_1240"] = 1.78;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.NoPileUp.GO01255.qq.N01250_14TEV.root");  labels.push_back("GOGO_14_000_1255_1250"); kfacts["GOGO_14_000_1255_1250"] = 1.78;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.NoPileUp.GO02489.qq.N00001_14TEV.root");  labels.push_back("GOGO_14_000_2489_0001"); kfacts["GOGO_14_000_2489_0001"] = 2.17;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.NoPileUp.GO03489.qq.N00001_14TEV.root");  labels.push_back("GOGO_14_000_3489_0001"); kfacts["GOGO_14_000_3489_0001"] = 1.45;
    // samples.push_back("GOSQ_masslessN0_14TeV/delphes.GOSQ_masslessN0_14TeV.NoPileUp.GOSQ02400.02800_14TEV.root");                      labels.push_back("GOSQ_14_000_2400_2800"); kfacts["GOSQ_14_000_2400_2800"] = 1.0;
    // samples.push_back("GOSQ_masslessN0_14TeV/delphes.GOSQ_masslessN0_14TeV.NoPileUp.GOSQ03200.03200_14TEV.root");                      labels.push_back("GOSQ_14_000_3200_3200"); kfacts["GOSQ_14_000_3200_3200"] = 1.0;
    // samples.push_back("SQ_decoupledGO_14TeV/delphes.SQ_decoupledGO_14TeV.NoPileUp.SQ00500.q.N00001_14TEV.root");                       labels.push_back("SQSQ_14_000_0500_0001"); kfacts["SQSQ_14_000_0500_0001"] = 1.59;
    // samples.push_back("SQ_decoupledGO_14TeV/delphes.SQ_decoupledGO_14TeV.NoPileUp.SQ01255.q.N00001_14TEV.root");                       labels.push_back("SQSQ_14_000_1255_0001"); kfacts["SQSQ_14_000_1255_0001"] = 1.86;
    // samples.push_back("SQ_decoupledGO_14TeV/delphes.SQ_decoupledGO_14TeV.NoPileUp.SQ02489.q.N00001_14TEV.root");                       labels.push_back("SQSQ_14_000_2489_0001"); kfacts["SQSQ_14_000_2489_0001"] = 2.74;


    // // 14 Tev backgrounds, pileup
    // samples.push_back("backgrounds/delphes.Bj-4p.14TEV.140PileUp.root");  labels.push_back("Bj_14_140");
    // samples.push_back("backgrounds/delphes.tt-4p.14TEV.140PileUp.root");  labels.push_back("tt_14_140");

    // // 14 TeV signals, pileup
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.140PileUp.GO00500.qq.N00001_14TEV.root");  labels.push_back("GOGO_14_140_0500_0001"); kfacts["GOGO_14_140_0500_0001"] = 1.61;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.140PileUp.GO01255.qq.N00001_14TEV.root");  labels.push_back("GOGO_14_140_1255_0001"); kfacts["GOGO_14_140_1255_0001"] = 1.78;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.140PileUp.GO02489.qq.N00001_14TEV.root");  labels.push_back("GOGO_14_140_2489_0001"); kfacts["GOGO_14_140_2489_0001"] = 2.17;
    // samples.push_back("GO_decoupledSQ_14TeV_LHE_QQALL/delphes.GO_decoupledSQ_14TeV_LHE_QQALL.140PileUp.GO03489.qq.N00001_14TEV.root");  labels.push_back("GOGO_14_140_3489_0001"); kfacts["GOGO_14_140_3489_0001"] = 1.45;

    // // 33 TeV backgrounds
    // samples.push_back("backgrounds/delphes.Bj-4p.33TEV.NoPileUp.root");  labels.push_back("Bj_33_000");
    // samples.push_back("backgrounds/delphes.tt-4p.33TEV.NoPileUp.root");  labels.push_back("tt_33_000");

    // // 33 TeV signals
    // samples.push_back("GO_decoupledSQ_33TeV_LHE_QQALL/delphes.GO_decoupledSQ_33TeV_LHE_QQALL.NoPileUp.GO00500.qq.N00001_33TEV.root");  labels.push_back("GOGO_33_000_0500_0001"); kfacts["GOGO_33_000_0500_0001"] = 1.60;
    // samples.push_back("GO_decoupledSQ_33TeV_LHE_QQALL/delphes.GO_decoupledSQ_33TeV_LHE_QQALL.NoPileUp.GO01255.qq.N00001_33TEV.root");  labels.push_back("GOGO_33_000_1255_0001"); kfacts["GOGO_33_000_1255_0001"] = 1.01;
    // samples.push_back("GO_decoupledSQ_33TeV_LHE_QQALL/delphes.GO_decoupledSQ_33TeV_LHE_QQALL.NoPileUp.GO02504.qq.N00001_33TEV.root");  labels.push_back("GOGO_33_000_2504_0001"); kfacts["GOGO_33_000_2504_0001"] = 1.00;
    // samples.push_back("GO_decoupledSQ_33TeV_LHE_QQALL/delphes.GO_decoupledSQ_33TeV_LHE_QQALL.NoPileUp.GO03152.qq.N00001_33TEV.root");  labels.push_back("GOGO_33_000_3152_0001"); kfacts["GOGO_33_000_3152_0001"] = 1.72;
    // samples.push_back("GO_decoupledSQ_33TeV_LHE_QQALL/delphes.GO_decoupledSQ_33TeV_LHE_QQALL.NoPileUp.GO04968.qq.N00001_33TEV.root");  labels.push_back("GOGO_33_000_4968_0001"); kfacts["GOGO_33_000_4968_0001"] = 1.89;
    
    // samples.push_back("GOSQ_masslessN0_33TeV/delphes.GOSQ_masslessN0_33TeV.NoPileUp.GOSQ01995.01995_33TEV.root");  labels.push_back("GOSQ_33_000_1995_1995"); kfacts["GOGO_33_000_1995_1995"] = 1.0;
    // samples.push_back("GOSQ_masslessN0_33TeV/delphes.GOSQ_masslessN0_33TeV.NoPileUp.GOSQ03162.03162_33TEV.root");  labels.push_back("GOSQ_33_000_3162_3162"); kfacts["GOGO_33_000_3162_3162"] = 1.0;
    // samples.push_back("GOSQ_masslessN0_33TeV/delphes.GOSQ_masslessN0_33TeV.NoPileUp.GOSQ04981.04981_33TEV.root");  labels.push_back("GOSQ_33_000_4981_4981"); kfacts["GOGO_33_000_4981_4981"] = 1.0;
    // samples.push_back("GOSQ_masslessN0_33TeV/delphes.GOSQ_masslessN0_33TeV.NoPileUp.GOSQ05981.05981_33TEV.root");  labels.push_back("GOSQ_33_000_5981_5981"); kfacts["GOGO_33_000_5981_5981"] = 1.0;
    // samples.push_back("GOSQ_masslessN0_33TeV/delphes.GOSQ_masslessN0_33TeV.NoPileUp.GOSQ06981.06981_33TEV.root");  labels.push_back("GOSQ_33_000_6981_6981"); kfacts["GOGO_33_000_6981_6981"] = 1.0;
    // samples.push_back("GOSQ_masslessN0_33TeV/delphes.GOSQ_masslessN0_33TeV.NoPileUp.GOSQ07981.09981_33TEV.root");  labels.push_back("GOSQ_33_000_7981_9981"); kfacts["GOGO_33_000_7981_9981"] = 1.0;
    // samples.push_back("SQ_decoupledGO_33TeV/delphes.SQ_decoupledGO_33TeV.NoPileUp.SQ00500.q.N00001_33TEV.root");  labels.push_back("SQSQ_33_000_0500_0001"); kfacts["GOGO_33_000_0500_0001"] = 1.41;
    // samples.push_back("SQ_decoupledGO_33TeV/delphes.SQ_decoupledGO_33TeV.NoPileUp.SQ00629.q.N00001_33TEV.root");  labels.push_back("SQSQ_33_000_0629_0001"); kfacts["GOGO_33_000_0629_0001"] = 1.45;
    // samples.push_back("SQ_decoupledGO_33TeV/delphes.SQ_decoupledGO_33TeV.NoPileUp.SQ01255.q.N00001_33TEV.root");  labels.push_back("SQSQ_33_000_1255_0001"); kfacts["GOGO_33_000_1255_0001"] = 1.59;
    // samples.push_back("SQ_decoupledGO_33TeV/delphes.SQ_decoupledGO_33TeV.NoPileUp.SQ01989.q.N00001_33TEV.root");  labels.push_back("SQSQ_33_000_1989_0001"); kfacts["GOGO_33_000_1989_0001"] = 1.68;
    // samples.push_back("SQ_decoupledGO_33TeV/delphes.SQ_decoupledGO_33TeV.NoPileUp.SQ03152.q.N00001_33TEV.root");  labels.push_back("SQSQ_33_000_3152_0001"); kfacts["GOGO_33_000_3152_0001"] = 1.83;
    
    // // 100 TeV backgrounds
    // samples.push_back("backgrounds/delphes.Bj-4p.100TEV.NoPileUp.root");  labels.push_back("Bj_100_000");
    // samples.push_back("backgrounds/delphes.tt-4p.100TEV.NoPileUp.root");  labels.push_back("tt_100_000");
    // samples.push_back("backgrounds/delphes.tj-4p.100TEV.NoPileUp.root");  labels.push_back("tj_100_000");

    // // 100 TeV signals
    // samples.push_back("GOSQ_masslessN0_100TeV/delphes.GOSQ_masslessN0_100TeV.NoPileUp.GOSQ03162.03162_100TEV.root");  labels.push_back("GOSQ_100_000_03162_03162"); kfacts["GOSQ_100_000_03162_03162"] = 1.0;
    // samples.push_back("GOSQ_masslessN0_100TeV/delphes.GOSQ_masslessN0_100TeV.NoPileUp.GOSQ07944.07944_100TEV.root");  labels.push_back("GOSQ_100_000_07944_07944"); kfacts["GOSQ_100_000_07944_07944"] = 1.0;
    // samples.push_back("GOSQ_masslessN0_100TeV/delphes.GOSQ_masslessN0_100TeV.NoPileUp.GOSQ09944.09944_100TEV.root");  labels.push_back("GOSQ_100_000_09944_09944"); kfacts["GOSQ_100_000_09944_09944"] = 1.0;
    // samples.push_back("GOSQ_masslessN0_100TeV/delphes.GOSQ_masslessN0_100TeV.NoPileUp.GOSQ15944.15944_100TEV.root");  labels.push_back("GOSQ_100_000_15944_15944"); kfacts["GOSQ_100_000_15944_15944"] = 1.0;
    // samples.push_back("GOSQ_masslessN0_100TeV/delphes.GOSQ_masslessN0_100TeV.NoPileUp.GOSQ23944.23944_100TEV.root");  labels.push_back("GOSQ_100_000_23944_23944"); kfacts["GOSQ_100_000_23944_23944"] = 1.0;

    // samples.push_back("SQ_decoupledGO_100TeV/delphes.SQ_decoupledGO_100TeV.NoPileUp.SQ01259.q.N00001_100TEV.root");  labels.push_back("SQSQ_100_000_1259_0001"); kfacts["SQSQ_100_000_1259_0001"] = 1.38;
    // samples.push_back("SQ_decoupledGO_100TeV/delphes.SQ_decoupledGO_100TeV.NoPileUp.SQ02512.q.N00001_100TEV.root");  labels.push_back("SQSQ_100_000_2512_0001"); kfacts["SQSQ_100_000_2512_0001"] = 1.46;
    // samples.push_back("SQ_decoupledGO_100TeV/delphes.SQ_decoupledGO_100TeV.NoPileUp.SQ03162.q.N00001_100TEV.root");  labels.push_back("SQSQ_100_000_3162_0001"); kfacts["SQSQ_100_000_3162_0001"] = 1.48;
    // samples.push_back("SQ_decoupledGO_100TeV/delphes.SQ_decoupledGO_100TeV.NoPileUp.SQ05012.q.N00001_100TEV.root");  labels.push_back("SQSQ_100_000_5012_0001"); kfacts["SQSQ_100_000_5012_0001"] = 1.53;
    // samples.push_back("SQ_decoupledGO_100TeV/delphes.SQ_decoupledGO_100TeV.NoPileUp.SQ07944.q.N00001_100TEV.root");  labels.push_back("SQSQ_100_000_7944_0001"); kfacts["SQSQ_100_000_7944_0001"] = 1.63;
    // samples.push_back("SQ_decoupledGO_100TeV/delphes.SQ_decoupledGO_100TeV.NoPileUp.SQ09944.q.N00001_100TEV.root");  labels.push_back("SQSQ_100_000_9944_0001"); kfacts["SQSQ_100_000_9944_0001"] = 1.75;

    // samples.push_back("GO_decoupledSQ_100TeV/delphes.GO_decoupledSQ_100TeV.NoPileUp.GO01585.qq.N00001_100TEV.root");  labels.push_back("GOGO_100_000_1585_0001"); kfacts["GOGO_100_000_1585_0001"] = 1.39;
    // samples.push_back("GO_decoupledSQ_100TeV/delphes.GO_decoupledSQ_100TeV.NoPileUp.GO03981.qq.N00001_100TEV.root");  labels.push_back("GOGO_100_000_3981_0001"); kfacts["GOGO_100_000_3981_0001"] = 1.47;
    // samples.push_back("GO_decoupledSQ_100TeV/delphes.GO_decoupledSQ_100TeV.NoPileUp.GO05012.qq.N00001_100TEV.root");  labels.push_back("GOGO_100_000_5012_0001"); kfacts["GOGO_100_000_5012_0001"] = 1.49;
    // samples.push_back("GO_decoupledSQ_100TeV/delphes.GO_decoupledSQ_100TeV.NoPileUp.GO07944.qq.N00001_100TEV.root");  labels.push_back("GOGO_100_000_7944_0001"); kfacts["GOGO_100_000_7944_0001"] = 1.52;
    // samples.push_back("GO_decoupledSQ_100TeV/delphes.GO_decoupledSQ_100TeV.NoPileUp.GO09944.qq.N00001_100TEV.root");  labels.push_back("GOGO_100_000_9944_0001"); kfacts["GOGO_100_000_9944_0001"] = 1.52;
    // samples.push_back("GO_decoupledSQ_100TeV/delphes.GO_decoupledSQ_100TeV.NoPileUp.GO13944.qq.N00001_100TEV.root");  labels.push_back("GOGO_100_000_13944_0001"); kfacts["GOGO_100_000_13944_0001"] = 1.57;
  }

  std::vector<string> vars; std::vector<string> varlabels;
  std::vector<int> nbins,nbinsx,nbinsy; std::vector<float> low,high,lowx,lowy,highx,highy;
  vars.push_back("MET");                       varlabels.push_back("MET");                nbins.push_back(5000);    low.push_back(0);    high.push_back(50000);
  if(!test){
    vars.push_back("METsignif");               varlabels.push_back("METsignif");          nbins.push_back(2500);     low.push_back(0);    high.push_back(500);
    vars.push_back("leadJetPt");               varlabels.push_back("leadJetPt");          nbins.push_back(5000);    low.push_back(0);    high.push_back(50000);
    vars.push_back("leadJetPt/HTjets_30_3p5"); varlabels.push_back("leadJetPt_HTjets");   nbins.push_back(100);     low.push_back(0);    high.push_back(1);
    vars.push_back("m_eff");                   varlabels.push_back("meff");               nbins.push_back(10000);    low.push_back(0);    high.push_back(100000);
    vars.push_back("HTjets_30_3p5");           varlabels.push_back("HTjets");             nbins.push_back(10000);    low.push_back(0);    high.push_back(100000);
    vars.push_back("nJets60_eta3p5");          varlabels.push_back("nJets60_eta3p5");     nbins.push_back(200);      low.push_back(0);    high.push_back(200);
  }
  // only 2D plots below here.....
  nbinsx.resize(nbins.size()); lowx.resize(low.size()); highx.resize(high.size());
  nbinsy.resize(nbins.size()); lowy.resize(low.size()); highy.resize(high.size());

  vars.push_back("MET___HTjets_30_3p5");     varlabels.push_back("MET___HTjets_30_3p5");
  nbinsx.push_back(1000);    lowx.push_back(0);   highx.push_back(100000);
  nbinsy.push_back(1000);    lowy.push_back(0);   highy.push_back(100000);


  std::vector<string> cuts; std::vector<string> cutlabels;
  cuts.push_back("weight*(HTleptons==0 && nJets60_eta3p5>3 && MET>100)");                                                     cutlabels.push_back("baseline");
  if(!test){
    cuts.push_back("weight*(HTleptons==0 && nJets60_eta3p5>3 && MET>100 && METsignif>15)");                                      cutlabels.push_back("METsignif15");
    cuts.push_back("weight*(HTleptons==0 && nJets60_eta3p5>3 && MET>100 && (leadJetPt/HTjets_30_3p5) < 0.40)");                  cutlabels.push_back("LeadJetLtp4");
    cuts.push_back("weight*(HTleptons==0 && nJets60_eta3p5>3 && MET>100 && METsignif>15 && (leadJetPt/HTjets_30_3p5) < 0.40)");  cutlabels.push_back("METsignif15_LeadJetLtp4");
    cuts.push_back("weight*(HTleptons==0 && nJets60_eta3p5>3 && MET>100 && METsignif>15 && m_eff > 3000)");                      cutlabels.push_back("METsignif15_meff3000");
  }


  for(unsigned int smp_i = 0; smp_i < samples.size(); smp_i++){
    TFile f(samples[smp_i].c_str(),"RO");
    for(unsigned int var_i = 0; var_i < vars.size(); var_i++){
      for(unsigned int cut_i = 0; cut_i < cuts.size(); cut_i++){

	if(vars[var_i].find("___") == string::npos){
	  TH1F* h = make_hist(&f,             labels[smp_i],
			      vars[var_i],    varlabels[var_i],
			      cuts[cut_i],    cutlabels[cut_i],
			      nbins[var_i], low[var_i], high[var_i]);
	  if(kfacts.find(labels[smp_i])!=kfacts.end())
	    h->Scale(kfacts[labels[smp_i]]);
	  hists->cd();
	  h->Write();
	}
	else{
	  TH2F* h = make_hist(&f,             labels[smp_i],
			      vars[var_i],    varlabels[var_i],
			      cuts[cut_i],    cutlabels[cut_i],
			      nbinsx[var_i], lowx[var_i], highx[var_i],
			      nbinsy[var_i], lowy[var_i], highy[var_i]);	
	  if(kfacts.find(labels[smp_i])!=kfacts.end())
	    h->Scale(kfacts[labels[smp_i]]);
	  hists->cd();
	  h->Write();
	}
      }
    }
  }

  hists->Close();


}

std::vector<TH1F*> get_BG_hists(int energy=14,
				int lumi=0,
				string variable="meff",
				string cut="all",
				int rebin=1.,
				float scale=1.,
				bool validationplots=false){

  std::vector<TH1F*> hists;

  TFile* f = new TFile("various_hists.root","RO");

  TH1F* h_bjj = (TH1F*)f->Get(Form("h_Bjj_%d_%s_%s_%s",energy,(lumi == 0 ? "000":"140"),
				   cut.c_str(),variable.c_str()));
  if(h_bjj){
    h_bjj->SetFillColor(kBlue-10);
    h_bjj->SetTitle("VBF W/Z");
    if(!validationplots) hists.push_back(h_bjj);
  }
  TH1F* h_tj = (TH1F*)f->Get(Form("h_tj_%d_%s_%s_%s",energy,(lumi == 0 ? "000":"140"),
				  cut.c_str(),variable.c_str()));
  if(h_tj){
    h_tj->SetFillColor(kGreen+2);
    h_tj->SetTitle("Single-t");
    if(!validationplots) hists.push_back(h_tj);
  }

  TH1F* h_tt = (TH1F*)f->Get(Form("h_tt_%d_%s_%s_%s",energy,(lumi == 0 ? "000":"140"),
				  cut.c_str(),variable.c_str()));
  if(h_tt){
    if(!validationplots)
      h_tt->SetFillColor(kGreen-6);
    else
      h_tt->SetLineColor(kOrange-3);
    h_tt->SetTitle("t#bar{t}");
    hists.push_back(h_tt);
  }
  TH1F* h_bj = (TH1F*)f->Get(Form("h_Bj_%d_%s_%s_%s",energy,(lumi == 0 ? "000":"140"),
				  cut.c_str(),variable.c_str()));
  if(h_bj){
    if(!validationplots)
      h_bj->SetFillColor(kBlue-6);
    else
      h_bj->SetLineColor(kMagenta-2);
    h_bj->SetTitle("W/Z+jets");
    hists.push_back(h_bj);
  }

  for(unsigned int i=0; i<hists.size(); i++){
    if(!validationplots){
      hists[i]->SetLineColor(kBlack);
      hists[i]->SetFillStyle(1001);
      hists[i]->SetLineWidth(1);
    }
    else{
      hists[i]->SetMarkerStyle(2);
    }
    hists[i]->SetMarkerStyle(1);
    hists[i]->SetMarkerColor(hists[i]->GetLineColor());
    hists[i]->Scale(scale);
    if(rebin>1) hists[i]->Rebin(rebin);
  }
  
  return hists;
}



THStack* get_BG_stack(int energy=14,
		      int lumi=0,
		      string variable="meff",
		      string cut="all",
		      int rebin=1.,
		      float scale=1.){

  std::vector<TH1F*> hists = get_BG_hists(energy,lumi,variable,cut,rebin,scale);

  THStack* hs = new THStack(Form("backgr_stack_%d_%d_%s_%s",
				 energy, lumi, variable.c_str(), cut.c_str()),
			    Form("backgrounds"));

  for(unsigned int i=0; i<hists.size(); i++){
    hs->Add(hists[i]);
  }
  
  return hs;
}

TH1F* get_BG_sum(int energy=14,
		 int lumi=0,
		 string variable="meff",
		 string cut="all",
		 int rebin=1.){

  TH1F* hs(0);

  std::vector<TH1F*> hists = get_BG_hists(energy,lumi,variable,cut,rebin);

  for(unsigned int i=0; i<hists.size(); i++){
    if(!hs)
      hs = new TH1F(*hists[i]);
    else
      hs->Add(hists[i]);
  }
  
  return hs;
}

/*

  Want several plots here....

  // disciminating variables for each pileup/energy scenario
  // -- 14, none
  // -- 14, 140
  // -- 33, none

  // also want to compare signal and/or background for 000/140 pileup

  can have one function that takes in a vector of histograms that have
  appropriate titles and styles, does all the scaling and whatnot correctly,
  adds labels as needed.  

 */

TCanvas* fastCanv(THStack* backgrounds,
		  std::vector<TH1F*>& signals,
		  std::vector<string>& labels,
		  string canv_label,
		  bool logY,
		  bool addlabel=false,
		  float xmax=-1){

  TCanvas* c = new TCanvas(Form("c_%s",canv_label.c_str()),
			   Form("c_%s",canv_label.c_str()),
			   800,600);
  
  backgrounds->Draw();

  float max=(backgrounds ? backgrounds->GetMaximum():0);
  
  for(unsigned int i=0; i<signals.size(); i++){
    if(signals[i]->GetMaximum()>max) max=signals[i]->GetMaximum();
    signals[i]->Draw("sames");
  }

  if(logY){
    backgrounds->SetMinimum(.1);
    if(labels.size()>3)
      backgrounds->SetMaximum(10e4*max);
    else
      backgrounds->SetMaximum(10e2*max);
    c->SetLogy(1);
  }

  TLegend* l;
  if(addlabel) l = new TLegend(0.5,0.58,0.85,0.85);
  else         l = new TLegend(0.5,0.58,0.85,0.93);
  l->SetFillColor(0);
  l->SetShadowColor(0);

  TIter next(backgrounds->GetHists());
  TObject* object = 0;
  std::vector<TH1F*> backgrs;
  while ((object = next())){
    backgrs.push_back((TH1F*)object);
  } 

  for(int i=(int)backgrs.size()-1; i>=0; i--){
    l->AddEntry(backgrs[i],backgrs[i]->GetTitle(),"F");
  }

  for(unsigned int i=0; i<signals.size(); i++){
    l->AddEntry(signals[i],signals[i]->GetTitle(),"L");
  }
  l->Draw();

  for(unsigned int i=0; i<labels.size(); i++){
    myText(0.2,0.89-i*0.06, kBlack, labels[i].c_str(), -1, 0.04);
  }

  if(xmax>0)
    backgrounds->GetXaxis()->SetRangeUser(0,xmax);

  backgrounds->GetXaxis()->SetTitleSize(0.06);
  backgrounds->GetYaxis()->SetTitleSize(0.06);
  backgrounds->GetXaxis()->SetTitleOffset(1.0);
  backgrounds->GetYaxis()->SetTitleOffset(1.0);


  return c;
}

TCanvas* fastCanv2(std::vector<TH1F*>&  backgrs,
		   std::vector<TH1F*>& signals,
		   std::vector<string>& labels,
		   string canv_label,
		   bool logY,
		   bool addlabel=false,
		   float xmax=-1){

  TCanvas* c = new TCanvas(Form("c_%s",canv_label.c_str()),
			   Form("c_%s",canv_label.c_str()),
			   800,600);
  
  float max=0;
  
  TH1F* axishist(0);

  for(unsigned int i=0; i<backgrs.size(); i++){
    if(!backgrs[i]) continue;
    if(i==0){ 
      backgrs[i]->Draw();
      axishist=backgrs[i];
    }
    else backgrs[i]->Draw("sames");
    if(backgrs[i]->GetMaximum()>max) max=backgrs[i]->GetMaximum();
  }

  for(unsigned int i=0; i<signals.size(); i++){
    if(signals[i]->GetMaximum()>max) max=signals[i]->GetMaximum();
    signals[i]->Draw("sames");
  }

  if(logY){
    axishist->SetMinimum(.4);
    axishist->SetMaximum(1.5*max);
    c->SetLogy(1);
  }

  TLegend* l;
  if(addlabel) l = new TLegend(0.45,0.58,0.85,0.85);
  else         l = new TLegend(0.45,0.58,0.85,0.93);
  l->SetFillColor(0);
  l->SetShadowColor(0);

  for(int i=(int)backgrs.size()-1; i>=0; i--){
    l->AddEntry(backgrs[i],backgrs[i]->GetTitle(),"L");
  }

  for(unsigned int i=0; i<signals.size(); i++){
    l->AddEntry(signals[i],signals[i]->GetTitle(),"L");
  }
  l->Draw();

  for(unsigned int i=0; i<labels.size(); i++){
    myText(0.65,0.5-i*0.06, kBlack, labels[i].c_str(), -1, 0.04);
  }

  if(xmax>0)
    axishist->GetXaxis()->SetRangeUser(0,xmax);

  axishist->GetXaxis()->SetTitleSize(0.06);
  axishist->GetYaxis()->SetTitleSize(0.06);
  axishist->GetXaxis()->SetTitleOffset(1.0);
  axishist->GetYaxis()->SetTitleOffset(1.0);


  return c;
}

std::vector<TH1F*> get_signal_hists(TFile* f, bool sameGOmass, int energy, int lumi, string cut, string var, int rebin, float scale=1, bool validationplots=false,
				    bool doSQSQB=false, bool doGOSQ=false){
  std::vector<TH1F*> signal_hists;
  if(validationplots || doGOSQ){
    if(energy==14){
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOSQ_%d_%s_2400_2800_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOSQ_%d_%s_3200_3200_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
  
      signal_hists[0]->SetTitle("m_{#tilde{q}}=2.8 TeV, m_{#tilde{g}}=2.4 TeV");
      signal_hists[1]->SetTitle("m_{#tilde{q}}=3.2 TeV, m_{#tilde{g}}=3.2 TeV");
    }
    else if(energy==33){
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOSQ_%d_%s_1995_1995_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOSQ_%d_%s_3162_3162_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOSQ_%d_%s_4981_4981_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOSQ_%d_%s_6981_6981_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
  
      signal_hists[0]->SetTitle("m_{#tilde{q}}=m_{#tilde{g}}=2 TeV");
      signal_hists[1]->SetTitle("m_{#tilde{q}}=m_{#tilde{g}}=3 TeV");
      signal_hists[2]->SetTitle("m_{#tilde{q}}=m_{#tilde{g}}=5 TeV");
      signal_hists[3]->SetTitle("m_{#tilde{q}}=m_{#tilde{g}}=7 TeV");
    }
    else if(energy==100){
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOSQ_%d_%s_03162_03162_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOSQ_%d_%s_09944_09944_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOSQ_%d_%s_15944_15944_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOSQ_%d_%s_23944_23944_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
  
      signal_hists[0]->SetTitle("m_{#tilde{q}}=m_{#tilde{g}}=3.2 TeV");
      signal_hists[1]->SetTitle("m_{#tilde{q}}=m_{#tilde{g}}=10 TeV");
      signal_hists[2]->SetTitle("m_{#tilde{q}}=m_{#tilde{g}}=16 TeV");
      signal_hists[3]->SetTitle("m_{#tilde{q}}=m_{#tilde{g}}=24 TeV");
    }

    if(validationplots){
      signal_hists[0]->SetLineColor(kBlue);
      signal_hists[0]->SetLineStyle(kDashed);
      signal_hists[1]->SetLineColor(kRed);
      signal_hists[1]->SetLineStyle(kDashed);
    }
    else{
      signal_hists[0]->SetLineColor(kRed-4);
      signal_hists[1]->SetLineColor(kRed-2);
      if(signal_hists.size()>=3) signal_hists[2]->SetLineColor(kRed);
      if(signal_hists.size()>=4) signal_hists[3]->SetLineColor(kRed+2);
    }
  }
  else if(doSQSQB){
    if(energy==14){
      signal_hists.push_back((TH1F*)f->Get(Form("h_SQSQ_%d_%s_0500_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_SQSQ_%d_%s_1255_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_SQSQ_%d_%s_2489_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));

      signal_hists[0]->SetTitle("m_{#tilde{q}}=500 GeV");
      signal_hists[1]->SetTitle("m_{#tilde{q}}=1255 GeV");
      signal_hists[2]->SetTitle("m_{#tilde{q}}=2489 GeV");
    }
    else if(energy==33){
      signal_hists.push_back((TH1F*)f->Get(Form("h_SQSQ_%d_%s_0500_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_SQSQ_%d_%s_1255_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_SQSQ_%d_%s_1989_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));

      signal_hists[0]->SetTitle("m_{#tilde{q}}=500 GeV");
      signal_hists[1]->SetTitle("m_{#tilde{q}}=1255 GeV");
      signal_hists[2]->SetTitle("m_{#tilde{q}}=1989 GeV");
    }
    else if(energy==100){
      signal_hists.push_back((TH1F*)f->Get(Form("h_SQSQ_%d_%s_1259_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_SQSQ_%d_%s_2512_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_SQSQ_%d_%s_5012_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_SQSQ_%d_%s_9944_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));

      signal_hists[0]->SetTitle("m_{#tilde{q}}=1.3 TeV");
      signal_hists[1]->SetTitle("m_{#tilde{q}}=2.5 TeV");
      signal_hists[2]->SetTitle("m_{#tilde{q}}=5.0 TeV");
      signal_hists[3]->SetTitle("m_{#tilde{q}}=10 TeV");
    }
  
    signal_hists[0]->SetLineColor(kRed-4);
    signal_hists[1]->SetLineColor(kRed-2);
    signal_hists[2]->SetLineColor(kRed);
    if(signal_hists.size()>=4) signal_hists[3]->SetLineColor(kRed+2);
  }
  else if(!sameGOmass){
    if(energy==14){
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_0500_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_1255_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_2489_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_3489_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists[0]->SetTitle("m_{#tilde{g}}=500 GeV");
      signal_hists[1]->SetTitle("m_{#tilde{g}}=1255 GeV");
      signal_hists[2]->SetTitle("m_{#tilde{g}}=2489 GeV");
      signal_hists[3]->SetTitle("m_{#tilde{g}}=3489 GeV");
    }
    else if(energy==33){ 
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_0500_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_1255_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_2504_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_4968_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists[0]->SetTitle("m_{#tilde{g}}=500 GeV"); 
      signal_hists[1]->SetTitle("m_{#tilde{g}}=1255 GeV");
      signal_hists[2]->SetTitle("m_{#tilde{g}}=2504 GeV");
      signal_hists[3]->SetTitle("m_{#tilde{g}}=4968 GeV");
    }
    else if(energy==100){
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_1585_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_3981_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_7944_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_13944_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
      signal_hists[0]->SetTitle("m_{#tilde{g}}=1.6 TeV"); 
      signal_hists[1]->SetTitle("m_{#tilde{g}}=4 TeV");
      signal_hists[2]->SetTitle("m_{#tilde{g}}=8 TeV");
      signal_hists[3]->SetTitle("m_{#tilde{g}}=14 TeV");
    }
  
    signal_hists[0]->SetLineColor(kRed-4);
    signal_hists[1]->SetLineColor(kRed-2);
    signal_hists[2]->SetLineColor(kRed);
    signal_hists[3]->SetLineColor(kRed+2);
  }
  else{
    signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_1255_0001_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
    signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_1255_0502_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
    signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_1255_1004_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
    signal_hists.push_back((TH1F*)f->Get(Form("h_GOGO_%d_%s_1255_1250_%s_%s",energy,(lumi==0 ? "000":"140"),cut.c_str(),var.c_str())));
  
    signal_hists[0]->SetTitle("m_{#tilde{#chi}_{1}^{0}}=1 GeV");
    signal_hists[0]->SetLineColor(kRed-4);
    signal_hists[1]->SetTitle("m_{#tilde{#chi}_{1}^{0}}=502 GeV");
    signal_hists[1]->SetLineColor(kRed-2);
    signal_hists[2]->SetTitle("m_{#tilde{#chi}_{1}^{0}}=1004 GeV");
    signal_hists[2]->SetLineColor(kRed);
    signal_hists[3]->SetTitle("m_{#tilde{#chi}_{1}^{0}}=1250 GeV");
    signal_hists[3]->SetLineColor(kRed+2);
  }
  
  for(unsigned int i=0; i<signal_hists.size(); i++){
    signal_hists[i]->Rebin(rebin);
    signal_hists[i]->SetLineWidth(4);
    signal_hists[i]->Scale(scale);
  }

  return signal_hists;
}


TCanvas* fastCanvWrap1(int energy, int lumi, int rebin, string var, string cut, std::vector<string> morelabels, bool sameGOmass=false, string yaxislabel="Entries/Bin", string xaxislabel="", int ndiv=10, float scale=1., float xmax=-1, bool doSQSQB=false, bool doGOSQ=false){

  TFile* f = new TFile("various_hists.root","RO");

  THStack* bg = get_BG_stack(energy,lumi, var.c_str(), cut.c_str(), rebin, scale);

  std::vector<TH1F*> signal_hists = get_signal_hists(f,sameGOmass, energy, lumi, cut, var, rebin, scale, false, doSQSQB, doGOSQ);

  string procstring = "GOGO";
  std::vector<std::string> labels;
  if(!doSQSQB && !doGOSQ){
    labels.push_back("pp#rightarrow#tilde{g}#tilde{g}#rightarrowq#bar{q}#tilde{#chi}_{1}^{0}q#bar{q}#tilde{#chi}_{1}^{0}");
    //labels.push_back("pp#rightarrow#tilde{g}#tilde{g},#tilde{g}#tilde{q},#tilde{q}#tilde{q}");
  }
  else if(doSQSQB){
    labels.push_back("pp#rightarrow#tilde{q}#tilde{q}#rightarrowq#tilde{#chi}_{1}^{0}#bar{q}#tilde{#chi}_{1}^{0}");
    procstring="SQSQ";
  }
  else if(doGOSQ){
    labels.push_back("pp#rightarrow#tilde{g}#tilde{g},#tilde{g}#tilde{q},#tilde{q}#tilde{q}");
    procstring="GOSQ";
  }
  labels.push_back(Form("#sqrt{s}=%d TeV",energy));
  labels.push_back(lumi == 0 ? "0 extra int/Xing":"140 extra int/Xing");
  
  for(unsigned int i=0; i<morelabels.size(); i++)
    if(morelabels[i] != "") labels.push_back(morelabels[i]);

  bool printextralabel=false;
  TCanvas* c = fastCanv(bg,signal_hists,labels,Form("%s_%s_%d_%d",procstring.c_str(),var.c_str(),energy,lumi),true,printextralabel,xmax); 

  if(printextralabel){
    if(!sameGOmass)
      myText(0.55,0.87,kBlack,"m_{#tilde{#chi}_{1}^{0}}=1 GeV");
    else
      myText(0.55,0.87,kBlack,"m_{#tilde{g}}=1255 GeV");
  }

  bg->GetHistogram()->GetYaxis()->SetTitle(yaxislabel.c_str());
  bg->GetHistogram()->GetXaxis()->SetTitle(xaxislabel.c_str());
  bg->GetHistogram()->GetXaxis()->SetTitleSize(0.06);
  bg->GetHistogram()->GetYaxis()->SetTitleSize(0.06);
  bg->GetHistogram()->GetXaxis()->SetTitleOffset(1.0);
  bg->GetHistogram()->GetYaxis()->SetTitleOffset(1.0);

  if(ndiv>0)
    bg->GetHistogram()->GetXaxis()->SetNdivisions(ndiv);

  c->Update();
  
  return c;
}

TCanvas* fastCanvWrap2(int energy, int lumi, int rebin, string var, string cut, std::vector<string> morelabels, bool sameGOmass=false, string yaxislabel="Entries/Bin", string xaxislabel="", int ndiv=510, float scale=1., float xmax=-1){

  TFile* f = new TFile("various_hists.root","RO");

  std::vector<TH1F*> backgr_hists = get_BG_hists(energy,lumi, var.c_str(), cut.c_str(), rebin, scale, true);
  std::vector<TH1F*> signal_hists = get_signal_hists(f,sameGOmass, energy, lumi, cut, var, rebin, scale, true);

  std::vector<std::string> labels;
  //labels.push_back("pp#rightarrow#tilde{g}#tilde{g}#rightarrowq#bar{q}#tilde{#chi}_{1}^{0}q#bar{q}#tilde{#chi}_{1}^{0}");
  labels.push_back("pp#rightarrow#tilde{g}#tilde{g},#tilde{g}#tilde{q},#tilde{q}#tilde{q}");
  labels.push_back(Form("#sqrt{s}=%d TeV",energy));
  labels.push_back(lumi == 0 ? "0 extra int/Xing":"140 extra int/Xing");
  
  for(unsigned int i=0; i<morelabels.size(); i++)
    if(morelabels[i] != "") labels.push_back(morelabels[i]);

  bool printextralabel=false;
  TCanvas* c = fastCanv2(backgr_hists,signal_hists,labels,Form("%s_%d_%d",var.c_str(),energy,lumi),true,printextralabel,xmax); 

  if(printextralabel){
    if(!sameGOmass)
      myText(0.55,0.87,kBlack,"m_{#tilde{#chi}_{1}^{0}}=1 GeV");
    else
      myText(0.55,0.87,kBlack,"m_{#tilde{g}}=1255 GeV");
  }

  backgr_hists[0]->GetYaxis()->SetTitle(yaxislabel.c_str());
  backgr_hists[0]->GetXaxis()->SetTitle(xaxislabel.c_str());
  backgr_hists[0]->GetXaxis()->SetTitleSize(0.06);
  backgr_hists[0]->GetYaxis()->SetTitleSize(0.06);
  backgr_hists[0]->GetXaxis()->SetTitleOffset(1.0);
  backgr_hists[0]->GetYaxis()->SetTitleOffset(1.0);

  if(ndiv>0)
    backgr_hists[0]->GetXaxis()->SetNdivisions(ndiv);

  c->Update();
  
  return c;
}

void fastwrap(){
  std::vector<string> labels;

  TCanvas *cmet,*cht;
  labels.clear();
  // labels.push_back("E_{T}^{miss}/#sqrt{H_{T}} > 15 GeV^{1/2}");
  // labels.push_back("p_{T}^{leading jet} < 0.4#timesH_{T}");


  // ***********************************************************************************
  // validation plots
  if(false){
    TCanvas* cmetsig = fastCanvWrap2(14,0,6,"METsignif","baseline", labels, false, "Events/1.2 GeV^{1/2}", "E_{T}^{miss}/#sqrt{H_{T}} [GeV^{1/2}]", -1, 3000., 60);
    cmetsig->Print("c_validate_atlas_met.eps");
    cmetsig->Print("c_validate_atlas_met.pdf");

    // labels.push_back("E_{T}^{miss}/#sqrt{H_{T}} > 15 GeV^{1/2}");
    // labels.push_back("m_{eff} > 3 TeV");
    // fastCanvWrap1(14,0,2,"leadJetPt_HTjets","METsignif15_meff3000", labels, false, "Entries/0.2", "p_{T}^{leading jet}/H_{T}", 10);

    labels.clear();
    //labels.push_back("E_{T}^{miss}/#sqrt{H_{T}} > 15 GeV^{1/2}");
    //labels.push_back("p_{T}^{leading jet} < 0.4#timesH_{T}");
    TCanvas* cmeff = fastCanvWrap2(14,0,20.,"meff","METsignif15", labels, false, "Events/200 GeV", "m_{eff} [GeV]", 505, 3000.);
    cmeff->Print("c_validate_atlas_meff.eps");
    cmeff->Print("c_validate_atlas_meff.pdf");
    
  }
  // ***********************************************************************************

  // ***********************************************************************************
  if(false){
    // 14, no pileup, GOGO
    // fastCanvWrap1(14,0,2,"METsignif","baseline", labels, false, "Entries/(2 GeV^{1/2})", "E_{T}^{miss}/#sqrt{H_{T}} [GeV^{1/2}]", -1);

    // labels.push_back("E_{T}^{miss}/#sqrt{H_{T}} > 15 GeV^{1/2}");
    // labels.push_back("m_{eff} > 3 TeV");
    // fastCanvWrap1(14,0,2,"leadJetPt_HTjets","METsignif15_meff3000", labels, false, "Entries/0.2", "p_{T}^{leading jet}/H_{T}", 10);

    // labels.clear();
    // labels.push_back("E_{T}^{miss}/#sqrt{H_{T}} > 15 GeV^{1/2}");
    // labels.push_back("p_{T}^{leading jet} < 0.4#timesH_{T}");
    // fastCanvWrap1(14,0,20.,"meff","METsignif15_LeadJetLtp4", labels, false, "Entries/200 GeV", "m_{eff} [GeV]", 5);

    cmet = fastCanvWrap1(14,0,10,"MET","METsignif15_LeadJetLtp4", labels, false, "Events/100 GeV in 3ab^{-1}", "E_{T}^{miss} [GeV]", 510,3000.,2700);
    cmet->Print("c_GOGO_MET.pdf");

    cht = fastCanvWrap1(14,0,20.,"HTjets","METsignif15_LeadJetLtp4", labels, false, "Events/200 GeV in 3ab^{-1}", "H_{T} [GeV]", 510,3000.,6500);
    cht->Print("c_GOGO_HT.pdf");

    // 14, no pileup, SQSQB
    cmet = fastCanvWrap1(14,0,10,"MET","METsignif15_LeadJetLtp4", labels, false, "Events/100 GeV in 3ab^{-1}", "E_{T}^{miss} [GeV]", 510,3000.,2700, true, false);
    cmet->Print("c_SQSQB_MET.pdf");

    cht = fastCanvWrap1(14,0,20.,"HTjets","METsignif15_LeadJetLtp4", labels, false, "Events/200 GeV in 3ab^{-1}", "H_{T} [GeV]", 510,3000.,6500, true, false);
    cht->Print("c_SQSQB_HT.pdf");

    // 14, no pileup, GOSQ
    cmet = fastCanvWrap1(14,0,10,"MET","METsignif15_LeadJetLtp4", labels, false, "Events/100 GeV in 3ab^{-1}", "E_{T}^{miss} [GeV]", 510,3000.,2700, false, true);
    cmet->Print("c_GOSQ_MET.pdf");

    cht = fastCanvWrap1(14,0,20.,"HTjets","METsignif15_LeadJetLtp4", labels, false, "Events/200 GeV in 3ab^{-1}", "H_{T} [GeV]", 510,3000.,6500, false, true);
    cht->Print("c_GOSQ_HT.pdf");
  }
  // ***********************************************************************************

  // ***********************************************************************************
  // do 14, 140 pileup 
  if(false){
    // fastCanvWrap1(14,140,2,"METsignif","baseline", labels, false, "Entries/(2 GeV^{1/2})", "E_{T}^{miss}/#sqrt{H_{T}} [GeV^{1/2}]", -1);

    // labels.push_back("E_{T}^{miss}/#sqrt{H_{T}} > 15 GeV^{1/2}");
    // labels.push_back("m_{eff} > 3 TeV");
    // fastCanvWrap1(14,140,2,"leadJetPt_HTjets","METsignif15_meff3000", labels, false, "Entries/0.2", "p_{T}^{leading jet}/H_{T}", 10);

    fastCanvWrap1(14,140,20.,"meff","METsignif15_LeadJetLtp4", labels, false, "Entries/200 GeV", "m_{eff} [GeV]", 5);
  }
  // ***********************************************************************************


  // ***********************************************************************************
  if(false){
    // 33, no pileup, GOGO
    cmet = fastCanvWrap1(33,0,10,"MET","METsignif15_LeadJetLtp4", labels, false, "Events/100 GeV in 3ab^{-1}", "E_{T}^{miss} [GeV]", 510,3000.,5000);
    cmet->Print("c_GOGO_MET.pdf");

    cht = fastCanvWrap1(33,0,20.,"HTjets","METsignif15_LeadJetLtp4", labels, false, "Events/200 GeV in 3ab^{-1}", "H_{T} [GeV]", 505,3000.,15000);
    cht->Print("c_GOGO_HT.pdf");

    // 33, no pileup, SQSQB
    cmet = fastCanvWrap1(33,0,10,"MET","METsignif15_LeadJetLtp4", labels, false, "Events/100 GeV in 3ab^{-1}", "E_{T}^{miss} [GeV]", 510,3000.,5000, true, false);
    cmet->Print("c_SQSQB_MET.pdf");

    cht = fastCanvWrap1(33,0,20.,"HTjets","METsignif15_LeadJetLtp4", labels, false, "Events/200 GeV in 3ab^{-1}", "H_{T} [GeV]", 505,3000.,12000, true, false);
    cht->Print("c_SQSQB_HT.pdf");

    // 33, no pileup, GOSQ
    cmet = fastCanvWrap1(33,0,10,"MET","METsignif15_LeadJetLtp4", labels, false, "Events/100 GeV in 3ab^{-1}", "E_{T}^{miss} [GeV]", 510,3000.,5000, false, true);
    cmet->Print("c_GOSQ_MET.pdf");

    cht = fastCanvWrap1(33,0,20.,"HTjets","METsignif15_LeadJetLtp4", labels, false, "Events/200 GeV in 3ab^{-1}", "H_{T} [GeV]", 505,3000.,12000, false, true);
    cht->Print("c_GOSQ_HT.pdf");
  }
  // ***********************************************************************************

  // ***********************************************************************************
  if(true){
    // 100, no pileup, GOGO
    cmet = fastCanvWrap1(100,0,20,"MET","METsignif15_LeadJetLtp4", labels, false, "Events/200 GeV in 3ab^{-1}", "E_{T}^{miss} [GeV]", 510,3000.,12000);
    cmet->Print("c_GOGO_MET.pdf");
    cmet->Print("c_GOGO_MET.png");

    cht = fastCanvWrap1(100,0,50.,"HTjets","METsignif15_LeadJetLtp4", labels, false, "Events/500 GeV in 3ab^{-1}", "H_{T} [GeV]", 505,3000.,30000);
    cht->Print("c_GOGO_HT.pdf");
    cht->Print("c_GOGO_HT.png");

    // 100, no pileup, SQSQB
    cmet = fastCanvWrap1(100,0,20,"MET","METsignif15_LeadJetLtp4", labels, false, "Events/200 GeV in 3ab^{-1}", "E_{T}^{miss} [GeV]", 510,3000.,12000, true, false);
    cmet->Print("c_SQSQB_MET.pdf");

    cht = fastCanvWrap1(100,0,50.,"HTjets","METsignif15_LeadJetLtp4", labels, false, "Events/500 GeV in 3ab^{-1}", "H_{T} [GeV]", 505,3000.,30000, true, false);
    cht->Print("c_SQSQB_HT.pdf");

    // 100, no pileup, GOSQ
    cmet = fastCanvWrap1(100,0,20,"MET","METsignif15_LeadJetLtp4", labels, false, "Events/200 GeV in 3ab^{-1}", "E_{T}^{miss} [GeV]", 510,3000.,12000, false, true);
    cmet->Print("c_GOSQ_MET.pdf");

    cht = fastCanvWrap1(100,0,50.,"HTjets","METsignif15_LeadJetLtp4", labels, false, "Events/500 GeV in 3ab^{-1}", "H_{T} [GeV]", 505,3000.,30000, false, true);
    cht->Print("c_GOSQ_HT.pdf");
  }
  // ***********************************************************************************

}


TCanvas* comparePileup_HT(){

  TFile* f = new TFile("various_hists.root","RO");

  THStack* bg_000 = get_BG_stack(14,  0, "HTjets", "METsignif15_LeadJetLtp4", 20.);
  TH1F*    bg_140 = get_BG_sum  (14,140, "HTjets", "METsignif15_LeadJetLtp4", 20.);
  bg_140->SetLineColor(kBlue-6);
  bg_140->SetLineStyle(kDashed);
  bg_140->SetLineWidth(3);
  bg_140->SetFillColor(0);
  bg_140->SetFillStyle(0);

  std::vector<std::string> labels;
  labels.push_back("pp#rightarrow#tilde{g}#tilde{g}#rightarrowq#bar{q}#tilde{#chi}_{1}^{0}q#bar{q}#tilde{#chi}_{1}^{0}");
  labels.push_back(Form("#sqrt{s}=%d TeV",14));
  labels.push_back("E_{T}^{miss}/#sqrt{H_{T}} > 15 GeV^{1/2}");
  labels.push_back("p_{T}^{leading jet} < 0.4#timesH_{T}");
  
  std::vector<TH1F*> signals;
  signals.push_back((TH1F*)f->Get("h_GOGO_14_000_1255_0001_METsignif15_LeadJetLtp4_HTjets"));
  signals.push_back((TH1F*)f->Get("h_GOGO_14_140_1255_0001_METsignif15_LeadJetLtp4_HTjets"));

  signals[0]->SetTitle("SUSY: 0 extra int/Xing");
  signals[1]->SetTitle("SUSY: 140 extra int/Xing");

  bool logY=true;
  TCanvas* c = new TCanvas(Form("c_%s","pileupComp"),
			   Form("c_%s","pileupComp"),
			   800,600);
  
  if(logY){
    c->SetLogy(1);
  }
  
  float max=(bg_000 ? bg_000->GetMaximum():0);
  
  bg_000->Draw();
  bg_140->Draw("same");

  for(unsigned int i=0; i<signals.size(); i++){
    signals[i]->Rebin(20);
    signals[i]->SetLineColor(kRed);
    signals[i]->SetLineWidth(3);
    if(signals[i]->GetMaximum()>max) max=signals[i]->GetMaximum();
    signals[i]->Draw("sames");
  }
  signals[1]->SetLineStyle(kDashed);

  if(logY){
    bg_000->SetMinimum(.00001);
    bg_000->SetMaximum(10e3*max);
  }

  bg_000->GetXaxis()->SetRangeUser(0,6500);

  TLegend* l = new TLegend(0.50,0.50,0.85,0.93);
  l->SetFillColor(0);
  l->SetShadowColor(0);

  TIter next(bg_000->GetHists());
  TObject* object = 0;
  std::vector<TH1F*> backgrs;
  while ((object = next())){
    backgrs.push_back((TH1F*)object);
  } 

  for(int i=(int)backgrs.size()-1; i>=0; i--){
    l->AddEntry(backgrs[i],backgrs[i]->GetTitle(),"F");
  }
  l->AddEntry(bg_140, "BG: 140 extra int/Xing", "L");

  for(unsigned int i=0; i<signals.size(); i++){
    l->AddEntry(signals[i],signals[i]->GetTitle(),"L");
  }
  l->Draw();

  for(unsigned int i=0; i<labels.size(); i++){
    myText(0.2,0.89-i*0.06, kBlack, labels[i].c_str(), -1, 0.04);
  }

  bg_000->GetHistogram()->GetYaxis()->SetTitle("fb/(200 GeV)");
  bg_000->GetHistogram()->GetXaxis()->SetTitle("H_{T} [GeV]");
  bg_000->GetHistogram()->GetXaxis()->SetNdivisions(505);
  bg_000->GetHistogram()->GetXaxis()->SetTitleSize(0.06);
  bg_000->GetHistogram()->GetYaxis()->SetTitleSize(0.06);
  bg_000->GetHistogram()->GetXaxis()->SetTitleOffset(1.0);
  bg_000->GetHistogram()->GetYaxis()->SetTitleOffset(1.0);


  c->Update();

  c->Print("c_pileup_comp_HT_14TeV.pdf");
  
  return c;
}


TCanvas* comparePileup_met(){

  TFile* f = new TFile("various_hists.root","RO");

  THStack* bg_000 = get_BG_stack(14,  0, "MET", "METsignif15_LeadJetLtp4", 10.);
  TH1F*    bg_140 = get_BG_sum  (14,140, "MET", "METsignif15_LeadJetLtp4", 10.);
  bg_140->SetLineColor(kBlue-6);
  bg_140->SetLineStyle(kDashed);
  bg_140->SetLineWidth(3);
  bg_140->SetFillColor(0);
  bg_140->SetFillStyle(0);

  std::vector<std::string> labels;
  labels.push_back("pp#rightarrow#tilde{g}#tilde{g}#rightarrowq#bar{q}#tilde{#chi}_{1}^{0}q#bar{q}#tilde{#chi}_{1}^{0}");
  labels.push_back(Form("#sqrt{s}=%d TeV",14));
  labels.push_back("E_{T}^{miss}/#sqrt{H_{T}} > 15 GeV^{1/2}");
  labels.push_back("p_{T}^{leading jet} < 0.4#timesH_{T}");
  
  std::vector<TH1F*> signals;
  signals.push_back((TH1F*)f->Get("h_GOGO_14_000_1255_0001_METsignif15_LeadJetLtp4_MET"));
  signals.push_back((TH1F*)f->Get("h_GOGO_14_140_1255_0001_METsignif15_LeadJetLtp4_MET"));

  signals[0]->SetTitle("SUSY: 0 extra int/Xing");
  signals[1]->SetTitle("SUSY: 140 extra int/Xing");

  bool logY=true;
  TCanvas* c = new TCanvas(Form("c_%s","pileupComp"),
			   Form("c_%s","pileupComp"),
			   800,600);
  
  if(logY){
    c->SetLogy(1);
  }
  
  float max=(bg_000 ? bg_000->GetMaximum():0);
  
  bg_000->Draw();
  bg_140->Draw("same");

  for(unsigned int i=0; i<signals.size(); i++){
    signals[i]->Rebin(10);
    signals[i]->SetLineColor(kRed);
    signals[i]->SetLineWidth(3);
    if(signals[i]->GetMaximum()>max) max=signals[i]->GetMaximum();
    signals[i]->Draw("sames");
  }
  signals[1]->SetLineStyle(kDashed);

  if(logY){
    bg_000->SetMinimum(.00001);
    bg_000->SetMaximum(10e3*max);
  }
  bg_000->GetXaxis()->SetRangeUser(0,3000);

  TLegend* l = new TLegend(0.50,0.50,0.85,0.93);
  l->SetFillColor(0);
  l->SetShadowColor(0);

  TIter next(bg_000->GetHists());
  TObject* object = 0;
  std::vector<TH1F*> backgrs;
  while ((object = next())){
    backgrs.push_back((TH1F*)object);
  } 

  for(int i=(int)backgrs.size()-1; i>=0; i--){
    l->AddEntry(backgrs[i],backgrs[i]->GetTitle(),"F");
  }
  l->AddEntry(bg_140, "BG: 140 extra int/Xing", "L");

  for(unsigned int i=0; i<signals.size(); i++){
    l->AddEntry(signals[i],signals[i]->GetTitle(),"L");
  }
  l->Draw();

  for(unsigned int i=0; i<labels.size(); i++){
    myText(0.2,0.89-i*0.06, kBlack, labels[i].c_str(), -1, 0.04);
  }

  bg_000->GetHistogram()->GetYaxis()->SetTitle("fb/100 GeV");
  bg_000->GetHistogram()->GetXaxis()->SetTitle("E_{T}^{miss} [GeV]");
  bg_000->GetHistogram()->GetXaxis()->SetNdivisions(510);
  bg_000->GetHistogram()->GetXaxis()->SetTitleSize(0.06);
  bg_000->GetHistogram()->GetYaxis()->SetTitleSize(0.06);
  bg_000->GetHistogram()->GetXaxis()->SetTitleOffset(1.0);
  bg_000->GetHistogram()->GetYaxis()->SetTitleOffset(1.0);


  c->Update();

  c->Print("c_pileup_comp_met_14TeV.pdf");
  
  return c;
}
