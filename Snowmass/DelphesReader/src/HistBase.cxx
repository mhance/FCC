// Dear emacs, this is -*- c++ -*-

#include <iostream>
#include <iomanip>
#include <cmath>
#include "HistBase.h"

using namespace std;

HistBase::HistBase(std::string name, std::string tag, std::string title, float scale) :
  m_name(name),
  m_tag(tag),
  m_title(title),
  m_scale(scale){
}

HistBase::~HistBase(){
  for(std::map<int, TH1*>::iterator hm_iter = m_histmap.begin();
      hm_iter != m_histmap.end();
      ++hm_iter){

    // SFrame screws this up....
    //if(hm_iter->second) delete hm_iter->second;
    //hm_iter->second = NULL;
  }
  m_histmap.clear();
}

void HistBase::SetBinnings(BinCollection* binnings){
  m_binnings = binnings;

  if(!binnings || !binnings->GetBinnings()) return;

  // now, create a new histogram for every variable 
  // for all in m_hists, make m_binnings.size() new hists
  // with names that just have the bin number on the end
  int binning_ctr=0;
  for(std::vector<BinBase*>::iterator b_iter = m_binnings->GetBinnings()->begin();
      b_iter != m_binnings->GetBinnings()->end();
      ++b_iter,binning_ctr++){
    BinBase* b = *b_iter;

    std::map<int,TH1*> new_histmap;
    
    for(std::map<int,TH1*>::iterator m_iter = m_histmap.begin(); m_iter != m_histmap.end(); ++m_iter){
      TH1* thishist=m_iter->second;
      std::string prevname=thishist->GetName();
      int thishash=m_iter->first;
      std::vector<int> thisbinnos=m_binnings->GetBinNumbers(thishash,binning_ctr);
      for(int i=0; i<b->GetNBins(); i++){
	thisbinnos.push_back(i);
	string newname = Form("%s__%s_%d",prevname.c_str(),b->GetName().c_str(),i);
	int newhash = m_binnings->GetHash(thisbinnos,binning_ctr+1);
	new_histmap[newhash]=(TH1*)thishist->Clone(newname.c_str());
	thisbinnos.pop_back();
      }
      delete thishist;
    }
    
    // blast the old histmap
    m_histmap = new_histmap;
  }

  /*
  //cout << "Finished setting binnings." << endl;
  for(std::map<int, TH1*>::iterator hm_iter = m_histmap.begin();
      hm_iter != m_histmap.end();
      ++hm_iter){
    cout << "Hist with hash 0x" << std::setw(10) << std::setfill('0') << std::hex << hm_iter->first 
	 << std::setfill(' ') << std::dec
	 << " has hist with ptr " << hm_iter->second;
    fflush(stdout);
    cout << " and name " << hm_iter->second->GetName() << endl;
  }
  */
}

TH1* HistBase::GetHist(std::vector<int> &binnos){
  // translate this vector of integers into a hash, which we then retrieve
  int hash = m_binnings->GetHash(binnos);
  if(hash>=0)
    return m_histmap[hash];
  else{
    cout << __FILE__ << ", Line " << __LINE__ << ": Can't get a hist with a negative hash!  Sometimes this happens when you've got a binning that doesn't go to 0, or when you're binning in in a variable like eta, and forgot to set the binning to use absolute values...." << endl;
    return (TH1*)0;
  }
}


TH1* HistBase::GetHist(int hash){
  if(hash>=0)
    return m_histmap[hash];
  else{
    cout << __FILE__ << ", Line " << __LINE__ << ": Can't get a hist with a negative hash!  Sometimes this happens when you've got a binning that doesn't go to 0, or when you're binning in in a variable like eta, and forgot to set the binning to use absolute values...." << endl;
    return (TH1*)0;
  }
}

