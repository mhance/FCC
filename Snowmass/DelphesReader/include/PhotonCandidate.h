// Dear emacs, this is -*- c++ -*-
// $Id: PhotonCandidate.h 321154 2013-03-28 21:06:41Z mhance $
#ifndef PhotonCANDIDATE_H
#define PhotonCANDIDATE_H

#include "ParticleCandidate.h"

class PhotonCandidate: public ParticleCandidate {
 public:
  PhotonCandidate (int i): 
    ParticleCandidate(i){}
  PhotonCandidate (): 
    ParticleCandidate(-1){}
  PhotonCandidate(TLorentzVector tlv):
    ParticleCandidate(tlv,-1){}
  PhotonCandidate(TLorentzVector tlv, int i):
    ParticleCandidate(tlv,i){}
  
  ~PhotonCandidate(){}

  PhotonCandidate(PhotonCandidate& rhs):
    ParticleCandidate(rhs){
    Copy(const_cast<const PhotonCandidate&>(rhs));
  }

  PhotonCandidate(const PhotonCandidate& rhs):
    ParticleCandidate(rhs){
    Copy(rhs);
  }

  PhotonCandidate& operator=(const PhotonCandidate &part){
    ParticleCandidate::operator=(part);
    Copy(part);
    return *this;
  }

  bool PhotonTightAR() { return m_PhotonTightAR; }
  bool PhotonTight()   { return m_PhotonTight; }
  bool NNTight()       { return m_NNTight; }

  float TrackIso() { return m_TrackIso; }
  float TopoIso() { return m_TopoIso; }

  void SetPhotonTightAR(bool t) { m_PhotonTightAR = t; }
  void SetPhotonTight(bool t) { m_PhotonTight = t; }
  void SetNNTight(bool t) { m_NNTight = t; }
  void SetMedianED(float ed) { m_ED_median = ed; }
  
  void SetTrackIso(float t) { m_TrackIso = t; }
  void SetTopoIso(float t) { m_TopoIso = t; }
  void SetConvFlag(int f) { m_ConvFlag = f; }

  void SetIndex(int index) { m_index = index; }

  //private:

  void Copy(const PhotonCandidate& rhs){
    m_PhotonTightAR = rhs.m_PhotonTightAR ;
    m_PhotonTight   = rhs.m_PhotonTight ;
    m_NNTight       = rhs.m_NNTight     ;
    m_TrackIso      = rhs.m_TrackIso    ;
    m_TopoIso       = rhs.m_TopoIso     ;
    m_ConvFlag      = rhs.m_ConvFlag    ;
    m_ED_median     = rhs.m_ED_median   ;
  }

  bool m_PhotonTightAR;
  bool m_PhotonTight;
  bool m_NNTight;
  
  float m_TrackIso;
  float m_TopoIso;

  int m_ConvFlag;

  float m_ED_median;

  // when adding variables, be sure to put them in the Copy() function too!
};

#endif
