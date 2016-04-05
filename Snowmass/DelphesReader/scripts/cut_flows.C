#include <iostream>
#include <iomanip>

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

void PrintLatex(float num, int colw){

  if(num>999){
    int l = (int)log10(num);
    float newnum=num/pow(10,l);
    string str = Form("$%3.2f \\times 10^{%d}$",newnum,l);
    cout << std::setprecision(3) << str;
  }
  else if(num>10){
    cout << std::setprecision(3) << std::setw(colw) << num;
  }
  else if(num>1)
    cout << std::setprecision(2) << std::setw(colw) << num;
  else
    cout << std::setprecision(1) << std::setw(colw) << num;    

  return;
}

void go(int energy=14, int lumi=3000, bool SQSQB=false, bool GOSQ=false){
  // get the histograms for different cut points, for different samples
  
  std::vector<string> cuts; std::vector<string> cutlabels;
  cuts.push_back("\\mathrm{Preselection}");                                                             cutlabels.push_back("baseline");
  cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}}/\\sqrt{H_{\\mathrm{T}}} > 15 \\mathrm{GeV}^{1/2}");  cutlabels.push_back("METsignif15");
  cuts.push_back("p_{\\mathrm{T}}^{\\mathrm{lead jet}} < 0.4\\times H_{\\mathrm{T}}");                  cutlabels.push_back("METsignif15_LeadJetLtp4");

  std::vector<string> bgsamples; std::vector<string> bglabels;
  std::vector<string> sgsamples; std::vector<string> sglabels;
  
  if(energy==14){
    bgsamples.push_back("$V$+jets");  bglabels.push_back("Bj_14_000");
    bgsamples.push_back("$t\\bar{t}$");  bglabels.push_back("tt_14_000");
  }
  else if(energy==33){
    bgsamples.push_back("$V$+jets");  bglabels.push_back("Bj_33_000");
    bgsamples.push_back("$t\\bar{t}$");  bglabels.push_back("tt_33_000");
  }
  else if(energy==100){
    bgsamples.push_back("$V$+jets");  bglabels.push_back("Bj_100_000");
    bgsamples.push_back("$t\\bar{t}$");  bglabels.push_back("tt_100_000");
  }
  
  if(!SQSQB && !GOSQ){
    if(energy==14){
      sgsamples.push_back("0500");  sglabels.push_back("GOGO_14_000_0500_0001");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 450 GeV, H_{\\mathrm{T}} > 800 GeV");    cutlabels.push_back("450,800");

      sgsamples.push_back("1255");  sglabels.push_back("GOGO_14_000_1255_0001");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 800 GeV, H_{\\mathrm{T}} > 1650 GeV");    cutlabels.push_back("800,1650");

      sgsamples.push_back("2489");  sglabels.push_back("GOGO_14_000_2489_0001");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 1050 GeV, H_{\\mathrm{T}} > 2600 GeV");    cutlabels.push_back("1050,2600");
    }
    else if(energy==33){
      sgsamples.push_back("1255");  sglabels.push_back("GOGO_33_000_1255_0001");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 900 GeV, H_{\\mathrm{T}} > 1900 GeV");    cutlabels.push_back("900,1900");

      sgsamples.push_back("3152");  sglabels.push_back("GOGO_33_000_3152_0001");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 2100 GeV, H_{\\mathrm{T}} > 3800 GeV");    cutlabels.push_back("2100,3800");

      sgsamples.push_back("4968");  sglabels.push_back("GOGO_33_000_4968_0001");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 2750 GeV, H_{\\mathrm{T}} > 5150 GeV");    cutlabels.push_back("2750,5150");
    }
    else if(energy==100){
      sgsamples.push_back("5012");  sglabels.push_back("GOGO_100_000_5012_0001");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 5150 GeV, H_{\\mathrm{T}} > 9550 GeV");    cutlabels.push_back("5150,9550");

      sgsamples.push_back("9944");  sglabels.push_back("GOGO_100_000_9944_0001");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 5530 GeV, H_{\\mathrm{T}} > 9750 GeV");    cutlabels.push_back("5530,9750");

      sgsamples.push_back("13944");  sglabels.push_back("GOGO_100_000_13944_0001");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 6150 GeV, H_{\\mathrm{T}} > 11700 GeV");    cutlabels.push_back("6150,11700");
    }
  }
  else if(SQSQB){
    if(energy==14){
      sgsamples.push_back("0500");  sglabels.push_back("SQSQ_14_000_0500_0001");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 550 GeV, H_{\\mathrm{T}} > 600 GeV");    cutlabels.push_back("550,600");

      sgsamples.push_back("1255");  sglabels.push_back("SQSQ_14_000_1255_0001");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 1200 GeV, H_{\\mathrm{T}} > 1400 GeV");    cutlabels.push_back("1200,1400");
    }
    else if(energy==33){
      sgsamples.push_back("629");  sglabels.push_back("SQSQ_33_000_0629_0001"); 
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 650 GeV, H_{\\mathrm{T}} > 650 GeV");    cutlabels.push_back("650,650");

      sgsamples.push_back("1255");  sglabels.push_back("SQSQ_33_000_1255_0001"); 
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 1300 GeV, H_{\\mathrm{T}} > 1350 GeV");    cutlabels.push_back("1300,1350");

      sgsamples.push_back("3152");  sglabels.push_back("SQSQ_33_000_3152_0001"); 
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 2650 GeV, H_{\\mathrm{T}} > 3350 GeV");    cutlabels.push_back("2650,3350");
    }
    else if(energy==100){ 
      sgsamples.push_back("3162");  sglabels.push_back("SQSQ_100_000_3162_0001"); 
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 5550 GeV, H_{\\mathrm{T}} > 900 GeV");    cutlabels.push_back("5550,900");

      sgsamples.push_back("5012");  sglabels.push_back("SQSQ_100_000_5012_0001"); 
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 4900 GeV, H_{\\mathrm{T}} > 5450 GeV");    cutlabels.push_back("4900,5450");

      sgsamples.push_back("7944");  sglabels.push_back("SQSQ_100_000_7944_0001"); 
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 6150 GeV, H_{\\mathrm{T}} > 8200 GeV");    cutlabels.push_back("6150,8200");
   }
  }
  else if(GOSQ){
    if(energy==14){
      sgsamples.push_back("2.4,2.8");  sglabels.push_back("GOSQ_14_000_2400_2800");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 1250 GeV, H_{\\mathrm{T}} > 3000 GeV");    cutlabels.push_back("1250,3000");

      sgsamples.push_back("3.2,3.2");  sglabels.push_back("GOSQ_14_000_3200_3200");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 1850 GeV, H_{\\mathrm{T}} > 2850 GeV");    cutlabels.push_back("1870,2850");
    }
    else if(energy==33){
      sgsamples.push_back("3.2,3.2");  sglabels.push_back("GOSQ_33_000_3162_3162");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 2650 GeV, H_{\\mathrm{T}} > 2700 GeV");    cutlabels.push_back("2650,2700"); 

      sgsamples.push_back("6.0,6.0");  sglabels.push_back("GOSQ_33_000_5981_5981");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 3700 GeV, H_{\\mathrm{T}} > 5350 GeV");    cutlabels.push_back("3700,5350");

      sgsamples.push_back("7.0,7.0");  sglabels.push_back("GOSQ_33_000_6981_6981");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 2800 GeV, H_{\\mathrm{T}} > 5600 GeV");    cutlabels.push_back("2800,5600");
    }
    else if(energy==100){
      sgsamples.push_back("8,8");  sglabels.push_back("GOSQ_100_000_07944_07944");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 5700 GeV, H_{\\mathrm{T}} > 8000 GeV");    cutlabels.push_back("5700,8000");

      sgsamples.push_back("16,16");  sglabels.push_back("GOSQ_100_000_15944_15944");
      cuts.push_back("E_{\\mathrm{T}}^{\\mathrm{miss}} > 5800 GeV, H_{\\mathrm{T}} > 17800 GeV");    cutlabels.push_back("5800,17800");
    }
  }
  
  TFile f("various_hists.root","RO");
  
  int colw=25;
  
  string spacer="&";
  string endline="\\\\";
  string hline="\\hline";
  
  TH1F* h;
  cout << hline << endl;
  cout << std::setprecision(3);
  if(!SQSQB && !GOSQ)
    cout << "&&&&\\multicolumn{" << sgsamples.size() << "}{c|}{$m_{\\tilde{g}}$ [GeV]}" << endline << endl;
  else if(SQSQB)
    cout << "&&&&\\multicolumn{" << sgsamples.size() << "}{c|}{$m_{\\tilde{q}}$ [GeV]}" << endline << endl;
  else if(GOSQ)
    cout << "&&&&\\multicolumn{" << sgsamples.size() << "}{c|}{$\\left(m_{\\tilde{g}},m_{\\tilde{q}}\\right)$ [TeV]}" << endline << endl;
  cout << std::setw(2*colw) << "Cut";
  for(unsigned int j=0; j<bgsamples.size(); j++){
    cout << spacer << std::setw(colw) << bgsamples[j];
  }
  cout << spacer << std::setw(colw) << "Total BG";
  for(unsigned int j=0; j<sgsamples.size(); j++){
    cout << spacer << std::setw(colw) << sgsamples[j];
  }
  cout << endline << endl;
  cout << hline << endl;
  
  for(unsigned int i=0; i<cutlabels.size(); i++){
    string cutlabel=cutlabels[i];
    
    std::vector<int> getintegral;
    if(cuts[i].find("{eff}}>") != string::npos){
      getintegral.push_back(atoi(cutlabels[i].c_str()));
      cutlabel=cutlabels[2];
      cout << "$" << std::setw(2*colw) << cuts[i] << "$";
    }
    else if(cutlabels[i].find(",") != string::npos){
      int start=0;
      while(cutlabels[i].find(",",start)!=string::npos){
	int next=cutlabels[i].find(",",start);
	if(next != (int)string::npos)
	  getintegral.push_back(atoi((cutlabels[i].substr(start,next-start)).c_str()));
	else
	  getintegral.push_back(atoi(cutlabels[i].substr(start).c_str()));
	start=next+1;
      }
      getintegral.push_back(atoi(cutlabels[i].substr(start).c_str()));
      cutlabel=cutlabels[2];
      cout << "$" << std::setw(2*colw) << cuts[i].substr(0,cuts[i].find(",")) << "$";
    }
    else{
      cout << "$" << std::setw(2*colw) << cuts[i] << "$";
    }

    float totalbg=0;
    for(unsigned int j=0; j<bgsamples.size(); j++){
      string histname=Form("h_%s_%s_%s",bglabels[j].c_str(),cutlabel.c_str(),"MET___HTjets_30_3p5");
      h = (TH1F*)f.Get(histname.c_str());
      float bg=0;
      cout << spacer; 

      if(getintegral.size()==0)      bg = h->GetSumOfWeights()*lumi;
      else if(getintegral.size()==1) bg = h->Integral(getintegral[0]/10,1000)*lumi;
      else{
	TH2F* h2 = (TH2F*)h;
	bg = h2->Integral((getintegral[1]/100)-1,1001,
			  (getintegral[0]/100)-1,1001)*lumi;
	cout << "\\multirow{2}{*}";
      }
      cout << "{";
      PrintLatex(bg,colw);
      cout << "}";
      totalbg+=bg;
      delete h;
    }

    cout << spacer; 
    if(getintegral.size()>1)
      cout << "\\multirow{2}{*}";
    cout << "{";
    PrintLatex(totalbg, colw);
    cout << "}";

    for(unsigned int j=0; j<sgsamples.size(); j++){
      h = (TH1F*)f.Get(Form("h_%s_%s_%s",sglabels[j].c_str(),cutlabel.c_str(),"MET___HTjets_30_3p5"));
      cout << spacer;
      if(getintegral.size()==0)      PrintLatex(h->GetSumOfWeights()*lumi,colw);
      else if(getintegral.size()==1) PrintLatex(h->Integral(getintegral[0]/10,1000)*lumi,colw);
      else{
	TH2F* h2 = (TH2F*)h;
	cout << "\\multirow{2}{*}{";
	PrintLatex(h2->Integral((getintegral[1]/100)-1,1001,
				(getintegral[0]/100)-1,1001)*lumi, colw);
	cout << "}";
      }
      delete h;
    }
    
    cout << endline << endl;

    if(cutlabels[i].find(",") != string::npos){
      cout << "$" << std::setw(2*colw) << cuts[i].substr(cuts[i].find(",")+1) << "$";
      for(unsigned int j=0; j<sgsamples.size()+bgsamples.size()+1; j++)
	cout << "&";
      cout << endline << endl;
    }
  }
  cout << hline << endl;

}
