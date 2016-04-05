#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <string>

#include "TString.h"

using namespace std;

string PlotName(string grid,
		int energy,
		int pileup,
		int lumi,
		string variable,
		float uncertainty,
		string label,
		int leadjetcut=1,
		int metsignifcut=15){
  int newunc=(uncertainty<1 ? uncertainty*100:uncertainty);
  if(leadjetcut>=0 && metsignifcut>=0)
    return Form("%s_%dTeV_%dPileup_%difb_%s_unc%d_leadjetcut_%d_metsignif_%d_%s",
		grid.c_str(),
		energy,
		pileup,
		lumi,
		variable.c_str(),
		newunc,
		leadjetcut,
		metsignifcut,
		label.c_str());
  else
    return Form("%s_%dTeV_%dPileup_%difb_%s_unc%d_%s",
		grid.c_str(),
		energy,
		pileup,
		lumi,
		variable.c_str(),
		newunc,
		label.c_str());
}

enum PlotType { CROSSSECTION,  //  0
		ACCEPTANCE,    //  1
		CSA,           //  2
		LIMIT,         //  3
		DISCOVERY,     //  4
		SIGNIFICANCE,  //  5
		NUMSIG,        //  6
		PRESELSIG,     //  7
		PRESELEFF,     //  8
		BDTEFF,        //  9
		CUT,           // 10
		CUT2,          // 11
		CUTSUM,        // 12
		MASSCUT,       // 13
		MASSCUT2,      // 14
		MASSCUTSUM     // 15
};


void myText(Double_t x,Double_t y,Color_t color,const char *text, int font=-1, double tsize=.05) {

  TLatex* l = new TLatex; //l.SetTextAlign(12); 
  l->SetTextSize(tsize);
  l->SetNDC(kTRUE);
  l->SetTextColor(color);
  if(font>0) l->SetTextFont(font);
  l->DrawLatex(x,y,text);
}

TGraph ** h2graph(TH2D * in_hist) // could be TH2F, etc.
{
  TCanvas temp_can;         // This avoids disturbing canvases already in use.
  temp_can.cd();            // Yes, we do need a canvas for this to work, and
  in_hist->Draw("contlist");// yes, we do need to call Update() on it after
  temp_can.Update();        // Draw(). No, ROOT is not very friendly.

  TObjArray * plah = (TObjArray*)gROOT->GetListOfSpecials()->FindObject("contours");

  TGraph ** agr = (TGraph **)malloc(plah->GetSize() * sizeof(TGraph *));

  for(int i = 0; i < plah->GetSize(); i++){
    TList * list = (TList*) plah->At(i);
    agr[i] = new TGraph(*((TGraph*) list->First()));
    agr[i]->SetLineColor(in_hist->GetLineColor()); // totally optional
  }

  return agr;
}


TH2D* TGraph2Histogram(TGraph2D* g, int nbins=75){
  
  if (!g || g->GetN() <= 0){
    Error("GetHistogram", "Empty TGraph2D");
    return 0;
  }

  Double_t hxmin=-1,hymin=-1;
  Double_t hxmax=-1,hymax=-1;
  Double_t *xvals = g->GetX();
  Double_t *yvals = g->GetY();
  for(int i=0; i<g->GetN(); i++){
    if(xvals[i] > hxmax)              hxmax = xvals[i];
    if(xvals[i] < hxmin || hxmin < 0) hxmin = xvals[i];
    if(yvals[i] > hymax)              hymax = yvals[i];
    if(yvals[i] < hymin || hymin < 0) hymin = yvals[i];
  }

  TH2D* hist  = new TH2D(Form("h_%s",g->GetName()), 
			 g->GetTitle(),
			 nbins, hxmin, hxmax,
			 nbins, hymin, hymax);

  TGraphDelaunay *dt = new TGraphDelaunay(g);
  //dt->SetMaxIter(fMaxIter);
  //dt->SetMarginBinsContent(fZout);
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


#endif
