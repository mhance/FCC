#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TAxis.h"

using namespace std;


float GetEvents(TFile* f,
		string cut=""){
  if(!f){
    std::cerr << "Can't find ROOT file!" << std::endl;
    return 0;
  }

  if(cut=="")
    cut="weight>0.";
  TTree* tree = (TTree*)f->Get("OutputTree");
  TH1F hist("h","h",1,-100000000,100000000);
  float entries = tree->Draw("MET>>h",Form("weight*(%s)",cut.c_str()),"goff");
  float totalevents = hist.GetSumOfWeights();
  
  delete tree;
  return totalevents;
}

void PrintEvents(std::vector<TFile*> files,
		  std::vector<string> labels,
		  std::string output,
		  string cut="",
		  bool debug=false){
  
  std::ofstream outfile(output.c_str());
  for(unsigned int i=0; i<files.size(); i++){
    outfile << std::setw(30) << labels[i] << "\t\t" << std::setw(10) << GetEvents(files[i],cut) << std::endl;
    if(debug)
      cout << std::setw(30) << labels[i] << "\t\t" << std::setw(10) << GetEvents(files[i],cut) << std::endl;
  }
  outfile.close();
  return;
}


void PrintEvents(std::vector<string> files,
		 std::vector<string> labels,
		 std::string output,
		 string cut="",
		 bool debug=false){

  std::vector<TFile*> rootfiles;
  for(unsigned int i=0; i<files.size(); i++){
    rootfiles.push_back(new TFile(files[i].c_str(),"RO"));
  }
  PrintEvents(rootfiles,labels,output,cut,debug);
  for(unsigned int i=0; i<rootfiles.size(); i++){
    rootfiles[i]->Close();
  }
  return;
}


float GetAcceptance(TFile* f,
		    string histname,
		    string directory="",
		    float cutval=0.){
  if(!f){
    std::cerr << "Can't find ROOT file!" << std::endl;
    return 0;
  }

  if(directory != (string)""){
    char lastchar = *directory.rbegin();
    if(lastchar != '/')
      directory += "/";
  }

  TH1F* hist = (TH1F*)f->Get(Form("%s%s",directory.c_str(),histname.c_str()));
  if(!hist){
    std::cerr << "Can't find hist " << Form("%s%s",directory.c_str(),histname.c_str()) << std::endl;
    return 0;
  }

  float totalevents=0;
  if(cutval > 0){
    for(int i=0; i<hist->GetNbinsX(); i++){
      if(hist->GetXaxis()->GetBinLowEdge(i) >= cutval){
	totalevents += hist->GetBinContent(i);
      }
    }
  }
  else{
    totalevents = hist->GetSumOfWeights();
  }
  
  TH1F* prehist = (TH1F*)f->Get("noselection/MET/h_noselection_MET");
  if(!prehist){
    cout << "error!" << endl;
    return 0;
  }

  return totalevents/prehist->GetSumOfWeights();
}

void PrintAcceptance(std::vector<TFile*> files,
		     std::vector<string> labels,
		     std::string output,
		     string histname,
		     string directory="",
		     float cutval=0.,
		     float scale=1.,
		     bool debug=false){
  
  std::ofstream outfile(output.c_str());
  for(unsigned int i=0; i<files.size(); i++){
    outfile << std::setw(30) << labels[i] << "\t\t" << std::setw(10) << scale*GetAcceptance(files[i],histname,directory,cutval) << std::endl;
    if(debug)
      cout << std::setw(30) << labels[i] << "\t\t" << std::setw(10) << scale*GetAcceptance(files[i],histname,directory,cutval) << std::endl;
  }
  outfile.close();
  return;
}


void PrintAcceptance(std::vector<string> files,
		     std::vector<string> labels,
		     std::string output,
		     string histname,
		     string directory="",
		     float cutval=0.,
		     float scale=1.,
		     bool debug=false){
  
  std::vector<TFile*> rootfiles;
  for(unsigned int i=0; i<files.size(); i++){
    rootfiles.push_back(new TFile(files[i].c_str(),"RO"));
  }
  PrintAcceptance(rootfiles,labels,output,histname,directory,cutval,scale,debug);
  for(unsigned int i=0; i<rootfiles.size(); i++){
    rootfiles[i]->Close();
  }
  return;
}

