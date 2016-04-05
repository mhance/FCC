// Dear emacs, this is -*- c++ -*-

#ifndef ELECTRON_CANDIDATE_H
#define ELECTRON_CANDIDATE_H

#include "LeptonBase.h"

class ElectronCandidate: public LeptonBase {
 public:
  ElectronCandidate():
    LeptonBase(){
    m_flavor = 1;
  }

  ElectronCandidate(TLorentzVector& tlv):
    LeptonBase(tlv){
    m_flavor = 1;
  }

  ElectronCandidate(TLorentzVector& tlv, int i):
    LeptonBase(tlv,i){
    m_flavor = 1;
  }

  ~ElectronCandidate(){};

};

#endif
