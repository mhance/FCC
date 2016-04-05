#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

#include "TMVA/Reader.h"

using namespace std;

void classify(string sample,
	      string weights,
	      string output,
	      string treename,
	      string methodname,
	      std::vector<string>& vars,
	      std::vector<string>& spectators){

  TMVA::Reader* reader = new TMVA::Reader( "" );

  TFile* infile(0);
  
  TFile* outfile(0);
  TTree* t(0);
  TTree* t2(0);
  if(output != "" && output != sample){
    infile  = new TFile(sample.c_str(),"RO");
    t = (TTree*)infile->Get(treename.c_str());
    outfile = new TFile(output.c_str(),"RECREATE");
    outfile->cd();
    t2 = t->CloneTree();
  }
  else{
    infile = new TFile(sample.c_str(),"UPDATE");
    t = (TTree*)infile->Get(treename.c_str());
    outfile = infile;
    t2 = t;
  }
  
  float score=0;
  TBranch* branch = t2->Branch(Form("%sScore", methodname.c_str()), &score);
  
  // spectators are all integers, fortunately
  std::map<string,int> varmapint;
  for(unsigned int i=0; i<spectators.size(); i++){
    varmapint[spectators[i]]=0;
    reader->AddSpectator(spectators[i].c_str(),&varmapint[vars[i]]);
    t2->SetBranchAddress(spectators[i].c_str(), &varmapint[vars[i]]);
  }

  float weight;
  reader->AddSpectator("weight", &weight);

  // real vars are all floats
  std::map<string,float> varmap;
  for(unsigned int i=0; i<vars.size(); i++){
    varmap[vars[i]]=0;
    reader->AddVariable(vars[i].c_str(), &varmap[vars[i]]);
    t2->SetBranchAddress(vars[i].c_str(), &varmap[vars[i]]);
  }

  reader->BookMVA(methodname.c_str(),weights.c_str());

  int entries = t->GetEntries();
  for(int i=0; i<entries; i++){
    t2->GetEntry(i);
    score = reader->EvaluateMVA(methodname.c_str());
    branch->Fill();
  }
  
  t2->Write();
  outfile->Close();

  if(output != "")
    infile->Close();
}

int main(int argc, char** argv){
  
  string sample,weights,output;
  string treename="OutputTree";
  string methodname="BDT";

  std::vector<string> vars,spectators;
  
  opterr = 0;
  int c;
  while ((c = getopt (argc, argv, "s:w:o:t:m:v:p:")) != -1)
    switch (c){
    case 's':
      sample=optarg;
      break;
    case 'w':
      weights=optarg;
      break;
    case 'o':
      output=optarg;
      break;
    case 't':
      treename=optarg;
      break;
    case 'm':
      methodname=optarg;
      break;
    case 'v':
      vars.push_back(optarg);
      break;
    case 'p':
      spectators.push_back(optarg);
      break;
    default:
      cout << "Unknown option " << optarg << ", aborting." << endl;
      return 0;
    }

  if(!vars.size()){
    std::cout << "No training variables specified -- not running TMVA." << std::endl;
    return 0;
  }

  classify(sample,weights,output,treename,methodname,vars,spectators);

  return 0;
}
