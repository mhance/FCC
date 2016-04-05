#ifndef GENERICHISTCOLLECTION_H
#define GENERICHISTCOLLECTION_H

#include "HistCollection.h"

#include <string>
#include <vector>

#include "Hist.hh"
#include "Hist2D.hh"

#include "TLorentzVector.h"
#include "TLorentzVectorHists.h"

#include "EventCandidate.h"

class GenericHistCollection : public HistCollection {
    
public:
  GenericHistCollection(std::string name);
    
  virtual ~GenericHistCollection();
    
  void Fill(float weight);

  void Fill(EventCandidate* event, float weight);
  void Fill(EventCandidate& event, float weight);

  void CreateHists();

  void SetBinning(){}
  void Fill(int, float weight) { Fill(weight); }

  virtual void SetHistsCreated(bool flag=true){ m_hists_created = flag;}
  virtual bool GetHistsCreated()              { return m_hists_created; }

private:

  bool m_hists_created;

  EventCandidate m_event;

  int m_nJets30;
  int m_nJets50;
  int m_nJets100;
  int m_nBJets;
  int m_nLeptons;
  int m_nPhotons;
  float m_htleptons;

};
#endif
