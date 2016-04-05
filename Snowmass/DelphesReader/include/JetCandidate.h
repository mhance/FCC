// Dear emacs, this is -*- c++ -*-

#ifndef JET_CANDIDATE_H
#define JET_CANDIDATE_H

#include "ParticleCandidate.h"

class JetCandidate: public ParticleCandidate {
 public:
  JetCandidate():
    ParticleCandidate(){
  }

  JetCandidate(TLorentzVector& v):
    ParticleCandidate(v){
  }

  JetCandidate(TLorentzVector& v, int btag, int tautag):
    ParticleCandidate(v),
    m_btag(btag),
    m_tautag(tautag){
  }

  JetCandidate(TLorentzVector& v, int btag, int tautag, int wtag, int toptag, float trimmedmass, float tau1, float tau2, float tau3):
    ParticleCandidate(v),
    m_btag(btag),
    m_tautag(tautag),
    m_TrimmedMass(trimmedmass),
    m_WTag(wtag),
    m_TopTag(toptag),
    m_Tau1(tau1),
    m_Tau2(tau2),
    m_Tau3(tau3){
  }

  JetCandidate(int i):
    ParticleCandidate(i){
  }

  JetCandidate(TLorentzVector& v, int i):
    ParticleCandidate(v,i){
  }

  /*
  JetCandidate(const JetCandidate& j){
    m_isBad = j.m_isBad;
    m_isBadQuality = j.m_isBadQuality;
    m_btag = j.m_btag;
    m_tautag = j.m_tautag;
    m_TrimmedMass = j.m_TrimmedMass;
    m_WTag=j.m_WTag;
    m_TopTag=j.m_TopTag;
    m_Tau1=j.m_Tau1;
    m_Tau2=j.m_Tau2;
    m_Tau3=j.m_Tau3;
  }
  */

  ~JetCandidate(){};

  bool IsBad()        { return m_isBad;        }
  bool IsBadQuality() { return m_isBadQuality; }
  void SetBad(bool b)        { m_isBad = b;        }
  void SetBadQuality(bool b) { m_isBadQuality = b; }
  int BTag() { return m_btag; }
  int TauTag() { return m_btag; }
  float Tau1() { return m_Tau1; }
  float Tau2() { return m_Tau2; }
  float Tau3() { return m_Tau3; }
  int WTag() { return m_WTag; }
  int TopTag() { return m_TopTag; }
  float TrimmedMass() { return m_TrimmedMass; }

private:
  bool m_isBad;
  bool m_isBadQuality;
  int m_btag;
  int m_tautag;
  
  float m_TrimmedMass;
  int m_WTag,m_TopTag;
  float m_Tau1,m_Tau2,m_Tau3;

};

#endif
