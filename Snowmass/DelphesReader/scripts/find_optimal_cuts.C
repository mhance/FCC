#include <iostream>
#include <fstream>
#include <string>
#include <map>

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

#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

#include "make_event_tables.C"

using namespace std;

void myText(Double_t x,Double_t y,Color_t color,const char *text, int font=-1, double tsize=.05) {

  TLatex* l = new TLatex; //l.SetTextAlign(12); 
  l->SetTextSize(tsize);
  l->SetNDC(kTRUE);
  l->SetTextColor(color);
  if(font>0) l->SetTextFont(font);
  l->DrawLatex(x,y,text);
}

std::pair<float,float> optimize_soverrootb(float signal,
					   float background,
					   std::vector<float> effS,
					   std::vector<float> effB){
  
  int bestpoint=-1;
  float bestsig=0;
  float sig=0;
  for(unsigned int i=0; i<effS.size(); i++){
    if(effS[i]>0){
      sig = signal*effS[i]/TMath::Sqrt(effB[i]*background+effS[i]*signal);
      if(sig > bestsig || bestpoint < 0){
	bestpoint = i;
	bestsig = sig;
      }
    }
  }
  return make_pair(bestpoint,bestsig);
}

std::pair<std::pair<float,float>,float> optimize_point(float signal,
						       float background,
						       string weightsfile,
						       string resultsfile,
						       std::vector<double>& cutsMin,
						       std::vector<double>& cutsMax){
  
  // get the plot of effB vs effS
  TFile f(resultsfile.c_str(),"RO");
  TH1F* h_effB = (TH1F*)f.Get("Method_Cuts/Cuts/MVA_Cuts_effBvsSLocal");

  std::vector<float> effS;
  std::vector<float> effB;
  for(int i=1; i<=h_effB->GetNbinsX(); i++){
    effS.push_back( h_effB->GetXaxis()->GetBinLowEdge(i) );
    effB.push_back( h_effB->GetBinContent(i) );
  }

  // call the function
  std::pair<int,float> optimal_point = optimize_soverrootb(signal, background, effS, effB);

  cout << "Signal=" << signal << ", background=" << background 
       << ", weightsfile=" << weightsfile
       << ", optimal point=" << optimal_point.first << ", optimal significance=" << optimal_point.second
       << ", optimal effic=" << effS[optimal_point.first] << ", optimal reject=" << effB[optimal_point.first]
       << endl;
  
  TMVA::Reader reader("Silent");

  float MET,HTjets,HTleptons,m_eff,pt_l0,pt_l1,pt_l2,mT_lllmet,Z_M,Z_pt,W_mT;
  reader.AddVariable("MET"      , &MET       );
//   reader.AddVariable("HTjets"   , &HTjets    );
  reader.AddVariable("HTleptons", &HTleptons );
//   reader.AddVariable("m_eff"    , &m_eff     );
 		 			
//   reader.AddVariable("pt_l0"    , &pt_l0     );
//   reader.AddVariable("pt_l1"    , &pt_l1     );
//   reader.AddVariable("pt_l2"    , &pt_l2     );
		     		 		 			
  reader.AddVariable("mT_lllmet", &mT_lllmet );
//   reader.AddVariable("Z_M"      , &Z_M       );
  reader.AddVariable("Z_pt"     , &Z_pt      );
  reader.AddVariable("W_mT"     , &W_mT      );

  float weight;
  reader.AddSpectator("weight"  , &weight    );
  
  TMVA::IMethod* meth = reader.BookMVA("Cuts",weightsfile.c_str());
  TMVA::MethodCuts* cuts = reader.FindCutsMVA("Cuts");
  cuts->GetCuts(effS[optimal_point.first],cutsMin,cutsMax);

  if(meth) delete meth;
  return make_pair(make_pair(effS[optimal_point.first],
			     effB[optimal_point.first])
		   ,optimal_point.second);
}

