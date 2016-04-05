#include "EventCandidate.h"

void EventCandidate::Copy(const EventCandidate& rhs){

  weight              = rhs.weight;
  RunNumber           = rhs.RunNumber;

  jets                = rhs.jets;
  btaggedjets         = rhs.btaggedjets;
  electrons           = rhs.electrons;
  muons               = rhs.muons;
  taus                = rhs.taus;
  photons             = rhs.photons;

  MET                 = rhs.MET;
  MET_phi             = rhs.MET_phi;
  HT                  = rhs.HT;
  ST                  = rhs.ST;

}
