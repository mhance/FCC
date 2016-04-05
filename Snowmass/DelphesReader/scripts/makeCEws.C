// Originally by Daniel Whiteson, 
// Modified by Sourabh Dube
#include "TFile.h"
#include "TH1.h"
#include "TSystem.h"


//This is the analysis selection which runs top.xml
void makeCEws(int ndata, float nfake, float nsmdp, float nsignal)
{
  TString outputrootname = "tmp_limits_data/counting_exp_data_DP.root";
  //outputrootname is what is used in onechan.xml

  TFile *tf = new TFile(outputrootname,"recreate");

  TH1F *hs = new TH1F("signal","signal histogram",1,0,1);
  hs->Fill(0.5,nsignal);
  hs->Write("signal");
  TH1F *hd = new TH1F("data","data histogram",1,0,1);
  hd->Fill(0.5,ndata);
  hd->Write("data");
  TH1F *hfake = new TH1F("fake","fake histogram",1,0,1);
  hfake->Fill(0.5,nfake);
  hfake->Write("fake");
  TH1F *hsmdp = new TH1F("smdp","smdp histogram",1,0,1);
  hsmdp->Fill(0.5,nsmdp);
  hsmdp->Write("smdp");
  tf->Close();

  cout<<"Made histogram file: "<<outputrootname<<" with (data,nFake,ntt,nwt,nwz,nSignal)=("
      <<ndata<<","<<nfake<<","<<nsmdp<<","<<nsignal<<")"<<endl;
  gSystem->Exec("hist2workspace tmp_limits/top.xml"); 
  cout<<"Made workspace using tmp_limits/top.xml"<<endl;
}
