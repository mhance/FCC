#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>

#include "TMath.h"
#include "TFile.h"
#include "TH1F.h"
#include "TSystem.h"

#include "RooRandom.h"

using namespace std;

#include "StandardHypoTestInvDemo.C"

using namespace RooFit;
using namespace RooStats;


void runCEws(int fullfreq,
	     int ntoys=3000,
	     int npoints=20,
	     float mulow=0.,
	     float muhigh=4.0,
	     const std::vector<string>& channels = vector<string>(),
	     bool doCombined=true,
	     float sigscale=1.){

  //fullfreq=0 is AsymptoticCalculator
  //fullfreq=1 is StandardFrequentistCLsDemo

  int seed=1;           // 0 = cpu clock, so random 
  int calculatorType=(fullfreq ? 0:2); // 2=asymptotic limit. 0=frequentist
  int testStatType=3;   // one-sided test profile statistic
  bool useCLs=true;
  double poiValue = -1;

  // set random seed for toy generation
  RooRandom::randomGenerator()->SetSeed(seed);

  // open the workspace
  TFile *wsfile; 
  RooWorkspace* w;
  if(doCombined){
    wsfile = new TFile("tmp_limits/limit__combined_MultiLep_model.root","RO");
    w = (RooWorkspace*)wsfile->Get("combined"); 
  }
  else{
    cout << "No support for individual channels in this mode.  Aborting.";
    exit(0);
  }

  HypoTestInvTool calc;

  // set parameters
  calc.SetParameter("PlotHypoTestResult", false);
  calc.SetParameter("WriteResult", false);
  calc.SetParameter("Optimize", true); // <--- true
  calc.SetParameter("UseVectorStore", true); // <---- true
  calc.SetParameter("GenerateBinned", true); // <---- true
  calc.SetParameter("NToysRatio", 2); // <--- 2 
  calc.SetParameter("MaxPOI", -1);
  calc.SetParameter("UseProof", false);
  calc.SetParameter("Rebuild", false); // 

  // run asymptotic limit first to get a range to work in
  HypoTestInverterResult * hypo = calc.RunInverter(w, 
						   "ModelConfig",
						   "",
						   "obsData", 
						   2, 
						   testStatType, 
						   useCLs,
						   npoints, 
						   mulow, 
						   muhigh,  
						   ntoys);
  cout<<"asymptotic check : "
      << hypo->UpperLimit() <<" "
      << hypo->GetExpectedUpperLimit(-2)*sigscale << " "
      << hypo->GetExpectedUpperLimit(-1)*sigscale <<" "
      << hypo->GetExpectedUpperLimit(0)*sigscale  <<" "
      << hypo->GetExpectedUpperLimit(1)*sigscale  <<" "
      << hypo->GetExpectedUpperLimit(2)*sigscale  <<" "
      << hypo->UpperLimit()*sigscale
      << endl;    

  cout << "old eul=" << muhigh << ", new eul=" << 1.20 * hypo->GetExpectedUpperLimit(2)*sigscale << endl;

  if(hypo){
    if(calculatorType!=2){
      double eul2 = 1.20 * hypo->GetExpectedUpperLimit(2);
      if(eul2<muhigh) eul2=muhigh;
      cout << "readable results" << "\t" << eul2 << endl;
      delete hypo;
      hypo = calc.RunInverter(w, 
			      "ModelConfig",
			      "",
			      "obsData", 
			      calculatorType, 
			      testStatType, 
			      useCLs,
			      npoints, 
			      mulow, 
			      eul2,
			      ntoys);
    }

    cout<<"readable results "
	<< hypo->UpperLimit() <<" "
	<< hypo->GetExpectedUpperLimit(-2)*sigscale << " "
	<< hypo->GetExpectedUpperLimit(-1)*sigscale <<" "
	<< hypo->GetExpectedUpperLimit(0)*sigscale  <<" "
	<< hypo->GetExpectedUpperLimit(1)*sigscale  <<" "
	<< hypo->GetExpectedUpperLimit(2)*sigscale  <<" "
	<< hypo->UpperLimit()*sigscale
	<< endl;    
    cout<<"==RESULT== "
	<<hypo->UpperLimit() <<" "
	<<hypo->GetExpectedUpperLimit(0)*sigscale<<" "
	<<hypo->GetExpectedUpperLimit(1)*sigscale<<" "
	<<hypo->GetExpectedUpperLimit(-1)*sigscale<<" "
	<<hypo->GetExpectedUpperLimit(2)*sigscale<<" "
	<<hypo->GetExpectedUpperLimit(-2)*sigscale<<" "
	<<hypo->UpperLimit()*sigscale<<endl;
    delete hypo;
  }
  else{
    cout<< "==RESULT== " << 0 << " "
	<< 0 << " " << 0 << " " << 0 << " "
	<< 0 << " " << 0 << " " << 0 << endl;    
  }
  wsfile->Close();
 
  // now do discovery.  close and re-open the file to get a fresh workspace.
  if(doCombined){
    wsfile = new TFile("tmp_limits/discovery__combined_MultiLep_model.root","RO");
    w = (RooWorkspace*)wsfile->Get("combined"); 
  }
  else{
    cout << "No support for individual channels in this mode.  Aborting.";
    exit(0);
  }

  // get the modelConfig out of the file
  ModelConfig* sbModel = (ModelConfig*) w->obj("ModelConfig");

  // get the data out of the file
  RooAbsData* data = w->data("obsData");

  // make b model
  ModelConfig* bModel = (ModelConfig*) w->obj("");

  // case of no systematics
  // remove nuisance parameters from model
  bool noSystematics=false;
  if (noSystematics) { 
    const RooArgSet * nuisPar = sbModel->GetNuisanceParameters();
    if (nuisPar && nuisPar->getSize() > 0) { 
      std::cout << "Switch off all systematics by setting them constant to their initial values" << std::endl;
      RooStats::SetAllConstant(*nuisPar);
    }
    if (bModel) { 
      const RooArgSet * bnuisPar = bModel->GetNuisanceParameters();
      if (bnuisPar) 
	RooStats::SetAllConstant(*bnuisPar);
    }
  }

  if (!bModel ) {
    cout << "The background model does not exist" << endl
	 << ": Copy it from ModelConfig ModelConfig and set POI to zero" << endl;
    bModel = (ModelConfig*) sbModel->Clone();
    bModel->SetName(TString("ModelConfig")+TString("B_only"));      
    RooRealVar * var = dynamic_cast<RooRealVar*>(bModel->GetParametersOfInterest()->first());
    if (!var) return;
    double oldval = var->getVal();
    var->setVal(0);
    bModel->SetSnapshot( RooArgSet(*var)  );
    var->setVal(oldval);
  }
  
  if (!sbModel->GetSnapshot() || poiValue > 0) { 
    cout << "Model has no snapshot  - make one using model poi" << endl;
    RooRealVar * var = dynamic_cast<RooRealVar*>(sbModel->GetParametersOfInterest()->first());
    if (!var) return;
    double oldval = var->getVal();
    if (poiValue > 0)  var->setVal(poiValue);
    sbModel->SetSnapshot( RooArgSet(*var) );
    if (poiValue > 0) var->setVal(oldval);
  }

  cout << endl
       << "-----------------------------------------------------------------------" << endl
       << "Configuring discovery calculator" << endl;

  HypoTestCalculatorGeneric *  hypoCalc = 0;
  if(fullfreq==0 || true){
    //AsymptoticCalculator::SetPrintLevel(10);
    hypoCalc= new  AsymptoticCalculator(*data, *sbModel, *bModel);
    ((AsymptoticCalculator*)hypoCalc)->SetOneSidedDiscovery(true);  

  }
  else{
    hypoCalc  = new FrequentistCalculator(*data, *sbModel, *bModel);

    // 1000 for null (B) and 1000 for alt (S+B) 
    ((FrequentistCalculator*)hypoCalc)->SetToys(ntoys,ntoys);

    // create the test statistics
    ProfileLikelihoodTestStat* profll = new ProfileLikelihoodTestStat(*sbModel->GetPdf());
    // use one-sided profile likelihood
    profll->SetOneSidedDiscovery(true);
    
    // configure  ToyMCSampler and set the test statistics
    ToyMCSampler *toymcs = (ToyMCSampler*)hypoCalc->GetTestStatSampler();
    toymcs->SetTestStatistic(profll);
  }

  cout << endl
       << "-----------------------------------------------------------------------" << endl
       << "running discoveries" << endl;

  HypoTestResult *  hypoCalcResult = hypoCalc->GetHypoTest();
  hypoCalcResult->SetPValueIsRightTail(true);
  hypoCalcResult->SetBackgroundAsAlt(false);
  hypoCalcResult->Print();

  cout << endl
       << "-----------------------------------------------------------------------" << endl
       << "Getting p-values" << endl;


  /*
  cout << "Alternate p-value = " << hypoCalcResult->AlternatePValue() << endl;
  double pvalue = AsymptoticCalculator::GetExpectedPValues( hypoCalcResult->NullPValue(), 
							    hypoCalcResult->AlternatePValue(), 
							    0, 
							    true,
							    true);
  if(pvalue!=pvalue) pvalue=0.73;
  float signif = ROOT::Math::normal_quantile_c(pvalue,1);
  */

  double pvalue = hypoCalcResult->NullPValue();
  double signif = hypoCalcResult->Significance();

  if(signif < 0) signif=0;

  cout << endl
       << "-----------------------------------------------------------------------" << endl
       << "readable results" << "\t" << pvalue << "\t" << signif << endl;
  cout << "==DISCOVERY== "   << "\t" << pvalue << "\t" << signif << endl;

  wsfile->Close();
}


int main(int argc, char** argv){
  
  float mulow=0;
  float muhigh=5;
  float ntoys=2000;
  float npoints=10;
  int fullfreq=0;
  float sigscale=1.;

  std::vector<string> channels;

  opterr = 0;
  int c;
  while ((c = getopt (argc, argv, "f:t:p:l:h:c:s:")) != -1)
    switch (c){
    case 'f':
      fullfreq = (atoi(optarg)>0);
      break;
    case 't':
      ntoys = atoi(optarg);
      break;
    case 'p':
      npoints = atoi(optarg);
      break;
    case 'l':
      sscanf(optarg,"%f",&mulow);
      break;
    case 'h':
      sscanf(optarg,"%f",&muhigh);
      break;
    case 'c':
      channels.push_back(optarg);
      break;
    case 's':
      sscanf(optarg,"%f",&sigscale);
      break;
    default:
      return 0;
    }

  runCEws(fullfreq,ntoys,npoints,mulow,muhigh,channels,true,sigscale);

  return 0;
}
