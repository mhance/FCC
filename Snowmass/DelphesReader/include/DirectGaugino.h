#ifndef DIRECTGAUGINO_H
#define DIRECTGAUGINO_H

#include "DelphesReaderBase.h"

#include "GenericHistCollection.h"

class DirectGaugino: public DelphesReaderBase{
 public:
  DirectGaugino();
  ~DirectGaugino();

  int ProcessEvent(float weight);
  int BookHists();

 private:

  GenericHistCollection* m_hists_noselection;
  GenericHistCollection* m_hists_preselection;
  GenericHistCollection* m_hists_lowmassZ;
  std::map<int,GenericHistCollection*> m_hists_met;

  float m_met;
  float m_HTjets;
  float m_m_eff;

  float m_HTleptons;
  float m_pt_l0,m_pt_l1,m_pt_l2;
  float m_mT_lllmet;
  float m_Z_M;
  float m_Z_pt;
  float m_W_mT;
  float m_W_charge;
  float m_dPhi_ZoffZ;
  float m_dPhi_min;
  int m_eventClass;
  float m_weight;

  int m_isoLepLep;
  int m_isoLepLep30;
  int m_isoLepLep20;
  int m_isoLepLep10;
  int m_isoLepJet;
  int m_lowMll;
  int m_nBtags;
  int m_nJets;
  int m_nLeptons;

  TTree* m_outputTree;

  void FillTreeVars(EventCandidate& event, float weight);
};

#endif
