// Dear emacs, this is -*- c++ -*-

#ifndef PARTICLE_CANDIDATE_H
#define PARTICLE_CANDIDATE_H

#include "TLorentzVector.h"

class ParticleCandidate: public TLorentzVector {
 public:
  
  ParticleCandidate():
    TLorentzVector(),
    m_index(-1){
  }

  ParticleCandidate(TLorentzVector& tlv):
    TLorentzVector(tlv),
    m_index(-1),
    m_hadOverEM(0){
  }

  ParticleCandidate(int i):
    TLorentzVector(),
    m_index(i),
    m_hadOverEM(0){
  }

  ParticleCandidate(TLorentzVector& tlv, int i):
    TLorentzVector(tlv),
    m_index(i),
    m_hadOverEM(0){
  }

  ~ParticleCandidate(){}

  ParticleCandidate(ParticleCandidate& rhs):
    TLorentzVector(rhs){
    Copy(const_cast<ParticleCandidate&>(rhs));
  }

  ParticleCandidate(const ParticleCandidate& rhs):
    TLorentzVector(rhs){
    Copy(rhs);
  }

  ParticleCandidate& operator=(const ParticleCandidate &part){
    TLorentzVector::operator=(part);
    Copy(part);
    return *this;
  }

  int Index() { return m_index; }

  void SetIndex(int index) { m_index = index; }

  void SetHadOverEM(float h) { m_hadOverEM = h; }
  float HadOverEM() { return m_hadOverEM; }

  static bool PtSortDescending(ParticleCandidate p0, ParticleCandidate p1) { 
    return p1.Pt()<p0.Pt(); 
  }
  static bool PtSortAscending(ParticleCandidate p0, ParticleCandidate p1) { 
    return p0.Pt()<p1.Pt(); 
  }

protected:
  void Copy(const ParticleCandidate& rhs){
    m_index         = rhs.m_index       ;
    m_hadOverEM     = rhs.m_hadOverEM   ;
  }

  int m_index;

  float m_hadOverEM;
};


#endif
