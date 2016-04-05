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
#include "TGraphDelaunay.h"

using namespace std;

TH2D* TGraph2Histogram(TGraph2D* g)
{
  
  if (!g) return 0;
  
  if (g->GetN() <= 0) {
    Error("GetHistogram", "Empty TGraph2D");
    return 0;
  }

  Double_t hxmin=-1,hymin=-1;
  Double_t hxmax=-1,hymax=-1;
  Double_t *xvals = g->GetX();
  Double_t *yvals = g->GetY();
  Double_t *zvals = g->GetZ();
  for(int i=0; i<g->GetN(); i++){
    if(xvals[i] > hxmax) hxmax=xvals[i];
    if(xvals[i] < hxmin || hxmin < 0) hxmin = xvals[i];
    if(yvals[i] > hymax) hymax=yvals[i];
    if(yvals[i] < hymin || hymin < 0) hymin = yvals[i];
  }

  float nbins=75;

  TH2D* hist  = new TH2D(g->GetName(), g->GetTitle(),
			 nbins, hxmin, hxmax,
			 nbins, hymin, hymax);

  TGraphDelaunay *dt = new TGraphDelaunay(g);
  dt->SetMaxIter(100000);
  dt->SetMarginBinsContent(0);
  TList *hl = hist->GetListOfFunctions();
  hl->Add(dt);

  Double_t dx = (hxmax - hxmin) / nbins;
  Double_t dy = (hymax - hymin) / nbins;

  Double_t x, y, z;
  for (Int_t ix = 1; ix <= nbins; ix++) {
    x  = hxmin + (ix - 0.5) * dx;
    for (Int_t iy = 1; iy <= nbins; iy++) {
      y  = hymin + (iy - 0.5) * dy;
      z  = dt->ComputeZ(x, y);
      hist->Fill(x, y, z);
    }
  }

  return hist;
}


void myText(Double_t x,Double_t y,Color_t color,const char *text, int font=-1, double tsize=.05) {

  TLatex* l = new TLatex; //l.SetTextAlign(12); 
  l->SetTextSize(tsize);
  l->SetNDC(kTRUE);
  l->SetTextColor(color);
  if(font>0) l->SetTextFont(font);
  l->DrawLatex(x,y,text);
}



