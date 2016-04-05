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

#include "utilities.h"

using namespace std;

TCanvas* MakeGrid(std::map<string,float>& x, string xname,
		  std::map<string,float>& y, string yname,
		  std::map<string,float>& z, string zname,
		  string label,
		  bool logZ=true,
		  float drawContour=-1,
		  bool invert=false,
		  int energy=14,
		  int pileup=0,
		  int lumi=3000,
		  int uncertainty=20,
		  string variable="m_eff",
		  string samplelabel="WinoBino",
		  int leadjetcut=-1,
		  int metsignifcut=-1){
  string name=PlotName(samplelabel,energy,pileup,lumi,variable,uncertainty,label,leadjetcut,metsignifcut);

  TCanvas* c = new TCanvas(name.c_str(),label.c_str(),800,600);
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

  int counter=0;
  float BIGNUMBER=100.;
  for(std::map<string,float>::iterator z_iter = z.begin(); z_iter != z.end(); ++z_iter,counter++){
    string sample = z_iter->first;
    x_vals[counter] = x[sample];
    y_vals[counter] = y[sample];
    z_vals[counter] = z[sample];

    if(invert)
      zinv_vals[counter] = BIGNUMBER-z[sample];
    else
      zinv_vals[counter] = z[sample];

    //cout << zinv_vals[counter] << "\t" << z[sample] << endl;

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

  //cout << z_low << "\t" << z_high << endl;
  TGraph2D* g = new TGraph2D(Form("g_%s",name.c_str()),
			     name.c_str(),
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

  if(drawContour>=0){
    TCanvas c2("c2","c2",800,600);
    TGraph2D* g2 = new TGraph2D(Form("g_inverse_%s",name.c_str()),
				Form("inverse_%s",name.c_str()),
				z.size(),
				x_vals,y_vals,zinv_vals);
      
    g2->Draw("COLZ");
    TH2D* hist = g2->GetHistogram();
    TH2D* newhist = new TH2D(*hist);
    newhist->SetContour(1);
    newhist->SetContourLevel(0,(invert*BIGNUMBER)+(invert ? -1:1)*drawContour);
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
	mycontour->SetLineStyle(kSolid);
	mycontour->SetLineColor(kBlack);
	mycontour->SetLineWidth(4);
	mycontour->Draw("l");

	if(label == "limit" || label == "disc"){
	  TFile contouroutputfile("contours.root","UPDATE");
	  mycontour->Write(name.c_str());
	}
      }
      else{
	cout << "no contours!" << endl;
      }
    }
    else{
      c->cd();
      newhist->Draw("CONT2 SAMES");
    }
  }


  TGraph* g3 = new TGraph(x.size(),
			  x_vals,y_vals);
  g3->Draw("P");
  g3->SetMarkerStyle(34);
  g3->SetMarkerSize(1);
  c->Update();
  

  return c;
}


TCanvas* Make2D(std::map<string,float>& x, string xname,
		std::map<string,float>& y, string yname,
		string label,
		int energy=14,
		int pileup=0,
		int lumi=3000,
		int uncertainty=20,
		string variable="m_eff",
		int mLSP=1,
		string samplelabel="WinoBino",
		int leadjetcut=-1,
		int metsignifcut=-1){
  string name=PlotName(samplelabel,energy,pileup,lumi,variable,uncertainty,label,leadjetcut,metsignifcut);

  TCanvas* c = new TCanvas(name.c_str(),label.c_str(),800,600);
  c->cd(1);

  c->cd();

  double* x_vals = new double[x.size()];
  double* y_vals = new double[y.size()];

  float x_low = 100000000;
  float x_high = 0;
  float y_low = 100000000;
  float y_high = 0;

  int counter=0;
  for(std::map<string,float>::iterator y_iter = y.begin(); y_iter != y.end(); ++y_iter,counter++){
    string sample = y_iter->first;
    x_vals[counter] = x[sample];
    y_vals[counter] = y[sample];

    if(x_vals[counter] > x_high) x_high = x_vals[counter];
    if(x_vals[counter] < x_low)  x_low = x_vals[counter];
    if(y_vals[counter] > y_high) y_high = y_vals[counter];
    if(y_vals[counter] < y_low)  y_low = y_vals[counter];
  }

  TGraph* g = new TGraph(y.size(),
			 x_vals,y_vals);
  g->SetName (Form("g_%s_mlsp%d",name.c_str(),mLSP));
  g->SetTitle(name.c_str());
			 

  g->Draw("AL");
  g->GetHistogram()->GetXaxis()->SetTitle(xname.c_str());
  g->GetHistogram()->GetYaxis()->SetTitle(yname.c_str());
  g->GetHistogram()->GetXaxis()->SetRangeUser(x_low,x_high);
  g->GetHistogram()->GetYaxis()->SetRangeUser(y_low,y_high);

  if(x_high > 1000){
    g->GetXaxis()->SetLimits(x_low,x_high+200);
    g->GetYaxis()->SetLimits(y_low,y_high+200);
  }
  else{
    g->GetXaxis()->SetLimits(x_low,x_high+0.200);
    g->GetYaxis()->SetLimits(y_low,y_high+0.200);
  }

  TFile cutoutputfile("meffcuts.root","UPDATE");
  g->Write(g->GetName());
  cutoutputfile.Close();

  c->Update();

  c->Update();
  

  return c;
}


