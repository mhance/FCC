// Dear emacs, this is -*- c++ -*-
#ifndef TLorentzVectorHISTS_H
#define TLorentzVectorHISTS_H

#include <string>
#include <vector>

#include "Hist.hh"
#include "HistCollection.h"

#include "TLorentzVector.h"

class TLorentzVectorHists : public HistCollection {
    
public:
  TLorentzVectorHists(std::string name,
		      int nbins_M=200,
		      int max_M=1000,
		      int nbins_pt=1000,
		      int max_pt=1000,
		      int nbins_eta=200,
		      int nbins_phi=40,
		      int nbins_rap=40);
    
  virtual ~TLorentzVectorHists();

  
    
  void SetTLorentzVector(TLorentzVector& tlv) { 
    m_tlv = tlv; 
    m_pt  = m_tlv.Pt();
    m_eta = m_tlv.Eta();
    m_phi = m_tlv.Phi();
    m_m   = m_tlv.M();
    m_rap = m_tlv.Rapidity();
  }

  void SetTLorentzVector(const TLorentzVector& tlv) { 
    SetTLorentzVector(const_cast<TLorentzVector&>(tlv));
  }

  void SetTLorentzVector(TLorentzVector* tlv) { 
    if(tlv){
      SetTLorentzVector(*tlv);
    }
    else{
      std::cerr << __FILE__ << ":" << __LINE__ << ": Cannot handle null TLorentzVector pointer!" << std::endl;
    }
  }
 
  virtual void Fill(float weight);

  virtual void Fill(TLorentzVector& tlv, float weight) {
    SetTLorentzVector(tlv);
    return Fill(weight);
  }

  virtual void CreateHists();

  virtual void Fill(int, float weight){
    return Fill(weight);
  }

  void AddBinning(BinBase* binning){
    if(m_bins_set){
      std::cerr << "Can't add any more binnings!  They're already fixed." << std::endl;
      return;
    }
    GetBinnings()->AddBinning(binning);
  }

  void SetBinning();

  bool BinsSet() { return m_bins_set;}

  virtual void SetHistsCreated(bool flag=true){ m_hists_created = flag;}
  virtual bool GetHistsCreated() { return m_hists_created; }

protected:

  TLorentzVector m_tlv;

  float m_pt;
  float m_eta;
  float m_phi;
  float m_m;
  float m_rap;

  int m_nbins_M;
  int m_max_M;
  int m_nbins_pt;
  int m_max_pt;
  int m_nbins_eta;
  int m_nbins_phi;
  int m_nbins_rap;

  bool m_hists_created;

  bool m_bins_set;
};

#endif

