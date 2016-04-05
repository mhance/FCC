// Dear emacs, this is -*- c++ -*-

#ifndef TAU_CANDIDATE_H
#define TAU_CANDIDATE_H

#include "LeptonBase.h"

class TauCandidate: public LeptonBase {
 public:
  TauCandidate():
    LeptonBase(){
    m_flavor=3;
  }

  TauCandidate(TLorentzVector& tlv):
    LeptonBase(tlv){
    m_flavor=3;
  }

  ~TauCandidate(){};

};

#endif
