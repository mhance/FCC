// Dear emacs, this is -*- c++ -*-

#ifndef COMMON_HISTCOLLECTION_H
#define COMMON_HISTCOLLECTION_H

#include <string>
#include <vector>

#include "BinBase.h"
#include "BinCollection.h"
#include "HistBase.h"

#include "TVector.h"
#include "TDirectory.h"

class WriteableBin {
public:
  WriteableBin(){}
  WriteableBin(TVector v,std::string n):
    binvec(v),name(n){}
  ~WriteableBin(){}

  TVector binvec;
  std::string name;
};


class HistCollection {
 public:
  HistCollection(std::string name):
    m_name(name),
    m_hists_created(false){
    m_binnings = new BinCollection();
  }

  virtual ~HistCollection(){
    for(unsigned int i=0; i<m_hists.size(); i++){
      if(m_hists[i]) delete m_hists[i];
    }
    m_hists.clear();

    std::vector<HistCollection*>* children = GetHistCollections();
    for(unsigned int j=0; children && j<children->size(); j++){
      if((*children)[j]) delete (*children)[j];
    }
    children->clear();
  }

  void Clear(){
    for(unsigned int i=0; i<m_hists.size(); i++){
      m_hists[i]->Clear();
    }

    std::vector<HistCollection*>* children = GetHistCollections();
    for(unsigned int j=0; j<children->size(); j++){
      (*children)[j]->Clear();
    }
  }

  std::vector<HistBase*>* GetHists() { return &m_hists; }
  //virtual void AddHist(std::string name, int nbins, float low, float high, std::string title) = 0;
  virtual void AddHist(HistBase* hist){
    m_hists.push_back(hist);
  };
  
  std::string GetName() const { return m_name; }

  virtual void AddBinning(BinBase *b){
    m_binnings->AddBinning(b);
  }

  virtual void SetBinning() = 0;

  virtual void Fill(int iter, float weight) = 0;

  virtual void SetBinningPtr(BinCollection* ptr) { m_binnings = ptr; }

  virtual std::vector<WriteableBin> GetWriteableBinnings();
  
  void SetOutFileDirectory(TDirectory* dir) { m_collection_base_dir = dir; }

  TDirectory* GetOutFileDirectory() { return m_collection_base_dir; }

  BinCollection* GetBinnings() { return m_binnings; }

  std::vector<HistCollection*>* GetHistCollections() { return &m_histCollections; }

  inline void AddHistCollection(HistCollection* hc) { m_histCollections.push_back(hc); }

  static inline void AddAdditionalHist(HistBase* h) { m_additionalHists.push_back(h); }

  inline std::vector<HistBase*>* GetAdditionalHists() { return &m_additionalHists; }

  void CreateAdditionalHists();

  virtual void CreateHists() {m_hists_created = true;}

  virtual void SetHistsCreated(bool flag=true){ m_hists_created = flag;}
  virtual bool GetHistsCreated() { return m_hists_created; }

 private:

  BinCollection* m_binnings;

  std::string m_name;

  std::vector<HistBase*> m_hists;

  TDirectory* m_collection_base_dir;

  std::vector<HistCollection*> m_histCollections;

  static std::vector<HistBase*> m_additionalHists;

  bool m_hists_created;

};

#endif
