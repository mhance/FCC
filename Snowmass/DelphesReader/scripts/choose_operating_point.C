#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TParameter.h"

#include "RooStats/NumberCountingUtils.h"

using namespace std;

void choose_operating_point(string signal,
			    std::vector<string> backgrounds,
			    string variable,
			    string treename,
			    float lumi,
			    float minimum,
			    string extracut="",
			    float kfactor=1.,
			    float bkguncert=0.20,
			    string preselectioncut="",
			    bool capsignif=true,
			    bool extrapolate=false,
			    bool debug=false){

  TFile sigfile(signal.c_str(),"RO");
  TTree* sigtree = (TTree*)sigfile.Get(treename.c_str());

  TChain* baktree = new TChain(treename.c_str());
  for(unsigned int i=0; i<backgrounds.size(); i++)
    baktree->AddFile(backgrounds[i].c_str());

  if(!sigtree || !baktree){
    cout << "Can't do anything without signal or background tree!" << endl;
    cout << "Could not find an optimal working point." << endl;
    return;
  }
  
  if(variable == ""){
    // if we don't specify a variable to optimize, then just output the results
    // of the rectangular cuts passed in via extracut.
    TH1F* sighist = new TH1F("sighist","sighist",2,-1e9,1e9);
    TH1F* bakhist = new TH1F("bakhist","bakhist",2,-1e9,1e9);

    sigtree->Draw("MET>>sighist","weight*(weight>0)","goff");
    baktree->Draw("MET>>bakhist","weight*(weight>0)","goff");
    
    float totsig=sighist->GetSumOfWeights()*lumi;
    float totbak=bakhist->GetSumOfWeights()*lumi;

    float preselsig=totsig;
    float preselbak=totbak;

    if(preselectioncut!=""){
      sigtree->Draw("MET>>sighist",Form("weight*(%s)",preselectioncut.c_str()),"goff");
      baktree->Draw("MET>>bakhist",Form("weight*(%s)",preselectioncut.c_str()),"goff");
      
      preselsig=sighist->GetSumOfWeights()*lumi;
      preselbak=bakhist->GetSumOfWeights()*lumi;
    }

    if(extracut=="")
      extracut="weight>0.";

    
    if(preselectioncut!=""){
      extracut+="&&";
      extracut+=preselectioncut;
    }

    sigtree->Draw("MET>>sighist",Form("weight*(%s)",extracut.c_str()),"goff");
    baktree->Draw("MET>>bakhist",Form("weight*(%s)",extracut.c_str()),"goff");

    float sigaftercut=sighist->GetSumOfWeights()*lumi*kfactor;
    float bakaftercut=bakhist->GetSumOfWeights()*lumi;
    
    float signif=0;
    if(sigaftercut>0)
      signif = RooStats::NumberCountingUtils::BinomialExpZ(sigaftercut, bakaftercut, 0.30); // Z_n

    if(debug)
      cout << std::setw(20) << "Variable"
	   << std::setw(20) << "Cut"
	   << std::setw(20) << "NSignal"
	   << std::setw(20) << "NBackgr"
	   << std::setw(20) << "Significance"
	   << std::setw(20) << "epsSignal"
	   << std::setw(20) << "epsBackgr"
	   << endl;
    
    cout << std::setw(20) << "RectCut"
	 << std::setw(20) << -1
	 << std::setw(20) << sigaftercut
	 << std::setw(20) << bakaftercut
	 << std::setw(20) << signif // (signif <= 10. ? signif:10)
	 << std::setw(20) << (totsig > 0. ? sigaftercut/totsig:0)
	 << std::setw(20) << (totbak > 0. ? bakaftercut/totbak:0)
	 << std::setw(20) << (preselsig > 0. ? sigaftercut/preselsig:0)
	 << std::setw(20) << (preselbak > 0. ? bakaftercut/preselbak:0)
	 << endl;
    return;
  }
  else{
    cout << "Probing variable " << variable << endl;

    float sigmin = sigtree->GetMinimum(variable.c_str());
    float sigmax = sigtree->GetMaximum(variable.c_str());
    float bakmin = baktree->GetMinimum(variable.c_str());
    float bakmax = baktree->GetMaximum(variable.c_str());

    float min = (sigmin < bakmin ? sigmin:bakmin);
    float max = (sigmax > bakmax ? sigmax:bakmax);
  
    int n_bins=400;

    TH1F* sighist = new TH1F("sighist","sighist",n_bins,min,max);
    TH1F* bakhist = new TH1F("bakhist","bakhist",n_bins,min,max);

    cout << "... drawing " << variable << endl;

    sigtree->Draw(Form("%s>>sighist",variable.c_str()),"weight*(weight>0.)","goff");
    baktree->Draw(Form("%s>>bakhist",variable.c_str()),"weight*(weight>0.)","goff");

    float totsig=sighist->GetSumOfWeights()*lumi*kfactor;
    float totbak=bakhist->GetSumOfWeights()*lumi;

    float preselsig=totsig;
    float preselbak=totbak;

    if(preselectioncut!=""){
      sigtree->Draw(Form("%s>>sighist",variable.c_str()),Form("weight*(%s)",preselectioncut.c_str()),"goff");
      baktree->Draw(Form("%s>>bakhist",variable.c_str()),Form("weight*(%s)",preselectioncut.c_str()),"goff");
      
      preselsig=sighist->GetSumOfWeights()*lumi;
      preselbak=bakhist->GetSumOfWeights()*lumi;
    }
    
    if(extracut=="")
      extracut="weight>0.";

    if(preselectioncut!=""){
      extracut+="&&";
      extracut+=preselectioncut;
    }

    string tempvariable=variable;
    if(variable=="m_eff"){
      tempvariable="HTjets_30_2p5+MET";
    }

    sigtree->Draw(Form("%s>>sighist",tempvariable.c_str()),
		  Form("weight*(%s)",extracut.c_str()),
		  "goff");
    baktree->Draw(Form("%s>>bakhist",tempvariable.c_str()),
		  Form("weight*(%s)",extracut.c_str()),
		  "goff");

    cout << "... drew histograms with cut " << Form("weight*(%s)",extracut.c_str()) << endl;

    sighist->Scale(lumi*kfactor);
    bakhist->Scale(lumi);
  
    float sigtotal=0;
    float baktotal=0;
    float signif=0;
    float maxsignif=-999999;
    int maxsignifbin=-1;
    float sigtotalatmaxsignif=0;
    float baktotalatmaxsignif=0;
    for(int i=0; i<n_bins; i++){
      sigtotal += sighist->GetBinContent(n_bins-i);
      baktotal += bakhist->GetBinContent(n_bins-i);

      // signif = sigtotal/TMath::Sqrt(sigtotal+baktotal);

      // we need some way to take the background uncertainty into account...  for now just assuming
      // the uncertainty is 30%, and we want to maximize sig/uncbak, which reduces to
      // sig/sqrt((reluncbak*bak)^2 + (sqrt(bak))^2 + 1)
      //signif = sigtotal/TMath::Sqrt(.04*baktotal*baktotal + baktotal + 1);
      signif = RooStats::NumberCountingUtils::BinomialExpZ(sigtotal, baktotal, bkguncert); // Z_n

      cout << "cut=" << setw(20) << sighist->GetBinLowEdge(n_bins-i) 
	   << ", signif=" << setw(20) << signif
	   << ", nsig=" << setw(20) << sigtotal
	   << ", nbak=" << setw(20) << baktotal << endl;

      // baktotal>0. && 
      if((signif >= maxsignif || (capsignif && signif > 7)) && 
	 sigtotal > 5.0 && 
	 sighist->GetBinLowEdge(n_bins-i)>minimum){
	maxsignif = signif;
	maxsignifbin = n_bins-i;
	sigtotalatmaxsignif=sigtotal;
	baktotalatmaxsignif=baktotal;
      }
    }
  
    if(maxsignifbin >= 0){
      if(debug)
	cout << std::setw(20) << "Variable"
	     << std::setw(20) << "Cut"
	     << std::setw(20) << "NSignal"
	     << std::setw(20) << "NBackgr"
	     << std::setw(20) << "Significance"
	     << std::setw(20) << "epsSignal"
	     << std::setw(20) << "epsBackgr"
	     << endl;
      cout << std::setw(20) << variable
	   << std::setw(20) << sighist->GetBinLowEdge(maxsignifbin)
	   << std::setw(20) << sigtotalatmaxsignif
	   << std::setw(20) << baktotalatmaxsignif
	   << std::setw(20) << maxsignif //(maxsignif <= 10. ? maxsignif:10)
	   << std::setw(20) << sigtotalatmaxsignif/totsig
	   << std::setw(20) << baktotalatmaxsignif/totbak
	   << std::setw(20) << sigtotalatmaxsignif/preselsig
	   << std::setw(20) << baktotalatmaxsignif/preselbak
	   << endl;
    }
    else
      cout << "Could not find an optimal working point." << endl;

    delete sighist;
    delete bakhist;
    delete sigtree;
    delete baktree;
  }
  return;
}