void go_limits_2D(string inputfile,
		  string outputname="",
		  string outputtype="",
		  string samplelabel="",
		  int energy=14,
		  int pileup=0,
		  int lumi=3000,
		  int uncertainty=20,
		  string var="m_eff",
		  int mLSP=1,
		  string process="",
		  PlotType plottype=CROSSSECTION,
		  string x_axis_label="m(#tilde{#chi}_{2}^{0}) = m(#tilde{#chi}_{1}^{#pm}) [GeV]",
		  int leadjetcut=-1,
		  int metsignifcut=-1){


  if(plottype == "")
    return;

  string line;
  char label[100];
  char variable[100];
  char variable2[100];

  // read in the limits.  the limits should have the same labels as above,
  // so we can use maps to associate them.
  std::map<string,float> v_median,v_sigma,v_mx,v_my,v_cs,v_cut,v_cut2,v_cutsum,v_eff,v_rej,v_signif,v_acctimesCS,v_nsig,v_preselsig,v_bdteff,v_bdtrej,v_preseleff;
  float merr2,merr1,med,perr1,perr2,p0,signalsigma;
  int mx,my;
  float cs,cut,cut2,eff,rej,preeff,prerej,signif,nsig,nbkg;
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

      // yeah, I know.  Ugly.
      sscanf(line.c_str(),
	     "%s      %d    %d    %f   %s          %f      %f      %f      %f       %f       %f      %f     %f       %f      %f      %f    %f      %f      %f   %f            %s          %f",
	     label,   &mx,  &my,  &cs, variable,   &cut,   &nsig,  &nbkg,  &preeff, &prerej, &eff,   &rej,  &signif, &merr2, &merr1, &med, &perr1, &perr2, &p0, &signalsigma, variable2, &cut2);

      if(signalsigma != signalsigma) signalsigma=10;
      if(signif > 7 && med > 1) med=0.1;
      if(signif > 7 && med <= 0.) med=0.00001;

      if(med>0.0000 && my==mLSP){
	v_median[(string)label] = (med > 5 ? 5:med);
	v_sigma[(string)label] = signalsigma;
	v_mx[(string)label] = mx;
	v_my[(string)label] = my;
	v_cs[(string)label] = cs*1000.;
	v_cut[(string)label] = cut/1000.;
	v_cut2[(string)label] = cut2/1000.;
	v_rej[(string)label] = nbkg;
	v_signif[(string)label] = (signif > 0 ? signif:0.);
	v_nsig[(string)label] = nsig;

 	if(x_axis_label.find("TeV")!=string::npos){
	  v_mx[(string)label] /= 1000.;
	  v_my[(string)label] /= 1000.;
	}

	v_cutsum[(string)label] = v_cut2[(string)label]+v_cut[(string)label];

	v_preselsig[(string)label] = (nsig/eff)*preeff;

	v_preseleff[(string)label] = preeff;
	v_eff[(string)label] = eff;
	v_bdteff[(string)label] = eff/preeff;
	v_bdtrej[(string)label] = rej/prerej;

	v_acctimesCS[(string)label] = eff*cs*1000.;

      }
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

  if(((string)variable2).find("MET") != string::npos){
    strcpy(variable2,"E_{T}^{miss}");
  }
  if(((string)variable2).find("HT") != string::npos || var.find("HT") != string::npos){
    strcpy(variable2,"H_{T}");
  }
  if((string)variable2 == "m_eff"){
    strcpy(variable2,"m_{eff}");
  }


  std::map<string,float>& xaxis = v_mx;
  std::map<string,float> yaxis;
  string plotlabel,y_axis_label;
  std::vector<float> drawContour;
  switch(plottype){
  case MASSCUT:      yaxis = v_cut;        plotlabel = "masscut";       y_axis_label=Form("%s cut %s", variable, varunit.c_str()); break;
  case MASSCUT2:     yaxis = v_cut2;       plotlabel = "masscut2";      y_axis_label=Form("%s cut %s", variable2, varunit.c_str()); break;
  case MASSCUTSUM:   yaxis = v_cutsum;     plotlabel = "masscutsum";    y_axis_label=Form("Combined cut %s", varunit.c_str()); break;
  default:
    return;
  }

  // now the plot!
  TCanvas* c = Make2D(xaxis,x_axis_label,
		      yaxis,y_axis_label,
		      plotlabel,
		      energy,
		      pileup,
		      lumi,
		      uncertainty,
		      var,
		      mLSP,
		      samplelabel,
		      leadjetcut,
		      metsignifcut);
  
  
  
  myText(0.2,0.88,kBlack,process.c_str());
  myText(0.2,0.80,kBlack,Form("#sqrt{s} = %d TeV",energy));
  myText(0.2,0.72,kBlack,Form("%d Extra Int/Crossing",pileup));
  if(plottype != CROSSSECTION){
    myText(0.2,0.64,kBlack,Form("#scale[.75]{#int}Ldt = %d fb^{-1}",lumi));
  }
  else{
    /*
    // want to draw a line at the cross section where we'd get 3 signal events.
    string sample_at_lumi;
    for(std::map<string,float>::iterator z_iter = zaxis.begin(); z_iter != zaxis.end(); ++z_iter){
      if(z_iter->second > 3./lumi)
	sample_at_lumi=z_iter->first;
    }
    float x_at_lumi = xaxis[sample_at_lumi];
    TLine* mline = new TLine;
    mline->SetLineWidth(4);
    mline->SetLineColor(1);
    mline->SetLineStyle(1);
    c->cd();
    mline->DrawLine(x_at_lumi,0,x_at_lumi,1000);
    */
  }
  if(c && outputtype!=""){
    c->Print(Form("%s_%s_%d_%s.%s",outputname.c_str(),samplelabel.c_str(),lumi,plotlabel.c_str(),outputtype.c_str()));
  }

  return;
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
	       PlotType plottype=CROSSSECTION,
	       string x_axis_label="m(#tilde{#chi}_{2}^{0}) = m(#tilde{#chi}_{1}^{#pm}) [GeV]",
	       string y_axis_label="m(#tilde{#chi}_{1}^{0}) [GeV]",
	       bool massdiffplots=false,
	       bool flipXY=false,
	       int leadjetcut=-1,
	       int metsignifcut=-1){


  if(plottype == "")
    return;

  string line;
  char label[100];
  char variable[100];
  char variable2[100];

  // read in the limits.  the limits should have the same labels as above,
  // so we can use maps to associate them.
  std::map<string,float> v_median,v_sigma,v_mx,v_my,v_cs,v_cut,v_cut2,v_cutsum,v_eff,v_rej,v_signif,v_acctimesCS,v_nsig,v_preselsig,v_bdteff,v_bdtrej,v_preseleff;
  float merr2,merr1,med,perr1,perr2,p0,signalsigma;
  int mx,my;
  float cs,cut,cut2,eff,rej,preeff,prerej,signif,nsig,nbkg;
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

      // yeah, I know.  Ugly.
      sscanf(line.c_str(),
	     "%s      %d    %d    %f   %s          %f      %f      %f      %f       %f       %f      %f     %f       %f      %f      %f    %f      %f      %f   %f            %s          %f",
	     label,   &mx,  &my,  &cs, variable,   &cut,   &nsig,  &nbkg,  &preeff, &prerej, &eff,   &rej,  &signif, &merr2, &merr1, &med, &perr1, &perr2, &p0, &signalsigma, variable2, &cut2);

      if(signalsigma != signalsigma) signalsigma=10;
      if(signif > 7 && med > 1) med=0.1;
      if(signif > 7 && med <= 0.) med=0.00001;

      if(med>0.){
	v_median[(string)label] = (med > 5 ? 5:med);
	v_sigma[(string)label] = signalsigma;
	if(!flipXY){
	  v_mx[(string)label] = mx;
	  v_my[(string)label] = (massdiffplots ? mx-my:my);
	}
	else{
	  v_mx[(string)label] = (massdiffplots ? mx-my:my);
	  v_my[(string)label] = mx;
	}
	v_cs[(string)label] = cs;
	v_cut[(string)label] = cut/1000.;
	v_cut2[(string)label] = cut2/1000.;
	v_rej[(string)label] = nbkg;
	if(std::isnan(signif)) signif=10;
	v_signif[(string)label] = (signif > 0 ? signif:0.);
	v_nsig[(string)label] = nsig;

 	if(x_axis_label.find("TeV")!=string::npos){
	  v_mx[(string)label] /= 1000.;
	  v_my[(string)label] /= 1000.;
	}

	v_cutsum[(string)label] = v_cut2[(string)label]+v_cut[(string)label];

	v_preselsig[(string)label] = (nsig/eff)*preeff;

	v_preseleff[(string)label] = preeff;
	v_eff[(string)label] = eff;
	v_bdteff[(string)label] = eff/preeff;
	v_bdtrej[(string)label] = rej/prerej;

	v_acctimesCS[(string)label] = eff*cs*1000.;

      }
      else{
	cout << "skipping point " << mx << "\t" << my << "\t" << med << "\t" << signif << "\t" << signalsigma << endl;
      }
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

  if(((string)variable2).find("MET") != string::npos){
    strcpy(variable2,"E_{T}^{miss}");
  }
  if(((string)variable2).find("HT") != string::npos || var.find("HT") != string::npos){
    strcpy(variable2,"H_{T}");
  }
  if((string)variable2 == "m_eff"){
    strcpy(variable2,"m_{eff}");
  }

  std::map<string,float>& xaxis = v_mx;
  std::map<string,float>& yaxis = v_my;
  std::map<string,float> zaxis;
  string plotlabel,z_axis_label;
  bool logZ=false;
  std::vector<float> drawContour;
  bool invert=false;
  switch(plottype){
  case CROSSSECTION: zaxis = v_cs;         plotlabel = "CS";        z_axis_label="#sigma [pb]";                                  logZ=true;  drawContour.push_back(-1); invert=false; break; 
  case ACCEPTANCE:   zaxis = v_eff;        plotlabel = "eff";       z_axis_label="Signal Acceptance";                            logZ=true;  drawContour.push_back(-1); invert=false; break; // 1
  case CSA:          zaxis = v_acctimesCS; plotlabel = "effCS";     z_axis_label="#sigma #times BR #times Acc. [fb]";            logZ=true;  drawContour.push_back(-1); invert=false; break;
  case LIMIT:        zaxis = v_median;     plotlabel = "limit";     z_axis_label="95% CL Upper Limit on #sigma / #sigma_{SUSY}"; logZ=false; drawContour.push_back( 1); invert= true; break; // 3
  case DISCOVERY:    zaxis = v_sigma;      plotlabel = "disc";      z_axis_label="Discovery Significance [#sigma]";              logZ=false; drawContour.push_back( 5); invert=false; break;
  case SIGNIFICANCE: zaxis = v_signif;     plotlabel = "signif";    z_axis_label="Discovery Significance [Z_{n}]";               logZ=false; drawContour.push_back( 5); invert=false; break; // 5
  case NUMSIG:       zaxis = v_nsig;       plotlabel = "nsig";      z_axis_label="Signal Events (Full Selection)";               logZ=true;  drawContour.push_back( 3); invert=false; break;
  case PRESELSIG:    zaxis = v_preselsig;  plotlabel = "preselsig"; z_axis_label="Signal Events (Preselection)";                 logZ=true;  drawContour.push_back(-1); invert=false; break; // 7
  case PRESELEFF:    zaxis = v_preseleff;  plotlabel = "preseleff"; z_axis_label="Signal Acceptance (Preselection)";             logZ=false; drawContour.push_back(-1); invert=false; break;
  case BDTEFF:       zaxis = v_bdteff;     plotlabel = "bdteff";    z_axis_label="Signal Acceptance (Full/Presel)";              logZ=false; drawContour.push_back(-1); invert=false; break; // 9
  case CUT:          zaxis = v_cut;        plotlabel = "cut";       z_axis_label=Form("%s cut %s", variable, varunit.c_str());   logZ=false; drawContour.push_back(-1); invert=false; break;
  case CUT2:         zaxis = v_cut2;       plotlabel = "cut2";      z_axis_label=Form("%s cut %s", variable2,varunit.c_str());   logZ=false; drawContour.push_back(-1); invert=false; break; // 11
  case CUTSUM:       zaxis = v_cutsum;     plotlabel = "cutsum";    z_axis_label=Form("Combined cut %s",varunit.c_str());        logZ=false; drawContour.push_back(-1); invert=false; break;
  default:
    return;
  }

  // now the plot!
  TCanvas* c = MakeGrid(xaxis,x_axis_label,
			yaxis,y_axis_label,
			zaxis,z_axis_label,
			plotlabel,
			logZ,
			drawContour[0],
			invert,
			energy,
			pileup,
			lumi,
			uncertainty,
			var,
			samplelabel,
			leadjetcut,
			metsignifcut);
  
  
  int labelcolor = kBlack;
  if(inputfile.find("GOSQ") != string::npos)
    labelcolor = kWhite;
  
  myText(0.2,0.88,labelcolor,process.c_str());
  myText(0.2,0.80,labelcolor,Form("#sqrt{s} = %d TeV",energy));
  myText(0.2,0.72,labelcolor,Form("%d Extra Int/Crossing",pileup));
  if(plottype != CROSSSECTION){
    myText(0.2,0.64,labelcolor,Form("#scale[.75]{#int}Ldt = %d fb^{-1}",lumi));
  }
  else{
    /*
    // want to draw a line at the cross section where we'd get 3 signal events.
    string sample_at_lumi;
    for(std::map<string,float>::iterator z_iter = zaxis.begin(); z_iter != zaxis.end(); ++z_iter){
      if(z_iter->second > 3./lumi)
	sample_at_lumi=z_iter->first;
    }
    float x_at_lumi = xaxis[sample_at_lumi];
    TLine* mline = new TLine;
    mline->SetLineWidth(4);
    mline->SetLineColor(1);
    mline->SetLineStyle(1);
    c->cd();
    mline->DrawLine(x_at_lumi,0,x_at_lumi,1000);
    */
  }
  if(c && outputtype!="" && !massdiffplots){
    c->Print(Form("%s_%s_%d_%s.%s",outputname.c_str(),samplelabel.c_str(),lumi,plotlabel.c_str(),outputtype.c_str()));
  }
  else if(c && outputtype!="" && massdiffplots){
    c->Print(Form("%s_%s_%d_%s_massdiff.%s",outputname.c_str(),samplelabel.c_str(),lumi,plotlabel.c_str(),outputtype.c_str()));
  }

  return;
}

