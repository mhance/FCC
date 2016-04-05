// Dear emacs, this is -*- c++ -*-

#ifndef MUON_CANDIDATE_H
#define MUON_CANDIDATE_H

#include "LeptonBase.h"

class MuonCandidate: public LeptonBase {
 public:
  MuonCandidate():
    LeptonBase(){
    m_flavor=2;
  }

  MuonCandidate(TLorentzVector& tlv):
    LeptonBase(tlv){
    m_flavor=2;
  }

  MuonCandidate(TLorentzVector& tlv, int i):
    LeptonBase(tlv, i){
    m_flavor=2;
  }

  ~MuonCandidate(){};

};

#endif