void choose_operating_point(string signal,
			    std::vector<string> backgrounds,
			    std::vector<string> variables,
			    string treename,
			    float lumi,
			    float minimum,
			    string extracut="",
			    float kfactor=1.,
			    float bkguncert=0.20,
			    string preselectioncut="",
			    bool capsignif=true,
			    bool extrapolate=false,
			    bool debug=false){

  TFile sigfile(signal.c_str(),"RO");
  TTree* sigtree = (TTree*)sigfile.Get(treename.c_str());

  TChain* baktree = new TChain(treename.c_str());
  for(unsigned int i=0; i<backgrounds.size(); i++)
    baktree->AddFile(backgrounds[i].c_str());

  if(!sigtree || !baktree){
    cout << "Can't do anything without signal or background tree!" << endl;
    cout << "Could not find an optimal working point." << endl;
    return;
  }
  
  float totsig=0;
  float totbak=0;

  // check if there are TParameters with the total weights before skimming
  TParameter<double>* sigweights = (TParameter<double>*)sigfile.Get("TotalProcessWeights");
  if(sigweights){
    totsig = sigweights->GetVal()*lumi*kfactor;
  }

  for(unsigned int i=0; i<backgrounds.size(); i++){
    TFile bgfile(backgrounds[i].c_str(),"RO");
    TParameter<double>* bakweights = (TParameter<double>*)bgfile.Get("TotalProcessWeights");
    if(bakweights){
      totbak+=bakweights->GetVal()*lumi;
    }
  }

  cout << "Total signal weights before skimming is " << totsig << endl;

  cout << "Probing variables ";
  for(unsigned int i=0; i<variables.size(); i++)
    cout << variables[i] << "\t";
  cout << endl;

  std::vector<float> sigmin,sigmax,bakmin,bakmax,min,max;
  for(unsigned int i=0; i<variables.size(); i++){
    sigmin.push_back(sigtree->GetMinimum(variables[i].c_str()));
    bakmin.push_back(baktree->GetMinimum(variables[i].c_str()));
    float tmin = (sigmin[i] < bakmin[i] ? sigmin[i]:bakmin[i]);

    // check to see if there's a lower bound on this variable in the cuts we're applying.
    float varmin=0;

    size_t varcutpos = preselectioncut.find(Form("%s>",variables[i].c_str()));
    if(varcutpos!=string::npos){ 
      string cutstr=preselectioncut.substr(varcutpos+variables[i].size()+1,
					   preselectioncut.find("&",varcutpos+variables[i].size())-(varcutpos+variables[i].size()+1));
      sscanf(cutstr.c_str(),"%f",&varmin);
    }
    
    if(varmin>0 && varmin>tmin)
      tmin=varmin;

    varcutpos = extracut.find(Form("%s>",variables[i].c_str()));
    if(varcutpos!=string::npos){
      string cutstr=extracut.substr(varcutpos+variables[i].size()+1,
				    extracut.find("&",varcutpos+variables[i].size())-(varcutpos+variables[i].size()+1));
      sscanf(cutstr.c_str(),"%f",&varmin);
    }

    if(varmin>0 && varmin>tmin)
      tmin=varmin;
    
    min.push_back(tmin);

    // now the maximum values
    if(variables[i].find("+") != string::npos){
      int endfirst=variables[i].find("+");
      int endnext=-1;
      int smax = sigtree->GetMaximum(variables[i].substr(0,endfirst).c_str());
      int bmax = baktree->GetMaximum(variables[i].substr(0,endfirst).c_str());
      cout << "Got smax,bmax for variable " << variables[i].substr(0,endfirst) << " as " << smax << "," << bmax << endl;
      do{
	endnext = variables[i].find("+",endfirst+1);
	smax += sigtree->GetMaximum(variables[i].substr(endfirst+1,endnext).c_str());
	bmax += baktree->GetMaximum(variables[i].substr(endfirst+1,endnext).c_str());
	cout << "++ smax,bmax for variable " << variables[i].substr(endfirst+1,endnext) << " as " << smax << "," << bmax << endl;
	endfirst = (endnext != (int)string::npos) ? endnext:variables[i].length();
      } while(variables[i].find("+",endfirst+1) != string::npos);
      sigmax.push_back(smax);
      bakmax.push_back(bmax);
    }
    else{
      sigmax.push_back(sigtree->GetMaximum(variables[i].c_str()));
      bakmax.push_back(baktree->GetMaximum(variables[i].c_str()));
    }

    float tmax = (sigmax[i] > bakmax[i] ? sigmax[i]:bakmax[i]);
    if(isinf(tmax)){
      if(isfinite(sigmax[i])) tmax=sigmax[i];
      else if(isfinite(bakmax[i])) tmax=bakmax[i];
      else{
	cout << "Can't find maximum for this variable!  Setting to 10000." << endl;
	tmax = 10000;
      }
    }
    max.push_back(tmax);

    cout << "Variable " << setw(20) << variables[i] << " runs from " << min[i] << " to " << max[i] << endl;
  }
  
  const int nbins=200;

  TH1* sighist(0);
  TH1* bakhist(0);
  if(variables.size()==2){
    sighist = new TH2F("sighist","sighist",nbins,min[1],max[1],nbins,min[0],max[0]);
    bakhist = new TH2F("bakhist","bakhist",nbins,min[1],max[1],nbins,min[0],max[0]);
  }

  cout << "... drawing variables" << endl;

  string drawstr = (variables[0] == "m_eff" ? "HTjets_30_2p5+MET":variables[0]);
  for(unsigned int i=1; i<variables.size(); i++)
    drawstr += Form(":%s",(variables[i] == "m_eff" ? "HTjets_30_2p5+MET":variables[i]).c_str());

  sigtree->Draw(Form("%s>>sighist",drawstr.c_str()),"weight*(weight>0.)","goff");
  baktree->Draw(Form("%s>>bakhist",drawstr.c_str()),"weight*(weight>0.)","goff");

  float preselsig=sighist->GetSumOfWeights()*lumi*kfactor;
  float preselbak=bakhist->GetSumOfWeights()*lumi;

  if(totsig<=0) totsig=preselsig;
  if(totbak<=0) totbak=preselbak;

  if(preselectioncut!=""){
    sigtree->Draw(Form("%s>>sighist",drawstr.c_str()),Form("weight*(%s)",preselectioncut.c_str()),"goff");
    baktree->Draw(Form("%s>>bakhist",drawstr.c_str()),Form("weight*(%s)",preselectioncut.c_str()),"goff");
      
    preselsig=sighist->GetSumOfWeights()*lumi;
    preselbak=bakhist->GetSumOfWeights()*lumi;
  }
    
  if(extracut=="")
    extracut="weight>0.";

  if(preselectioncut!=""){
    extracut+="&&";
    extracut+=preselectioncut;
  }

  sigtree->Draw(Form("%s>>sighist",drawstr.c_str()),
		Form("weight*(%s)",extracut.c_str()),
		"goff");
  baktree->Draw(Form("%s>>bakhist",drawstr.c_str()),
		Form("weight*(%s)",extracut.c_str()),
		"goff");

  cout << "... drew histograms for " << drawstr << " with cut " << Form("weight*(%s)",extracut.c_str()) << endl;

  sighist->Scale(lumi*kfactor);
  bakhist->Scale(lumi);

  cout << sighist->GetSumOfWeights() << endl;
  cout << sighist->GetEntries() << endl;
  
  float maxsignif=-999999;
  float signif=0;
  float sigtotal=0;
  float baktotal=0;
  float sigtotalatmaxsignif=0;
  float baktotalatmaxsignif=0;
  float* cutval = new float[variables.size()];

  if(!extrapolate){
    std::vector<int> maxsignifbin(nbins);

    double sig_entries[nbins][nbins];
    double bak_entries[nbins][nbins];
    double sig_sums[nbins][nbins];
    double bak_sums[nbins][nbins];
    double signifs[nbins][nbins];
    for(int i=0; i<nbins; i++){
      for(int j=0; j<nbins; j++){
	sig_entries[i][j] = ((TH2F*)sighist)->GetBinContent(i,j);
	bak_entries[i][j] = ((TH2F*)bakhist)->GetBinContent(i,j);

	sig_sums[i][j] = 0;
	bak_sums[i][j] = 0;
      }
    }
  
    for(int i=nbins-1; i>=0; i--){
      for(int j=nbins-1; j>=0; j--){
	sig_sums[i][j] = sig_entries[i][j];
	bak_sums[i][j] = bak_entries[i][j];
	if(i!=nbins-1 && j!=nbins-1){
	  sig_sums[i][j] += sig_sums[i+1][j+1];
	  bak_sums[i][j] += bak_sums[i+1][j+1];
	}
	if(j!=nbins-1){
	  for(int jj=j+1; jj<nbins; jj++){
	    sig_sums[i][j] += sig_entries[i][jj];
	    bak_sums[i][j] += bak_entries[i][jj];
	  }
	}
	if(i!=nbins-1){
	  for(int ii=i+1; ii<nbins; ii++){
	    sig_sums[i][j] += sig_entries[ii][j];
	    bak_sums[i][j] += bak_entries[ii][j];
	  }
	}

	signif = RooStats::NumberCountingUtils::BinomialExpZ(sig_sums[i][j], bak_sums[i][j], bkguncert);
	if(isinf(signif)) signif=0;
	signifs[i][j] = signif;
      
	sigtotal = sig_sums[i][j];
	baktotal = bak_sums[i][j];

	if((signif >= maxsignif || (capsignif && signif > 7)) && 
	   sigtotal > 5.0){
	  maxsignif = signif;
	  maxsignifbin[0] = j;
	  maxsignifbin[1] = i;
	  sigtotalatmaxsignif=sigtotal;
	  baktotalatmaxsignif=baktotal;
	}
      }
    }

    int binspace=1;
    if(nbins > 20)
      binspace=(int)(nbins/20);

    {
      cout << "Signal histogram" << endl;
      for(int i=nbins-1; i>=0; i-=binspace){
	cout << setw(10) << min[1]+i*(max[1]-min[1])/nbins;
	for(int j=0; j<nbins; j+=binspace){
	  cout << std::setw(10) << (int)sig_entries[i][j];
	}
	cout << endl;
      }
      cout << setw(10) << "";
      for(int j=0; j<nbins; j+=binspace){
	cout << std::setw(10) << min[0]+j*(max[0]-min[0])/nbins;
      }
      cout << endl << endl;
    }

    {
      cout << "Signal sums" << endl;
      for(int i=nbins-1; i>=0; i-=binspace){
	cout << setw(10) << min[1]+i*(max[1]-min[1])/nbins;
	for(int j=0; j<nbins; j+=binspace){
	  cout << std::setw(10) << (int)sig_sums[i][j];
	}
	cout << endl;
      }
      cout << setw(10) << "";
      for(int j=0; j<nbins; j+=binspace){
	cout << std::setw(10) << min[0]+j*(max[0]-min[0])/nbins;
      }
      cout << endl << endl;
    }

    {
      cout << "Background histogram" << endl;
      for(int i=nbins-1; i>=0; i-=binspace){
	cout << setw(10) << min[1]+i*(max[1]-min[1])/nbins;
	for(int j=0; j<nbins; j+=binspace){
	  cout << std::setw(10) << (int)bak_entries[i][j];
	}
	cout << endl;
      }
      cout << setw(10) << "";
      for(int j=0; j<nbins; j+=binspace){
	cout << std::setw(10) << min[0]+j*(max[0]-min[0])/nbins;
      }
      cout << endl << endl;
    }

    {
      cout << "Background sums" << endl;
      for(int i=nbins-1; i>=0; i-=binspace){
	cout << setw(10) << min[1]+i*(max[1]-min[1])/nbins;
	for(int j=0; j<nbins; j+=binspace){
	  cout << std::setw(10) << (int)bak_sums[i][j];
	}
	cout << endl;
      }
      cout << setw(10) << "";
      for(int j=0; j<nbins; j+=binspace){
	cout << std::setw(10) << min[0]+j*(max[0]-min[0])/nbins;
      }
      cout << endl << endl;
    }

    {
      cout << "Significances" << endl;
      for(int i=nbins-1; i>=0; i-=binspace){
	cout << setw(10) << min[1]+i*(max[1]-min[1])/nbins;
	for(int j=0; j<nbins; j+=binspace){
	  cout << std::setw(10) << signifs[i][j];
	}
	cout << endl;
      }
      cout << setw(10) << "";
      for(int j=0; j<nbins; j+=binspace){
	cout << std::setw(10) << min[0]+j*(max[0]-min[0])/nbins;
      }
      cout << endl << endl;
    }

    cutval[0] = min[0]+maxsignifbin[0]*((max[0]-min[0])/nbins);
    cutval[1] = min[1]+maxsignifbin[1]*((max[1]-min[1])/nbins);
  }
  else{
    // fit the signal and background histograms to some form.
    
  }

  if(maxsignif >= 0){
    if(debug){
      cout << std::setw(20) << "Variable"
	   << std::setw(20) << "Cut";
      cout << std::setw(20) << "NSignal"
	   << std::setw(20) << "NBackgr"
	   << std::setw(20) << "Significance"
	   << std::setw(20) << "epsSignal"
	   << std::setw(20) << "epsBackgr"
	   << std::setw(20) << "epsSignal"
	   << std::setw(20) << "epsBackgr";
      for(unsigned int i=1; i<variables.size(); i++)
	cout << std::setw(30) << "Variable"
	     << std::setw(20) << "Cut";
      cout << endl;
    }
    cout << std::setw(20) << variables[0]
	 << std::setw(20) << cutval[0];
    cout << std::setw(20) << sigtotalatmaxsignif
	 << std::setw(20) << baktotalatmaxsignif
	 << std::setw(20) << maxsignif
	 << std::setw(20) << sigtotalatmaxsignif/totsig
	 << std::setw(20) << baktotalatmaxsignif/totbak
	 << std::setw(20) << preselsig/totsig
	 << std::setw(20) << preselbak/totbak;
    for(unsigned int i=1; i<variables.size(); i++)
      cout << std::setw(30) << variables[i]
	   << std::setw(20) << cutval[i];
    cout << std::setw(20) << sigtotalatmaxsignif/preselsig
	 << std::setw(20) << baktotalatmaxsignif/preselbak;
    cout << endl;
  }
  else
    cout << "Could not find an optimal working point." << endl;
  
  delete sighist;
  delete bakhist;
  delete sigtree;
  delete baktree;
  delete cutval;
  
  return;
}