/*
void go_limits_wrap(){
  go_limits("../data/GO_decoupledSQ_13TeV/scan_summary.dat",
	    "GO_decoupledSQ_13TeV/limits_cut1000.3000fb.txt", 
	    "GO_decoupledSQ_13TeV/limits_cut1000",
	    "pdf",
	    "strong",
	    13,
	    3000);
}
*/


void go_plot(string grid="WinoBino", int lumi=300, int pt=0, string var="BDTScore", string printtype="",
	     int energy=14,
	     int pileup=0,
	     float uncertainty=0.20,
	     string label="pp#rightarrow#tilde{#chi}_{1}^{#pm}#tilde{#chi}_{2}^{0}#rightarrowW#tilde{#chi}_{1}^{0}Z#tilde{#chi}_{1}^{0}",
	     string x_axis_label="m(#tilde{#chi}_{2}^{0}) = m(#tilde{#chi}_{1}^{#pm}) [GeV]",
	     string y_axis_label="m(#tilde{#chi}_{1}^{0}) [GeV]",
	     bool massdiffplots=false,
	     bool flipXY=false,
	     int leadjetcut=1,
	     int metsignifcut=15){

  string uncstring="";
  string uncstring2="";
  int relunc = uncertainty*100;
  if(uncertainty < 0.10){
    uncstring = Form(".unc_0.0%d",relunc);
    uncstring2 = Form("unc_0%d",relunc);
  }
  else{
    uncstring = Form(".unc_0.%d",relunc);
    uncstring2 = Form("unc_%d",relunc);
  }

  string leadjetstr="",leadjetlabel="";
  string metsignifstr="",metsigniflabel="";
  if(leadjetcut>=0 && metsignifcut>=0){
    leadjetstr=Form(".leadjetcut_%d",leadjetcut);
    metsignifstr=Form(".metsignif_%d",metsignifcut);
    leadjetlabel=Form("_leadjetcut_%d",leadjetcut);
    metsigniflabel=Form("_metsignif_%d",metsignifcut);
  }

  // CROSSSECTION, ACCEPTANCE, CSA, LIMIT, DISCOVERY, SIGNIFICANCE 
  string inputfile=Form("%s/limits_opt.%s.lumi%d.%sPileUp%s%s%s.txt",grid.c_str(),var.c_str(),lumi,(pileup == 0 ? "No":Form("%d",pileup)),uncstring.c_str() ,leadjetstr.c_str(),metsignifstr.c_str());
  string outputfile=Form("%s/limits_opt_%s_%sPileUp_%s%s%s"         ,grid.c_str(),var.c_str(),     (pileup == 0 ? "No":Form("%d",pileup)),uncstring2.c_str(),leadjetlabel.c_str(),metsigniflabel.c_str());
  if(pt<MASSCUT){
    go_limits(inputfile, 
	      outputfile,
	      printtype,
	      grid,
	      energy,
	      pileup,
	      lumi,
	      uncertainty*100,
	      var,
	      label,
	      (PlotType)pt,
	      x_axis_label,
	      y_axis_label,
	      massdiffplots,
	      flipXY,
	      leadjetcut,
	      metsignifcut);
  }
  else{
    go_limits_2D(inputfile, 
		 outputfile,
		 printtype,
		 grid,
		 energy,
		 pileup,
		 lumi,
		 uncertainty*100,
		 var,
		 1,
		 label,
		 (PlotType)pt,
		 x_axis_label,
		 leadjetcut,
		 metsignifcut);
  }
}


