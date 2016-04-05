#include "HistCollection.h"

std::vector<HistBase*> HistCollection::m_additionalHists;

std::vector<WriteableBin> HistCollection::GetWriteableBinnings(){

  std::vector<WriteableBin> retvec;

  // make the list for the parent
  BinCollection* bc = GetBinnings();
  std::vector<BinBase*>* binnings = bc->GetBinnings();
  for(unsigned int j=0; j<binnings->size(); j++){
    retvec.push_back(WriteableBin((*binnings)[j]->GetTVectorBins(),
				  Form("%s/%s_binning",GetName().c_str(),(*binnings)[j]->GetName().c_str())));
  }
  

  // add the children's lists
  std::vector<HistCollection*>* children = GetHistCollections();
  for(unsigned int j=0; j<children->size(); j++){
    std::vector<WriteableBin> childvec = children->at(j)->GetWriteableBinnings();
    for(unsigned int i=0; i<childvec.size(); i++){
      childvec[i].name = GetName()+"/"+childvec[i].name;
    }
    retvec.insert(retvec.end(),childvec.begin(),childvec.end());
  }

  return retvec;
}


void HistCollection::CreateAdditionalHists(){
  std::vector<HistBase*>* additionalHists = GetAdditionalHists();
  
  std::vector<HistBase*>* hists = GetHists();
  for(unsigned int i=0; i<additionalHists->size(); i++){
    HistBase* hs = additionalHists->at(i)->Clone(GetName());
    hists->push_back(hs);
  }
}
