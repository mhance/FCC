// Dear emacs, this is -*- c++ -*-

#ifndef LEPTON_BASE_H
#define LEPTON_BASE_H

#include "ParticleCandidate.h"

class LeptonBase: public ParticleCandidate {
 public:

  LeptonBase():
    ParticleCandidate(){}
  LeptonBase(TLorentzVector& tlv):
    ParticleCandidate(tlv){}
  LeptonBase(TLorentzVector& tlv, int i):
    ParticleCandidate(tlv,i){}

  LeptonBase(LeptonBase& rhs):
    ParticleCandidate(rhs){
    Copy(const_cast<LeptonBase&>(rhs));
  }

  LeptonBase(const LeptonBase& rhs):
    ParticleCandidate(rhs){
    Copy(rhs);
  }
  
  void SetTrackIso(float t) { m_TrackIso = t; }
  void SetTopoIso(float t) { m_TopoIso = t; }
  void SetCharge(int c) { m_charge = c; }

  float TrackIso() { return m_TrackIso; }
  float TopoIso() { return m_TopoIso; }

  int Charge() { return m_charge; }
  int Flavor() { return m_flavor; }

  ~LeptonBase(){}

protected:

  int m_charge;
  float m_TrackIso;
  float m_TopoIso;
  int m_flavor;


private:
  void Copy(const LeptonBase& rhs){
    m_charge        = rhs.m_charge;
    m_TrackIso      = rhs.m_TrackIso;
    m_TopoIso       = rhs.m_TopoIso;
    m_flavor        = rhs.m_flavor;
  }
   

};

#endif