void go_DG_plot_wrap(string printtype="pdf"){
  for(int i=0; i<13; i++){
    //go_plot("WinoBino",   3,(PlotType)i, "BDTScore", printtype);
    //go_plot("WinoBino",  30,(PlotType)i, "BDTScore", printtype);
    //go_plot("WinoBino", 300,(PlotType)i, "BDTScore", printtype);
    //go_plot("WinoBino",3000,(PlotType)i, "BDTScore", printtype);
    //go_plot("WinoBino",   3,(PlotType)i, "MET", printtype);
    //go_plot("WinoBino",  30,(PlotType)i, "MET", printtype);
    //go_plot("WinoBino", 300,(PlotType)i, "MET", printtype);
    go_plot("WinoBino",3000,(PlotType)i, "MET", printtype);
    //go_plot("WinoBino",3000,(PlotType)i, "Beate4c", printtype);
  }
}


void go_GOGO_plot(int lumi=3, int pileup=0, int pt=4, string plottype="", int energy=14, bool massdiffplots=false, string optstr="m_eff", float uncertainty=0.20, int leadjetcut=1, int metsignifcut=15){

  string suffix="_LHE_QQALL";
  if(energy==100) suffix="";
  go_plot(Form("GO_decoupledSQ_%dTeV%s",energy,suffix.c_str()),lumi,(PlotType)pt,optstr,plottype, energy, pileup, uncertainty,
	  "pp#rightarrow#tilde{g}#tilde{g}#rightarrowq#bar{q}#tilde{#chi}_{1}^{0}q#bar{q}#tilde{#chi}_{1}^{0}",
	  "m_{#tilde{g}} [TeV]",
	  (massdiffplots ? "m_{#tilde{g}}-m_{#tilde{#chi}_{1}^{0}} [TeV]":"m_{#tilde{#chi}_{1}^{0}} [TeV]"),
	  massdiffplots,
	  false,
	  leadjetcut,
	  metsignifcut);
}