void optimize(string grid,
	      string configfile){


  TFile f_output(Form("%s/optimal_cuts.root",grid.c_str()),"RECREATE");
  TTree* cuts_output = new TTree("cuts","cuts");
  float mC1,mN1,nSig,nBkg,effS,effB,significance;
  float low_MET,low_HTjets,low_HTleptons,low_m_eff,low_pt_l0,low_pt_l1,low_pt_l2,low_mT_lllmet,low_Z_M,low_Z_pt,low_W_mT;
  float hgh_MET,hgh_HTjets,hgh_HTleptons,hgh_m_eff,hgh_pt_l0,hgh_pt_l1,hgh_pt_l2,hgh_mT_lllmet,hgh_Z_M,hgh_Z_pt,hgh_W_mT;
  cuts_output->Branch("mC1", &mC1, 'F');
  cuts_output->Branch("mN1", &mN1, 'F');
  cuts_output->Branch("nSig", &nSig, 'F');
  cuts_output->Branch("nBkg", &nBkg, 'F');
  cuts_output->Branch("effS", &effS, 'F');
  cuts_output->Branch("effB", &effB, 'F');
  cuts_output->Branch("significance", &significance, 'F');
  cuts_output->Branch("low_MET", &low_MET, 'F');
  cuts_output->Branch("low_HTjets", &low_HTjets, 'F');
  cuts_output->Branch("low_HTleptons", &low_HTleptons, 'F');
  cuts_output->Branch("low_m_eff", &low_m_eff, 'F');
  cuts_output->Branch("low_pt_l0", &low_pt_l0, 'F');
  cuts_output->Branch("low_pt_l1", &low_pt_l1, 'F');
  cuts_output->Branch("low_pt_l2", &low_pt_l2, 'F');
  cuts_output->Branch("low_mT_lllmet", &low_mT_lllmet, 'F');
  cuts_output->Branch("low_Z_M", &low_Z_M, 'F');
  cuts_output->Branch("low_Z_pt", &low_Z_pt, 'F');
  cuts_output->Branch("low_W_mT", &low_W_mT, 'F');
  cuts_output->Branch("hgh_MET", &hgh_MET, 'F');
  cuts_output->Branch("hgh_HTjets", &hgh_HTjets, 'F');
  cuts_output->Branch("hgh_HTleptons", &hgh_HTleptons, 'F');
  cuts_output->Branch("hgh_m_eff", &hgh_m_eff, 'F');
  cuts_output->Branch("hgh_pt_l0", &hgh_pt_l0, 'F');
  cuts_output->Branch("hgh_pt_l1", &hgh_pt_l1, 'F');
  cuts_output->Branch("hgh_pt_l2", &hgh_pt_l2, 'F');
  cuts_output->Branch("hgh_mT_lllmet", &hgh_mT_lllmet, 'F');
  cuts_output->Branch("hgh_Z_M", &hgh_Z_M, 'F');
  cuts_output->Branch("hgh_Z_pt", &hgh_Z_pt, 'F');
  cuts_output->Branch("hgh_W_mT", &hgh_W_mT, 'F');

  string line;
  char label[100];

  WrapGrid_DG("preselection/MET","h_preselection_MET",""   ,"nocut");
  string sigeventsfilename = Form("%s/events_cut_nocut.txt",grid.c_str());
  string bkgeventsfilename = Form("%s/backgrounds_cut_nocut.txt",grid.c_str());

  // read in the number of signal and background events for a basic set of cuts
  std::map<string,float> signal_events;
  ifstream sigeventsfile(sigeventsfilename.c_str());
  float sigevents;
  if(sigeventsfile.is_open()){
    while(sigeventsfile.good()){
      getline(sigeventsfile,line);
      sscanf(line.c_str(),"%s %f",label,&sigevents);
      signal_events[label] = sigevents;
    }
  }
  float backgr_events=0;
  float bkgevents;
  ifstream bkgeventsfile(bkgeventsfilename.c_str());
  if(bkgeventsfile.is_open()){
    while(bkgeventsfile.good()){
      getline(bkgeventsfile,line);
      sscanf(line.c_str(),"%s %f",label,&bkgevents);
      backgr_events += bkgevents;
    }
  }
  

  // read in the configuration file so we know what the grid looks like
  std::map<string,float> xaxis,yaxis,sigma;
  int mx,my,nev;
  float cs;
  char dummy1[100],dummy2[100];
  ifstream conffile (configfile.c_str());
  if (conffile.is_open()){
    while(conffile.good()){
      getline(conffile,line);
      if(line.find("#") != string::npos) continue;
      //cout << line << endl;
      sscanf(line.c_str(),
	     "%d %d %s %s %f %d %s",
	     &mx,&my,dummy1,dummy2,&cs,&nev,label);
      xaxis[(string)label] = mx;
      yaxis[(string)label] = my;
      sigma[(string)label] = cs;
    }
  }
  else{
    cout << "Couldn't find configuration file for this grid!  Aborting." << endl;
    return;
  }

  // now, for each point, get all the information
  std::vector<double> cutsMin,cutsMax;
  for(std::map<string,float>::iterator m_iter = xaxis.begin();
      m_iter != xaxis.end(); m_iter++){
    string point = m_iter->first;
    string weightsfile = Form("weights/MVA_%s_Cuts.weights.xml",point.c_str());
    string resultsfile = Form("%s/tmva.%s.%s.root",grid.c_str(),grid.c_str(),point.c_str());

    if(signal_events[point] > 0.){
      std::pair<std::pair<float,float>,float> results = optimize_point(signal_events[point],
								       backgr_events,
								       weightsfile,
								       resultsfile,
								       cutsMin,
								       cutsMax);
      effS = results.first.first;
      effB = results.first.second;
      significance = results.second;
      mC1 = xaxis[point];
      mN1 = yaxis[point];
      nSig = signal_events[point];
      nBkg = backgr_events;
      low_MET       = cutsMin[0];      hgh_MET       = cutsMax[0];
      //low_HTjets    = cutsMin[1];      hgh_HTjets    = cutsMax[1];
      low_HTleptons = cutsMin[1];      hgh_HTleptons = cutsMax[1];
      //low_m_eff     = cutsMin[3];      hgh_m_eff     = cutsMax[3];
					 					   	   
      //low_pt_l0     = cutsMin[4];      hgh_pt_l0     = cutsMax[4];
      //low_pt_l1     = cutsMin[5];      hgh_pt_l1     = cutsMax[5];
      //low_pt_l2     = cutsMin[6];      hgh_pt_l2     = cutsMax[6];
					 					   	   
      low_mT_lllmet = cutsMin[2];      hgh_mT_lllmet = cutsMax[2];
      //low_Z_M       = cutsMin[8];      hgh_Z_M       = cutsMax[8];
      low_Z_pt      = cutsMin[3];      hgh_Z_pt      = cutsMax[3];
      low_W_mT      = cutsMin[4];     hgh_W_mT      = cutsMax[4];

      cuts_output->Fill();
    }
    else{
      cerr << "Only have " << signal_events[point] << " for this point." << endl;
    }
  }

  f_output.cd();
  cuts_output->Write();
  f_output.Close();

}