TCanvas* MakeGrid(std::vector<float>& x, string xname,
		  std::vector<float>& y, string yname,
		  std::vector<float>& z, string zname,
		  string label,
		  bool logZ=true,
		  const std::vector<float>& drawContour=vector<float>(),
		  bool invert=false,
		  int energy=14,
		  int pileup=0,
		  int lumi=3000,
		  int uncertainty=20,
		  string variable="m_eff",
		  string samplelabel="WinoBino"){
  TCanvas* c = new TCanvas(Form("%s_%dTeV_%dPileup_%difb_%s_unc%d_%s",samplelabel.c_str(),energy,pileup,lumi,variable.c_str(),uncertainty,label.c_str()),label.c_str(),800,600);
  c->SetLogz(logZ);
  c->cd(1);
  gPad->SetRightMargin(0.18);

  c->cd();

  double* x_vals = new double[x.size()];
  double* y_vals = new double[y.size()];
  double* z_vals = new double[z.size()];
  double* zinv_vals = new double[z.size()];

  float x_low = 100000000;
  float x_high = 0;
  float y_low = 100000000;
  float y_high = 0;
  float z_low = 0.1;
  float z_high = 0;

  float BIGNUMBER=100.;
  for(unsigned int counter=0; counter<x.size(); counter++){
    x_vals[counter] = x[counter];
    y_vals[counter] = y[counter];
    z_vals[counter] = z[counter];

    if(invert)
      zinv_vals[counter] = BIGNUMBER-z[counter];
    else
      zinv_vals[counter] = z[counter];

    //cout << zinv_vals[counter] << "\t" << z[counter] << endl;

    //cout << x_vals[counter] << "\t" << y_vals[counter] << "\t" << z_vals[counter] << "\t" << zinv_vals[counter] << endl;
    
    if(x_vals[counter] > x_high) x_high = x_vals[counter];
    if(x_vals[counter] < x_low)  x_low = x_vals[counter];
    if(y_vals[counter] > y_high) y_high = y_vals[counter];
    if(y_vals[counter] < y_low)  y_low = y_vals[counter];
    if(z_vals[counter] > z_high) z_high = z_vals[counter];
    if(z_vals[counter] < z_low &&
       (!logZ || z_vals[counter]>0))  z_low = z_vals[counter];
  }

  // some special switches for SQSQ grids, which don't go out as far in X and Y
  if(samplelabel.find("SQ_decoupledGO") != string::npos){
    if(energy==14){
      x_high = 2.5;
      y_high = 2.5;
    }
    else if(energy==33){
      x_high = 5;
      y_high = 5;
    }
    else if(energy==100){
      x_high = 12;
      y_high = 12;
    }
  }

  TGraph2D* g = new TGraph2D(Form("g_%s_%dTeV_%dPileup_%difb_%s_unc%d_%s",samplelabel.c_str(),energy,pileup,lumi,variable.c_str(),uncertainty,label.c_str()),
			     Form("%s_%dTeV_%dPileup_%difb_%s_unc%d_%s",samplelabel.c_str(),energy,pileup,lumi,variable.c_str(),uncertainty,label.c_str()),
			     z.size(),
			     x_vals,y_vals,z_vals);

  bool drawGraph=false;
  TH2D* hist2d=0;
  if(drawGraph){
    g->Draw("COLZ");
    hist2d=g->GetHistogram();

    if(x_high > 1000){
      g->GetXaxis()->SetLimits(x_low,x_high+200);
      g->GetYaxis()->SetLimits(y_low,y_high+200);
    }
    else{
      g->GetXaxis()->SetLimits(x_low,x_high+0.200);
      g->GetYaxis()->SetLimits(y_low,y_high+0.200);
    }

  }
  else{
    hist2d = TGraph2Histogram(g);
    hist2d->Draw("COLZ");
  }

  hist2d->GetXaxis()->SetTitle(xname.c_str());
  hist2d->GetYaxis()->SetTitle(yname.c_str());
  hist2d->GetZaxis()->SetTitle(zname.c_str());
  hist2d->GetXaxis()->SetRangeUser(x_low,x_high);
  hist2d->GetYaxis()->SetRangeUser(y_low,y_high);
  hist2d->GetZaxis()->SetRangeUser(z_low,z_high);
  hist2d->GetXaxis()->SetTitleSize(0.06);
  hist2d->GetYaxis()->SetTitleSize(0.06);
  hist2d->GetZaxis()->SetTitleSize(0.06);
  hist2d->GetZaxis()->SetTitleOffset(1.0);
  hist2d->GetXaxis()->SetTitleOffset(1.0);
  hist2d->GetYaxis()->SetTitleOffset(1.0);

  c->Update();

  if(drawContour.size()>0){
    TCanvas c2("c2","c2",800,600);
    TGraph2D* g2 = new TGraph2D(Form("g_inverse_%s_%dTeV_%dPileup_%difb_%s_unc%d_%s",samplelabel.c_str(),energy,pileup,lumi,variable.c_str(),uncertainty,label.c_str()),
				Form("inverse_%s_%dTeV_%dPileup_%difb_%s_unc%d_%s",samplelabel.c_str(),energy,pileup,lumi,variable.c_str(),uncertainty,label.c_str()),
				z.size(),
				x_vals,y_vals,zinv_vals);
      
    g2->Draw();
    TH2D* hist = g2->GetHistogram();

    // now, draw all the contours 
    // probably a smarter way to do this, but this seems to work.
    for(unsigned int i=0; i<drawContour.size(); i++){
   
      TH2D* newhist = new TH2D(*hist);
      newhist->SetContour(1);
      newhist->SetContourLevel(0,(invert*BIGNUMBER)+(invert ? -1:1)*drawContour[i]);
      newhist->SetLineColor(kBlack);
      newhist->SetLineWidth(4);
      newhist->SetLineStyle(kSolid);
      newhist->Draw("CONTLIST");
      c2.Update();
      if(true){
	TObjArray *contours = (TObjArray*)gROOT->GetListOfSpecials()->FindObject("contours");
	TGraph* mycontour(0);
	if (contours){
	  TList *lcontour1 = (TList*)contours->At(0);
	  if (lcontour1){
	    TGraph *gc1 = (TGraph*)lcontour1->First();
	    if (gc1){
	      mycontour = new TGraph(*gc1);
	    }
	  }
	}
	c->cd();
	if(mycontour){
	  switch(i){
	  case 0: mycontour->SetLineStyle(kSolid);  break;
	  case 1: mycontour->SetLineStyle(kDashed); break;
	  default: mycontour->SetLineStyle(kDotted); break;
	  }
	  mycontour->SetLineColor(kBlack);
	  mycontour->SetLineWidth(4);
	  mycontour->Draw("l");
	  
	  TFile contouroutputfile("contours.root","UPDATE");
	  mycontour->Write(Form("%s_%dTeV_%dPileup_%difb_%s_unc%d_%s_%d",samplelabel.c_str(),energy,pileup,lumi,variable.c_str(),uncertainty,label.c_str(),i));
	}
	else{
	  cout << "no contours!" << endl;
	}
	c2.cd();
      }
      else{
	c->cd();
	newhist->Draw("CONT2 SAMES");
      }
    }
  }

  // draw the grid of points
  TGraph* g3 = new TGraph(x.size(),
			  x_vals,y_vals);
  g3->Draw("P");
  g3->SetMarkerStyle(34);
  g3->SetMarkerSize(1);
  c->Update();
  

  return c;
}


