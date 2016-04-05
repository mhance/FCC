// Dear emacs, this is -*- c++ -*-

#ifndef EVENTCANDIDATE_H
#define EVENTCANDIDATE_H

#include <vector>
#include <algorithm>
#include "TLorentzVector.h"

#include "PhotonCandidate.h"
#include "ElectronCandidate.h"
#include "MuonCandidate.h"
#include "TauCandidate.h"
#include "JetCandidate.h"
#include "BJetCandidate.h"

class EventCandidate{
 public:

  EventCandidate():
    MET(0),
    HT(0),
    ST(0),
    MET_phi(0),
    weight(1.){
  }
    
  EventCandidate(EventCandidate& rhs){
    Copy(const_cast<EventCandidate&>(rhs));
  }

  EventCandidate& operator=(const EventCandidate &rhs){
    Copy(rhs);
    return *this;
  };

  ~EventCandidate(){}

  std::vector<JetCandidate>      jets;
  std::vector<BJetCandidate>     btaggedjets;
  std::vector<ElectronCandidate> electrons;
  std::vector<MuonCandidate>     muons;
  std::vector<TauCandidate>      taus;
  std::vector<PhotonCandidate>   photons;

  float MET,HT,ST;
  float MET_phi;

  float weight;

  int RunNumber;

  TLorentzVector GetMET(){
    TLorentzVector v;
    v.SetPtEtaPhiM(MET,0.,MET_phi,0.);
    return v;
  }

  int GetNLeptons(){ return electrons.size()+muons.size(); }

protected:
  void Copy(const EventCandidate& rhs);
};

#endif
