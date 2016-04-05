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
	     string file_with_limits_ws="tmp_limits_results/DP_onechan_limit__channel1_DPLSMM_model.root", 
	     string file_with_discov_ws="tmp_limits_results/DP_onechan_discovery__channel1_DPLSMM_model.root", 
	     string wsname="channel1"){

  //fullfreq=0 is AsymptoticCalculator
  //fullfreq=1 is StandardFrequentistCLsDemo

  int seed=1;           // 0 = cpu clock, so random 
  int calculatorType=(fullfreq ? 0:2); // 2=asymptotic limit. 0=frequentist
  int testStatType=3;   // one-sided test profile statistic
  bool useCLs=true;
  double poiValue = 1;

  // set random seed for toy generation
  RooRandom::randomGenerator()->SetSeed(seed);

  // open the workspace
  TFile *wsfile = new TFile(file_with_limits_ws.c_str(),"RO");
  RooWorkspace* w = (RooWorkspace*)wsfile->Get(wsname.c_str()); 

  HypoTestInvTool calc;

  // set parameters
  calc.SetParameter("PlotHypoTestResult", false);
  calc.SetParameter("WriteResult", false);
  calc.SetParameter("Optimize", true);
  calc.SetParameter("UseVectorStore", true);
  calc.SetParameter("GenerateBinned", true);
  calc.SetParameter("NToysRatio", 2);
  calc.SetParameter("MaxPOI", -1);
  calc.SetParameter("UseProof", false);
  calc.SetParameter("Rebuild", false);

  HypoTestInverterResult * hypo = calc.RunInverter(w, 
						   "ModelConfig",
						   "",
						   "obsData", 
						   calculatorType, 
						   testStatType, 
						   useCLs,
						   npoints, 
						   mulow, 
						   muhigh,  
						   ntoys);
  if(hypo){
    double eul2 = 1.20 * hypo->GetExpectedUpperLimit(2);
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
    cout<<"readable results "
	<< hypo->UpperLimit() <<" "
	<< hypo->GetExpectedUpperLimit(-2) << " "
	<< hypo->GetExpectedUpperLimit(-1) <<" "
	<< hypo->GetExpectedUpperLimit(0)  <<" "
	<< hypo->GetExpectedUpperLimit(1)  <<" "
	<< hypo->GetExpectedUpperLimit(2)  <<" "
	<< endl;    
    cout<<"==RESULT== "
	<<hypo->UpperLimit() <<" "
	<<hypo->GetExpectedUpperLimit(0)<<" "
	<<hypo->GetExpectedUpperLimit(1)<<" "
	<<hypo->GetExpectedUpperLimit(-1)<<" "
	<<hypo->GetExpectedUpperLimit(2)<<" "
	<<hypo->GetExpectedUpperLimit(-2)<<endl;    
    delete hypo;
  }
  else{
    cout<< "==RESULT== " << 0 << " "
	<< 0 << " " << 0 << " " << 0 << " "
	<< 0 << " " << 0 << endl;    
  }
 

  // now do discovery.  close and re-open the file to get a fresh workspace.
  wsfile->Close();
  wsfile = new TFile(file_with_discov_ws.c_str(),"RO");
  w = (RooWorkspace*)wsfile->Get(wsname.c_str()); 

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
    RooRealVar * var = dynamic_cast<RooRealVar*>(sbModel->GetParametersOfInterest()->first());
    if (!var) return;
    cout << "Model has no snapshot  - make one using model poi=" << poiValue << endl;
    double oldval = var->getVal();
    if (poiValue > 0)  var->setVal(poiValue);
    sbModel->SetSnapshot( RooArgSet(*var) );
    if (poiValue > 0) var->setVal(oldval);
  }

  cout << endl
       << "-----------------------------------------------------------------------" << endl
       << "Configuring discovery calculator" << endl;

  HypoTestCalculatorGeneric *  hypoCalc = 0;
  if(true){
    //AsymptoticCalculator::SetPrintLevel(10);
    hypoCalc= new  AsymptoticCalculator(*data, *sbModel, *bModel);
    ((AsymptoticCalculator*)hypoCalc)->SetOneSidedDiscovery(true);  
    //((AsymptoticCalculator*)hypoCalc)->SetOneSided(true);
    //((AsymptoticCalculator*)hypoCalc)->SetQTilde(true); // breaks my setup.
  }
  else{
    hypoCalc  = new FrequentistCalculator(*data, *sbModel, *bModel);

    // 1000 for null (B) and 1000 for alt (S+B) 
    ((FrequentistCalculator*)hypoCalc)->SetToys(1,1);

    // create the test statistics
    ProfileLikelihoodTestStat* profll = new ProfileLikelihoodTestStat(*sbModel->GetPdf());
    // use one-sided profile likelihood
    profll->SetOneSidedDiscovery(true);
    
    // configure  ToyMCSampler and set the test statistics
    ToyMCSampler *toymcs = (ToyMCSampler*)hypoCalc->GetTestStatSampler();
    toymcs->SetTestStatistic(profll);
    toymcs->SetNEventsPerToy(1);
    toymcs->SetGenerateBinned(true);
    toymcs->SetUseMultiGen(true);
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

  cout << "Alternate p-value = " << hypoCalcResult->AlternatePValue() << endl;

  /*
  double pvalue = AsymptoticCalculator::GetExpectedPValues( hypoCalcResult->NullPValue(), 
							    hypoCalcResult->AlternatePValue(), 
							    0, 
							    true,
							    true);
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

  string workspace_lim="tmp_limits_results/DP_onechan_limit__channel1_DPLSMM_model.root";
  string workspace_dsc="tmp_limits_results/DP_onechan_discovery__channel1_DPLSMM_model.root";
  string wsname = "channel1";
  
  opterr = 0;
  int c;
  while ((c = getopt (argc, argv, "f:L:D:t:p:l:h:n:")) != -1)
    switch (c){
    case 'L':
      workspace_lim=optarg;
      break;
    case 'D':
      workspace_dsc=optarg;
      break;
    case 'n':
      wsname=optarg;
      break;
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
    
    default:
      return 0;
    }

  runCEws(fullfreq,ntoys,npoints,mulow,muhigh,workspace_lim,workspace_dsc,wsname);

  return 0;
}