int main(int argc, char** argv){
  
  string signal="";
  string variable="";
  std::vector<string> variables;
  std::vector<string> backgrounds;
  string treename="OutputTree";
  bool debug=false;

  float minimum=-999999;

  float lumi=1.;

  string extracut="";
  string preselectioncut="";

  float kfactor=1.;
  float bkguncert=0.30;

  bool capsignif=false;

  bool extrapolate=false;

  opterr = 0;
  int c;
  while ((c = getopt (argc, argv, "s:b:v:t:l:dm:c:k:p:u:Ce")) != -1)
    switch (c){
    case 's':
      signal=optarg;
      break;
    case 'b':
      backgrounds.push_back(optarg);
      break;
    case 'v':
      variables.push_back(optarg);
      break;
    case 't':
      treename=optarg;
      break;
    case 'l':
      sscanf(optarg,"%f",&lumi);
      break;
    case 'd':
      debug=true;
      break;
    case 'm':
      sscanf(optarg,"%f",&minimum);
      break;
    case 'p':
      preselectioncut=optarg;
      break;
    case 'c':
      extracut=optarg;
      break;
    case 'k':
      sscanf(optarg,"%f",&kfactor);
      break;
    case 'u':
      sscanf(optarg,"%f",&bkguncert);
      break;
    case 'C':
      capsignif=true;
      break;
    case 'e':
      extrapolate=true;
      break;
    default:
      return 0;
    }

  if(variables.size()==1){
    variable=variables[0];
    choose_operating_point(signal,
			   backgrounds,
			   variable,
			   treename,
			   lumi,
			   minimum,
			   extracut,
			   kfactor,	
			   bkguncert,
			   preselectioncut,
			   capsignif,
			   extrapolate,
			   debug);
  }
  else if(variables.size()==2){
    choose_operating_point(signal,
			   backgrounds,
			   variables,
			   treename,
			   lumi,
			   minimum,
			   extracut,
			   kfactor,	
			   bkguncert,
			   preselectioncut,
			   capsignif,
			   extrapolate,
			   debug);
  }
  return 0;
}
