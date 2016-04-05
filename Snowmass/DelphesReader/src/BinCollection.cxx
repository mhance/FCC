// Dear emacs, this is -*- c++ -*-

#include "BinCollection.h"

BinCollection::BinCollection(){}
BinCollection::~BinCollection(){}

void BinCollection::AddBinning(BinBase* binning){
  m_binnings.push_back(binning);
}

int BinCollection::GetHash(std::vector<int> binnos, int n_bins_to_check){
  if(n_bins_to_check<0) n_bins_to_check=m_binnings.size();

  if(binnos.size() != (unsigned int)n_bins_to_check){
    std::cerr << "More bin numbers than binnings!  Aborting." << std::endl;
    return -1;
  }
  
  int hash=0;
  for(unsigned int i=0; i<binnos.size(); i++){
    hash += (int)(binnos[i]*pow((float)MAX_BINS,(int)i));
  }

  return hash;
}

std::vector<int> BinCollection::GetBinNumbers(int hash, int n_bins_to_check){
  // reverse the hash
  if(n_bins_to_check<0) n_bins_to_check=m_binnings.size();
  std::vector<int> retvec(n_bins_to_check);
  for(int i=0; i<n_bins_to_check; i++){
    int bin=(int)(hash/pow((float)MAX_BINS,(int)i))%MAX_BINS;
    if(bin < m_binnings[i]->GetNBins()) retvec[i]=bin;
    else                                retvec[i]=m_binnings[i]->GetNBins();
  }
  return retvec;
}