void WrapGrid(string dir="noselection/m_eff",
	      string hist="h_noselection_m_eff",
	      string cut="m_eff>1000",
	      string cuttag="meff_gt_1000"){
  std::vector<string> files;
  std::vector<string> labels;
  
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00500.qq.N00001.root");  labels.push_back("GOGO_decoupledSQ.GO00500.qq.N00001");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00500.qq.N00200.root");  labels.push_back("GOGO_decoupledSQ.GO00500.qq.N00200");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00500.qq.N00300.root");  labels.push_back("GOGO_decoupledSQ.GO00500.qq.N00300");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00500.qq.N00350.root");  labels.push_back("GOGO_decoupledSQ.GO00500.qq.N00350");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00500.qq.N00400.root");  labels.push_back("GOGO_decoupledSQ.GO00500.qq.N00400");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00500.qq.N00450.root");  labels.push_back("GOGO_decoupledSQ.GO00500.qq.N00450");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00629.qq.N00001.root");  labels.push_back("GOGO_decoupledSQ.GO00629.qq.N00001");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00629.qq.N00252.root");  labels.push_back("GOGO_decoupledSQ.GO00629.qq.N00252");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00629.qq.N00377.root");  labels.push_back("GOGO_decoupledSQ.GO00629.qq.N00377");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00629.qq.N00440.root");  labels.push_back("GOGO_decoupledSQ.GO00629.qq.N00440");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00629.qq.N00503.root");  labels.push_back("GOGO_decoupledSQ.GO00629.qq.N00503");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00629.qq.N00566.root");  labels.push_back("GOGO_decoupledSQ.GO00629.qq.N00566");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00792.qq.N00001.root");  labels.push_back("GOGO_decoupledSQ.GO00792.qq.N00001");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00792.qq.N00317.root");  labels.push_back("GOGO_decoupledSQ.GO00792.qq.N00317");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00792.qq.N00475.root");  labels.push_back("GOGO_decoupledSQ.GO00792.qq.N00475");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00792.qq.N00554.root");  labels.push_back("GOGO_decoupledSQ.GO00792.qq.N00554");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00792.qq.N00634.root");  labels.push_back("GOGO_decoupledSQ.GO00792.qq.N00634");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00792.qq.N00713.root");  labels.push_back("GOGO_decoupledSQ.GO00792.qq.N00713");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00997.qq.N00001.root");  labels.push_back("GOGO_decoupledSQ.GO00997.qq.N00001");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00997.qq.N00399.root");  labels.push_back("GOGO_decoupledSQ.GO00997.qq.N00399");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00997.qq.N00598.root");  labels.push_back("GOGO_decoupledSQ.GO00997.qq.N00598");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00997.qq.N00698.root");  labels.push_back("GOGO_decoupledSQ.GO00997.qq.N00698");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00997.qq.N00798.root");  labels.push_back("GOGO_decoupledSQ.GO00997.qq.N00798");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO00997.qq.N00897.root");  labels.push_back("GOGO_decoupledSQ.GO00997.qq.N00897");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01255.qq.N00001.root");  labels.push_back("GOGO_decoupledSQ.GO01255.qq.N00001");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01255.qq.N00502.root");  labels.push_back("GOGO_decoupledSQ.GO01255.qq.N00502");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01255.qq.N00753.root");  labels.push_back("GOGO_decoupledSQ.GO01255.qq.N00753");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01255.qq.N00879.root");  labels.push_back("GOGO_decoupledSQ.GO01255.qq.N00879");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01255.qq.N01004.root");  labels.push_back("GOGO_decoupledSQ.GO01255.qq.N01004");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01255.qq.N01130.root");  labels.push_back("GOGO_decoupledSQ.GO01255.qq.N01130");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01580.qq.N00001.root");  labels.push_back("GOGO_decoupledSQ.GO01580.qq.N00001");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01580.qq.N00632.root");  labels.push_back("GOGO_decoupledSQ.GO01580.qq.N00632");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01580.qq.N00948.root");  labels.push_back("GOGO_decoupledSQ.GO01580.qq.N00948");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01580.qq.N01106.root");  labels.push_back("GOGO_decoupledSQ.GO01580.qq.N01106");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01580.qq.N01264.root");  labels.push_back("GOGO_decoupledSQ.GO01580.qq.N01264");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01580.qq.N01422.root");  labels.push_back("GOGO_decoupledSQ.GO01580.qq.N01422");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01989.qq.N00001.root");  labels.push_back("GOGO_decoupledSQ.GO01989.qq.N00001");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01989.qq.N00796.root");  labels.push_back("GOGO_decoupledSQ.GO01989.qq.N00796");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01989.qq.N01193.root");  labels.push_back("GOGO_decoupledSQ.GO01989.qq.N01193");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01989.qq.N01392.root");  labels.push_back("GOGO_decoupledSQ.GO01989.qq.N01392");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01989.qq.N01591.root");  labels.push_back("GOGO_decoupledSQ.GO01989.qq.N01591");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO01989.qq.N01790.root");  labels.push_back("GOGO_decoupledSQ.GO01989.qq.N01790");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO02504.qq.N00001.root");  labels.push_back("GOGO_decoupledSQ.GO02504.qq.N00001");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO02504.qq.N01002.root");  labels.push_back("GOGO_decoupledSQ.GO02504.qq.N01002");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO02504.qq.N01502.root");  labels.push_back("GOGO_decoupledSQ.GO02504.qq.N01502");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO02504.qq.N01753.root");  labels.push_back("GOGO_decoupledSQ.GO02504.qq.N01753");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO02504.qq.N02003.root");  labels.push_back("GOGO_decoupledSQ.GO02504.qq.N02003");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO02504.qq.N02254.root");  labels.push_back("GOGO_decoupledSQ.GO02504.qq.N02254");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO03152.qq.N00001.root");  labels.push_back("GOGO_decoupledSQ.GO03152.qq.N00001");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO03152.qq.N01261.root");  labels.push_back("GOGO_decoupledSQ.GO03152.qq.N01261");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO03152.qq.N01891.root");  labels.push_back("GOGO_decoupledSQ.GO03152.qq.N01891");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO03152.qq.N02206.root");  labels.push_back("GOGO_decoupledSQ.GO03152.qq.N02206");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO03152.qq.N02522.root");  labels.push_back("GOGO_decoupledSQ.GO03152.qq.N02522");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO03152.qq.N02837.root");  labels.push_back("GOGO_decoupledSQ.GO03152.qq.N02837");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO03968.qq.N00001.root");  labels.push_back("GOGO_decoupledSQ.GO03968.qq.N00001");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO03968.qq.N01587.root");  labels.push_back("GOGO_decoupledSQ.GO03968.qq.N01587");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO03968.qq.N02381.root");  labels.push_back("GOGO_decoupledSQ.GO03968.qq.N02381");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO03968.qq.N02778.root");  labels.push_back("GOGO_decoupledSQ.GO03968.qq.N02778");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO03968.qq.N03174.root");  labels.push_back("GOGO_decoupledSQ.GO03968.qq.N03174");
  files.push_back("GO_decoupledSQ_13TeV/delphes.GO_decoupledSQ_13TeV.GOGO_decoupledSQ.GO03968.qq.N03571.root");  labels.push_back("GOGO_decoupledSQ.GO03968.qq.N03571");

  /*
  PrintEvents(files, labels, Form("GO_decoupledSQ_13TeV/events_cut_%s.txt",cuttag.c_str()),
	      hist, dir, cut, true);
  */
  /*
  PrintAcceptance(files, labels, Form("GO_decoupledSQ_13TeV/acceptance_cut%d.txt",(int)cut),
		  hist, dir, cut, 1., true);
  */
  return;
}


