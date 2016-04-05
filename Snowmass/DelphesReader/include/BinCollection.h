// Dear emacs, this is -*- c++ -*-

#ifndef COMMON_BINCOLLECTION_H
#define COMMON_BINCOLLECTION_H

#include <vector>
#include <string>

#include "BinBase.h"
#include "Bin.hh"

class BinCollection{
 public :
  BinCollection();
  virtual ~BinCollection();

  virtual void AddBinning(BinBase* binning);

  int GetHash() { 
    std::vector<int> binnos = GetAllBinNumbers(0);
    return GetHash(binnos);
  }
  int GetHash(std::vector<int> binnos, int n_bins_to_check=-1);
  std::vector<int> GetBinNumbers(int hash, int n_bins_to_check=-1);
    
  std::vector<BinBase*>* GetBinnings() { return &m_binnings; }
  unsigned int GetNBinnings() { return m_binnings.size(); }

  virtual std::vector<int> GetAllBinNumbers() { 
    std::vector<BinBase*>* binnings = GetBinnings();
    std::vector<int> retvec(binnings->size());
    for(unsigned int i=0; i<binnings->size(); i++){
      Bin<int>* simpleintbin = dynamic_cast<Bin<int>*>((*binnings)[i]);
      if(simpleintbin){
	retvec[i] = simpleintbin->GetBinNumber();
	continue;
      }
      Bin<float>* simplefloatbin = dynamic_cast<Bin<float>*>((*binnings)[i]);
      if(simplefloatbin){
	retvec[i] = simplefloatbin->GetBinNumber();
	continue;
      }
    }
    return retvec;
  }
  virtual std::vector<int> GetAllBinNumbers(int iter) { 
    return GetAllBinNumbers();
  }
  virtual std::vector<int> GetAllBinNumbers(int lead_iter, int subl_iter) { 
    return GetAllBinNumbers();
  }

  // MH: I think I can take this out....
  //void WriteBinnings(){ 
  //  for(unsigned int i=0; i<m_binnings.size(); i++) 
  //    m_binnings[i]->WriteBins(); 
  //}

  void Clear() { m_binnings.clear(); }

 private:

  std::vector<BinBase*> m_binnings;
};

#endif
