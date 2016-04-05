#include <iostream>
#include <unistd.h>

#include "TSystem.h"

#include "ExampleDelphesReader.h"
#include "DirectGaugino.h"
#include "StrongProduction.h"
#include "LHCODumper.h"
#include "Logging.h"
#include "Units.h"

using namespace std;


int main(int argc, char** argv){

  string tag="";
  string sample="";
  string postfix="";
  std::vector<string> inputfiles;
  bool SLACsample=false;
  float crossSection=1.;
  string outputdir=".";
  bool reverseScale=false;

  int analysisType=-1;

  double units=GeV;

  string mt2file="";

  int c;
  opterr = 0;
  while ((c = getopt (argc, argv, "vt:s:p:f:Sc:o:T:MNm:")) != -1)
    switch (c){
    case 'v':
      PRINT_DEBUG = 1;
      break;
    case 't':
      tag=optarg;
      break;
    case 's':
      sample=optarg;
      break;
    case 'p':
      postfix=optarg;
      break;
    case 'f':
      inputfiles.push_back(optarg);
      break;
    case 'S':
      SLACsample=true;
      break;
    case 'c':
      sscanf(optarg,"%f",&crossSection);
      break;
    case 'o':
      outputdir=optarg;
      break;
    case 'T':
      analysisType=atoi(optarg);
      break;
    case 'M':
      units=MeV;
      break;
    case 'N':
      reverseScale=true;
      break;
    case 'm':
      mt2file=optarg;
      break;
    default:
      return 0;
    }

  std::vector<float> mt2vals;
  if(mt2file != ""){
    float mt2=0;
    // parse the mt2 file, fill the values
    std::ifstream mt2f(mt2file.c_str());
    while(mt2f >> mt2){ 
      mt2vals.push_back(mt2); 
      std::cout << "Considering mass " << mt2 << " for mT2" << std::endl;
    }
    mt2f.close();
  }
  if(mt2vals.size()==0)
    mt2vals.push_back(0);

  DelphesReaderBase* ex;

  switch(analysisType){
  case 0:
    ex = new DirectGaugino();
    break;
  case 1:
    ex = new StrongProduction();
    ((StrongProduction*)ex)->SetMT2Vals(mt2vals);
    break;
  case 2:
    ex = new LHCODumper();
    break;
  default:
    cout << "You must specify the analysis type:" << endl
	 << "\t" << "0: DirectGaugino" << endl
	 << "\t" << "1: StrongProduction" << endl
	 << endl;
    return 0;
  }

  if(outputdir[outputdir.length()-1] != '/')
    outputdir += "/";

  ex->SetInput(inputfiles);
  ex->EnableAll();
  if(analysisType!=2){
    ex->SetOutput(Form("%sdelphes.%s.%s%s.root",outputdir.c_str(),sample.c_str(),tag.c_str(),postfix.c_str()));
  }
  else{
    ((LHCODumper*)ex)->SetOutputFileName(Form("%sdelphes.%s.%s%s",outputdir.c_str(),sample.c_str(),tag.c_str(),postfix.c_str()));
  }
  ex->SetSampleScale(crossSection);
  ex->ScaleByNEvents(!SLACsample);
  ex->SetUnits(units);
  ex->ProcessEvents();

  delete ex;

  return 0;
}