void go_limits(string inputfile,
	       string outputname="",
	       string outputtype="",
	       string samplelabel="",
	       int energy=14,
	       int pileup=0,
	       int lumi=3000,
	       int uncertainty=20,
	       string var="m_eff",
	       string process="",
	       string x_axis_label="m(#tilde{#chi}_{2}^{0}) = m(#tilde{#chi}_{1}^{#pm}) [GeV]",
	       string y_axis_label="m(#tilde{#chi}_{1}^{0}) [GeV]",
	       bool massdiffplots=false,
	       bool flipXY=false){


  string line;
  char variable[100];

  // read in the limits.  the limits should have the same labels as above,
  // so we can use maps to associate them.
  std::vector<float> v_mx,v_my,v_signif;
  float mx,my;
  float signif;
  ifstream myfile (inputfile.c_str());
  if (myfile.is_open()){
    int linecount=0;
    while ( myfile.good() ){
      if(linecount==0){
	linecount++;
	continue;
      }
      getline (myfile,line);
      if(line.find("#") != string::npos) continue;

      if(line.size()<10) continue;

      sscanf(line.c_str(),
	     "%f    %f    %f",
	     &mx,  &my,  &signif);

      if(x_axis_label.find("TeV")!=string::npos){
	mx /= 1000.;
	my /= 1000.;
      }

      if(!flipXY){
	v_mx.push_back(mx);
	v_my.push_back((massdiffplots ? mx-my:my));
      }
      else{
	v_mx.push_back((massdiffplots ? mx-my:my));
	v_my.push_back(mx);
      }
      if(signif<=0)
	signif=0;
      if(signif>10)
	signif=10;

      v_signif.push_back((signif > 0 ? signif:0.));
    }
    myfile.close();
  }
  else{
    cout << "Couldn't find limits file (" << inputfile << ") for this grid!  Aborting." << endl;
    return;
  }

  string varunit="[TeV]";
  if(((string)variable).find("MET") != string::npos){
    strcpy(variable,"E_{T}^{miss}");
  }
  if(((string)variable).find("HT") != string::npos){
    strcpy(variable,"H_{T}");
  }
  if((string)variable == "m_eff"){
    strcpy(variable,"m_{eff}");
  }

  bool logZ=false;
  bool invert=false;
  string z_axis_label="Z_{n} Significance";
  string plotlabel="signif";
  std::vector<float> drawContour;
  drawContour.push_back(1.96);
  drawContour.push_back(5);

  // now the plot!
  TCanvas* c = MakeGrid(v_mx,x_axis_label,
			v_my,y_axis_label,
			v_signif,z_axis_label,
			plotlabel,
			logZ,
			drawContour,
			invert,
			energy,
			pileup,
			lumi,
			uncertainty,
			var,
			samplelabel);
  
  
  int labelcolor = kBlack;
  if(inputfile.find("GOSQ") != string::npos)
    labelcolor = kWhite;
  
  myText(0.2,0.88,labelcolor,process.c_str());
  myText(0.2,0.80,labelcolor,Form("#sqrt{s} = %d TeV",energy));
  myText(0.2,0.72,labelcolor,Form("%d Extra Int/Crossing",pileup));
  myText(0.2,0.64,labelcolor,Form("#scale[.75]{#int}Ldt = %d fb^{-1}",lumi));

  if(c && outputtype!="" && !massdiffplots){
    c->Print(Form("%s_%s_%d_%s.%s",outputname.c_str(),samplelabel.c_str(),lumi,plotlabel.c_str(),outputtype.c_str()));
  }
  else if(c && outputtype!="" && massdiffplots){
    c->Print(Form("%s_%s_%d_%s_massdiff.%s",outputname.c_str(),samplelabel.c_str(),lumi,plotlabel.c_str(),outputtype.c_str()));
  }

  return;
}


// pass in "pdf", "eps", "png", etc to print the plots
void go_sanjay(string printtype=""){
  string grid="GOGO_300ifb";
  int energy=14;
  int pileup=0;
  int lumi=300;
  float uncertainty=0.20;
  string var="meff";
  string label="pp#rightarrow#tilde{g}#tilde{g}#rightarrow#tilde{t}#tilde{#chi}_{1}^{0}#tilde{t}#tilde{#chi}_{1}^{0}";
  string x_axis_label="m_{#tilde{g}} [TeV]";
  string y_axis_label="m_{#tilde{#chi}_{1}^{0}} [TeV]";
  bool massdiffplots=false; // for comparing with compressed scenarios
  bool flipXY=false; // for switching m_g and m_q for GOSQ grids.

  //string infile="sanjay/GOGO_300ifb.txt";
  //string outfile="sanjay/GOGO_300ifb_signif";
  
  string infile="sanjay/stop.txt";
  string outfile="sanjay/stop_signif";

  go_limits(infile, 
	    outfile,
	    printtype,
	    grid,
	    energy,
	    pileup,
	    lumi,
	    uncertainty*100,
	    var,
	    label,
	    x_axis_label,
	    y_axis_label,
	    massdiffplots,
	    flipXY);
  return;
}