void go_SQSQ_plot(int lumi=3, int pileup=0, int pt=4, string plottype="", int energy=14, bool massdiffplots=false, string optstr="m_eff", float uncertainty=0.20, int leadjetcut=1, int metsignifcut=15){

  go_plot(Form("SQ_decoupledGO_%dTeV",energy),lumi,(PlotType)pt,optstr,plottype, energy, pileup, uncertainty,
	  "pp#rightarrow#tilde{q}#tilde{q}#rightarrowq#tilde{#chi}_{1}^{0}#bar{q}#tilde{#chi}_{1}^{0}",
	  "m_{#tilde{q}} [TeV]",
	  (massdiffplots ? "m_{#tilde{q}}-m_{#tilde{#chi}_{1}^{0}} [TeV]":"m_{#tilde{#chi}_{1}^{0}} [TeV]"),
	  massdiffplots,
	  false,
	  leadjetcut,
	  metsignifcut);
}

void go_GOSQ_plot(int lumi=3, int pileup=0, int pt=4, string plottype="", int energy=14, string optstr="m_eff", float uncertainty=0.20, bool flip=false, int leadjetcut=1, int metsignifcut=15){

  go_plot(Form("GOSQ_masslessN0_%dTeV",energy),lumi,(PlotType)pt,optstr,plottype, energy, pileup, uncertainty,
	  "pp#rightarrow#tilde{q}#tilde{q},#tilde{g}#tilde{q},#tilde{g}#tilde{g}",
	  (flip ? "m_{#tilde{q}} [TeV]":"m_{#tilde{g}} [TeV]"),
	  (flip ? "m_{#tilde{g}} [TeV]":"m_{#tilde{q}} [TeV]"),
	  false,
	  flip,
	  leadjetcut,
	  metsignifcut);
}