void WrapGrid_DG(string grid,
		 std::vector<int>& run_nos,
		 string cut="",
		 string cuttag=""){
  std::vector<string> files;
  std::vector<string> labels;
  
  for(unsigned int i=0; i<run_nos.size(); i++){
    files.push_back(Form("%s/delphes.%s.%s.%d.root",grid.c_str(),grid.c_str(),grid.c_str(),run_nos[i]));  
    labels.push_back(Form("%s.%d",grid.c_str(),run_nos[i]));
  }

  PrintEvents(files, labels, Form("%s/events_cut_%s.txt",grid.c_str(),cuttag.c_str()),
	      cut, true);
  /*
  PrintAcceptance(files, labels, Form("%s/acceptance_cut_%s.txt",cuttag),
		  hist, dir, cut, 1., true);
  */

  files.clear();
  labels.clear();
  files.push_back("delphes.processed.157953.root");                                              
  labels.push_back("WZ");
  PrintEvents(files, labels, Form("%s/backgrounds_cut_%s.txt",grid.c_str(),cuttag.c_str()),
	      cut, true);

  return;
}


void WrapGrid_DG_All(string grid, int run_start=164274, int run_end=164324){
  std::vector<int> run_nos;
  for(int i=run_start; i<run_end; i++)
    run_nos.push_back(i);

  WrapGrid_DG(grid,run_nos,"MET>0"   ,"MET_gt_0000");
  WrapGrid_DG(grid,run_nos,"MET>100" ,"MET_gt_0100");
  WrapGrid_DG(grid,run_nos,"MET>200" ,"MET_gt_0200");
  WrapGrid_DG(grid,run_nos,"MET>300" ,"MET_gt_0300");
  WrapGrid_DG(grid,run_nos,"MET>400" ,"MET_gt_0400");
  WrapGrid_DG(grid,run_nos,"MET>500" ,"MET_gt_0500");
  WrapGrid_DG(grid,run_nos,"MET>600" ,"MET_gt_0600");
  WrapGrid_DG(grid,run_nos,"MET>700" ,"MET_gt_0700");
  WrapGrid_DG(grid,run_nos,"MET>800" ,"MET_gt_0800");
  WrapGrid_DG(grid,run_nos,"MET>900" ,"MET_gt_0900");
  WrapGrid_DG(grid,run_nos,"MET>1000","MET_gt_1000");

  WrapGrid_DG(grid,run_nos,"Z_M<75", "optimal_nomet_50");
  WrapGrid_DG(grid,run_nos,"MET>100 && Z_M<75", "optimal_50");
  WrapGrid_DG(grid,run_nos,"MET>100 && MET>(50+100./4.) && HTleptons>(50+100./3.)", "optimal_100");
  WrapGrid_DG(grid,run_nos,"MET>100 && MET>(50+150./4.) && HTleptons>(50+150./3.)", "optimal_150");

  WrapGrid_DG(grid,run_nos,"MET>100 && MET>(50+200./4.) && HTleptons>(50+200./3.)", "optimal_200");

  WrapGrid_DG(grid,run_nos,"MET>100 && MET>(50+250./4.) && HTleptons>(50+250./3.)", "optimal_250");
  WrapGrid_DG(grid,run_nos,"MET>100 && MET>(50+300./4.) && HTleptons>(50+300./3.)", "optimal_300");
  WrapGrid_DG(grid,run_nos,"MET>100 && MET>(50+350./4.) && HTleptons>(50+350./3.)", "optimal_350");
  WrapGrid_DG(grid,run_nos,"MET>100 && MET>(50+400./4.) && HTleptons>(50+400./3.)", "optimal_400");
  WrapGrid_DG(grid,run_nos,"MET>100 && MET>(50+450./4.) && HTleptons>(50+450./3.)", "optimal_450");
  WrapGrid_DG(grid,run_nos,"MET>100 && MET>(50+500./4.) && HTleptons>(50+500./3.)", "optimal_500");
}
