#include <iostream>
#include <iomanip>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"

using namespace std;

void go(){

  std::vector<string> filenames;
  std::vector<string> filetags;

  filenames.push_back("backgrounds/delphes.tt-4p-0-600-v1510_14TEV.NoPileUp.root"	 ); filetags.push_back("tt__00000_00600__NoPileUp__14TEV");
  //filenames.push_back("backgrounds/delphes.tt-4p-600-1100-v1510_14TEV.NoPileUp.root"	 ); filetags.push_back("tt__00600_01100__NoPileUp__14TEV");
  //filenames.push_back("backgrounds/delphes.tt-4p-1100-1700-v1510_14TEV.NoPileUp.root"	 ); filetags.push_back("tt__01100_01700__NoPileUp__14TEV");
  //filenames.push_back("backgrounds/delphes.tt-4p-1700-2500-v1510_14TEV.NoPileUp.root"	 ); filetags.push_back("tt__01700_02500__NoPileUp__14TEV");
  filenames.push_back("backgrounds/delphes.tt-4p-2500-100000-v1510_14TEV.NoPileUp.root"	 ); filetags.push_back("tt__02500_10000__NoPileUp__14TEV");
  filenames.push_back("backgrounds/delphes.Bj-4p-0-300-v1510_14TEV.NoPileUp.root"	 ); filetags.push_back("Bj__00000_00300__NoPileUp__14TEV");
  //filenames.push_back("backgrounds/delphes.Bj-4p-300-600-v1510_14TEV.NoPileUp.root"	 ); filetags.push_back("Bj__00300_00600__NoPileUp__14TEV");
  //filenames.push_back("backgrounds/delphes.Bj-4p-600-1100-v1510_14TEV.NoPileUp.root"	 ); filetags.push_back("Bj__00600_01100__NoPileUp__14TEV");
  //filenames.push_back("backgrounds/delphes.Bj-4p-1100-1800-v1510_14TEV.NoPileUp.root"	 ); filetags.push_back("Bj__01100_01800__NoPileUp__14TEV");
  //filenames.push_back("backgrounds/delphes.Bj-4p-1800-2700-v1510_14TEV.NoPileUp.root"    ); filetags.push_back("Bj__01800_02700__NoPileUp__14TEV");
  //filenames.push_back("backgrounds/delphes.Bj-4p-2700-3700-v1510_14TEV.NoPileUp.root"	 ); filetags.push_back("Bj__02700_03700__NoPileUp__14TEV");
  filenames.push_back("backgrounds/delphes.Bj-4p-3700-100000-v1510_14TEV.NoPileUp.root"	 ); filetags.push_back("Bj__03700_10000__NoPileUp__14TEV");

  filenames.push_back("LHE_GOGO_SQ/delphes.LHE_GOGO_SQ.NoPileUpKiel.GO01255.qq.N01250_14TEV.root"); filetags.push_back("GO01255.qq.N01250__NoPileUp__14TEV");
  filenames.push_back("LHE_GOGO_SQ/delphes.LHE_GOGO_SQ.NoPileUpKiel.GO01255.qq.N00001_14TEV.root"); filetags.push_back("GO01255.qq.N00001__NoPileUp__14TEV");

  for(unsigned int i=0; i<filenames.size(); i++){
    TFile f(filenames[i].c_str(),"RO");
    TH1F dummyhist("dummyhist","dummyhist",100,0,1000000000);
    TTree* t = (TTree*)f.Get("OutputTree");
    t->Draw("m_eff>>dummyhist","weight*((nJets25_loweta>0)&&(HTleptons<1)&&(MET>100))","goff");
    cout << "File " << std::setw(40) << filetags[i] 
	 << " has " << std::setw(15) << dummyhist.GetEntries() 
	 << " entries, which equals " << std::setw(15) << dummyhist.GetSumOfWeights()*300.
	 << " events at 300 fb-1" << std::endl;
    delete t;
    f.Close();
  }
  
}
