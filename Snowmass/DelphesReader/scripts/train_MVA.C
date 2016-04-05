#include <iostream>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

#include "TMVA/Factory.h"

using namespace std;

class TMVAVar{
public:
  TMVAVar(string v, string t, string u, char y):
    varname(v),vartitle(t),unit(u),vartype(y){}
  
  string varname;
  string vartitle;
  string unit;
  char vartype;
};

void train(TFile* sigFile,
	   TFile* bkgFile,
	   TFile* outFile,
	   std::vector<TMVAVar*>& vars,
	   std::vector<string>& spectators,
	   string treeName,
	   string tag,
	   string cut){
  
  outFile->cd();

  TTree* sigTree = (TTree*)sigFile->Get(treeName.c_str());
  TTree* bkgTree = (TTree*)bkgFile->Get(treeName.c_str());

  outFile->cd();

  cout << "Creating factory" << endl;
  TMVA::Factory* factory = new TMVA::Factory( Form("MVA_%s",tag.c_str()), outFile, 
					      "V:Color:DrawProgressBar:AnalysisType=Classification" );
  Double_t weight=1.;
  cout << "Adding trees" << endl;
  factory->AddSignalTree    (sigTree, weight);
  factory->AddBackgroundTree(bkgTree, weight);

  cout << "Adding spectators and weights" << endl;
  for(unsigned int i=0; i<spectators.size(); i++){
    cout << "... " << spectators[i] << endl;
    factory->AddSpectator(spectators[i].c_str());
  }

  // this one we hard code
  factory->AddSpectator("weight");
  factory->SetWeightExpression("weight");

  cout << "Adding variables" << endl;
  for(unsigned int i=0; i<vars.size(); i++){
    cout << "... " << vars[i]->varname << endl;
    factory->AddVariable(vars[i]->varname.c_str(),
			 vars[i]->vartitle.c_str(),
			 vars[i]->unit.c_str(),
			 vars[i]->vartype);
  }

  cout << "Preparing trees" << endl;
  factory->PrepareTrainingAndTestTree(cut.c_str(),"NormMode=None");

  /*
  factory->BookMethod( TMVA::Types::kCuts, "Cuts", 
		       "FitMethod=GA:VarProp=FMax");
  */
  outFile->cd();
  cout << "Booking BDT" << endl;
  factory->BookMethod( TMVA::Types::kBDT, "BDT",
		       "!H:V:NTrees=850:nEventsMin=150:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:SeparationType=GiniIndex:nCuts=20:PruneMethod=NoPruning" );

  cout << "Training" << endl;

  factory->TrainAllMethods();
  factory->TestAllMethods();
  factory->EvaluateAllMethods();

  return;
}


void train_DG(string signal,
	      string background,
	      string output,
	      string tag,
	      string cut,
	      std::vector<string>& variables,
	      std::vector<string>& spectators){
  TFile* sig = new TFile(signal.c_str(),"RO");
  TFile* bkg = new TFile(background.c_str(),"RO");
  
  // this is a bit of a kludge, in that we assume all variables are floats and
  // all have units of GeV.  maybe fix it later.
  std::vector<TMVAVar*> vars;
  for(unsigned int i=0; i<variables.size(); i++){
    if(variables[i][0] != 'n')
      vars.push_back(new TMVAVar(variables[i], variables[i], "GeV", 'F'));
    else
      vars.push_back(new TMVAVar(variables[i], variables[i], "", 'i'));
  }

  TFile* out = new TFile(output.c_str(),"RECREATE");
  train(sig,bkg,out,vars,spectators,"OutputTree",tag,cut);
  out->Close();
}

int main(int argc, char** argv){
  
  string signal;
  string background;
  string output;
  string tag;
  string cut;
  bool useZmass=false;

  std::vector<string> variables,spectators;

  opterr = 0;
  int c;
  while ((c = getopt (argc, argv, "s:b:o:t:c:v:p:")) != -1)
    switch (c){
    case 's':
      signal=optarg;
      break;
    case 'b':
      background=optarg;
      break;
    case 'o':
      output=optarg;
      break;
    case 't':
      tag=optarg;
      break;
    case 'c':
      cut=optarg;
      break;
    case 'v':
      variables.push_back(optarg);
      break;
    case 'p':
      spectators.push_back(optarg);
      break;
    default:
      return 0;
    }

  if(!variables.size()){
    std::cout << "No training variables specified -- not running TMVA." << std::endl;
    return 0;
  }

  train_DG(signal,background,output,tag,cut,variables,spectators);

  return 0;
}