void go_plot_wrap(int pileup=0, int plottype=3, string optstr="m_eff", string printtype="", float uncertainty=0.20){
  bool massdiffplot=false;

  if(pileup==140){
    go_GOGO_plot( 300,pileup,plottype,printtype, 14,massdiffplot,optstr, uncertainty,1,15);
    go_GOGO_plot(3000,pileup,plottype,printtype, 14,massdiffplot,optstr, uncertainty,1,15);
  }
  else if(optstr=="m_eff"){
    go_GOGO_plot( 300,pileup,plottype,printtype, 14,massdiffplot,optstr, uncertainty,1,15);
    go_GOGO_plot(3000,pileup,plottype,printtype, 14,massdiffplot,optstr, uncertainty,1,15);
    go_GOGO_plot( 300,pileup,plottype,printtype, 33,massdiffplot,optstr, uncertainty,1,15);
    go_GOGO_plot(3000,pileup,plottype,printtype, 33,massdiffplot,optstr, uncertainty,1,15);
    go_GOGO_plot( 300,pileup,plottype,printtype,100,massdiffplot,optstr, uncertainty,1,15);
    go_GOGO_plot(3000,pileup,plottype,printtype,100,massdiffplot,optstr, uncertainty,1,15);

    /*
    if((int)(uncertainty*100)==20){
      go_GOSQ_plot( 300,pileup,plottype,printtype, 14,optstr, uncertainty,false,1,15);
      go_GOSQ_plot(3000,pileup,plottype,printtype, 14,optstr, uncertainty,false,1,15);
      go_GOSQ_plot( 300,pileup,plottype,printtype, 33,optstr, uncertainty,false,1,15);
      go_GOSQ_plot(3000,pileup,plottype,printtype, 33,optstr, uncertainty,false,1,15);
      go_GOSQ_plot( 300,pileup,plottype,printtype,100,optstr, uncertainty,false,1,15);
      go_GOSQ_plot(3000,pileup,plottype,printtype,100,optstr, uncertainty,false,1,15);

      go_SQSQ_plot( 300,pileup,plottype,printtype, 14,massdiffplot,optstr, uncertainty,1,15);
      go_SQSQ_plot(3000,pileup,plottype,printtype, 14,massdiffplot,optstr, uncertainty,1,15);
      go_SQSQ_plot( 300,pileup,plottype,printtype, 33,massdiffplot,optstr, uncertainty,1,15);
      go_SQSQ_plot(3000,pileup,plottype,printtype, 33,massdiffplot,optstr, uncertainty,1,15);
      go_SQSQ_plot( 300,pileup,plottype,printtype,100,massdiffplot,optstr, uncertainty,1,15);
      go_SQSQ_plot(3000,pileup,plottype,printtype,100,massdiffplot,optstr, uncertainty,1,15);
    }
    */
  }
  else{
    go_GOGO_plot( 300,pileup,plottype,printtype, 14,massdiffplot,optstr, uncertainty,1,15);
    go_GOGO_plot(3000,pileup,plottype,printtype, 14,massdiffplot,optstr, uncertainty,1,15);
    go_GOGO_plot( 300,pileup,plottype,printtype, 33,massdiffplot,optstr, uncertainty,1,15);
    go_GOGO_plot(3000,pileup,plottype,printtype, 33,massdiffplot,optstr, uncertainty,1,15);
    go_GOGO_plot(3000,pileup,plottype,printtype, 33,massdiffplot,optstr, uncertainty,1,23);
    go_GOGO_plot( 300,pileup,plottype,printtype,100,massdiffplot,optstr, uncertainty,1,15);
    go_GOGO_plot(3000,pileup,plottype,printtype,100,massdiffplot,optstr, uncertainty,1,15);
    go_GOGO_plot(3000,pileup,plottype,printtype,100,massdiffplot,optstr, uncertainty,1,40);

    if((int)(uncertainty*100)==20){
      go_GOSQ_plot( 300,pileup,plottype,printtype, 14,optstr, uncertainty,false,1,15);
      go_GOSQ_plot(3000,pileup,plottype,printtype, 14,optstr, uncertainty,false,1,15);
      go_GOSQ_plot( 300,pileup,plottype,printtype, 33,optstr, uncertainty,false,1,15);
      go_GOSQ_plot(3000,pileup,plottype,printtype, 33,optstr, uncertainty,false,1,15);
      go_GOSQ_plot( 300,pileup,plottype,printtype,100,optstr, uncertainty,false,1,15);
      go_GOSQ_plot(3000,pileup,plottype,printtype,100,optstr, uncertainty,false,1,15);

      go_SQSQ_plot( 300,pileup,plottype,printtype, 14,massdiffplot,optstr, uncertainty,1,15);
      go_SQSQ_plot(3000,pileup,plottype,printtype, 14,massdiffplot,optstr, uncertainty,1,15);
      go_SQSQ_plot(3000,pileup,plottype,printtype, 14,massdiffplot,optstr, uncertainty,0,15);
      go_SQSQ_plot( 300,pileup,plottype,printtype, 33,massdiffplot,optstr, uncertainty,1,15);
      go_SQSQ_plot(3000,pileup,plottype,printtype, 33,massdiffplot,optstr, uncertainty,1,15);
      go_SQSQ_plot(3000,pileup,plottype,printtype, 33,massdiffplot,optstr, uncertainty,0,15);
      go_SQSQ_plot( 300,pileup,plottype,printtype,100,massdiffplot,optstr, uncertainty,1,15);
      go_SQSQ_plot(3000,pileup,plottype,printtype,100,massdiffplot,optstr, uncertainty,1,15);
      go_SQSQ_plot(3000,pileup,plottype,printtype,100,massdiffplot,optstr, uncertainty,0,15);
    }
  }
}

void make_all_contours(string plottype=""){
  for(int i=0; i<16; i++){
    go_plot_wrap(0,i,"MET___HTjets_30_3p5",plottype,0.30);
    go_plot_wrap(0,i,"MET___HTjets_30_3p5",plottype,0.20);
    go_plot_wrap(0,i,"MET___HTjets_30_3p5",plottype,0.10);
    go_plot_wrap(0,i,"MET___HTjets_30_3p5",plottype,0.05);
    go_plot_wrap(0,i,"m_eff",plottype,0.30);
    go_plot_wrap(0,i,"m_eff",plottype,0.20);
    go_plot_wrap(0,i,"m_eff",plottype,0.10);
    go_plot_wrap(0,i,"m_eff",plottype,0.05);

    go_plot_wrap(140,i,"MET___HTjets_30_3p5",plottype,0.20);
  }
}

