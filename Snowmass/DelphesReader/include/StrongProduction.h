#ifndef STRONGPRODUCTION_H
#define STRONGPRODUCTION_H

#include "DelphesReaderBase.h"

#include "GenericHistCollection.h"

class StrongProduction: public DelphesReaderBase{
 public:
  StrongProduction();
  ~StrongProduction();

  int ProcessEvent(float weight);
  int BookHists();

  void SetMT2Vals(std::vector<float>& s){m_mT2vals=s;}

 private:

  float m_met;
  float m_met_phi;
  float m_metsignif;
  float m_HTjets;
  float m_HTjets_30_2p5;
  float m_HTjets_30_3p5;
  float m_HTjets_60_2p5;
  float m_HTjets_60_3p5;
  float m_HTjets_100_2p5;
  float m_HTjets_100_3p5;
  float m_m_eff;

  int m_njets25_eta2p5;
  int m_njets30;
  int m_njets30_eta2p5;
  int m_njets30_eta3p5;
  int m_njets60;
  int m_njets60_eta2p5;
  int m_njets60_eta3p5;
  int m_njets100;
  int m_njets100_eta2p5;
  int m_njets100_eta3p5;
  int m_njets200;
  int m_njets200_eta2p5;
  int m_njets200_eta3p5;
  int m_njets500;
  int m_njets1000;
  int m_nbtags;

  float m_massJet0;
  float m_massJet1;
  float m_massJet2;
  float m_massJet3;

  float m_mT2[1000];
  float m_mT2_leading[1000];
  float m_mT2_bjets[1000];
  float m_mT2LSP[1000];

  float m_pTJet[10];
  float m_etaJet[10];
  float m_phiJet[10];
  float m_massJet[10];
  int m_btagJet[10];
  int m_tautagJet[10];

  float m_pTBJet[10];
  float m_etaBJet[10];
  float m_phiBJet[10];
  float m_massBJet[10];

  float m_Tau1Jet[10];
  float m_Tau2Jet[10];
  float m_Tau3Jet[10];

  float m_WTagJet[10];
  float m_TrimmedMassJet[10];
  float m_TopTagJet[10];

  float m_mindPhiMETJet30;
  float m_mindPhiMETJet60;
  float m_mindPhiMETJet100;
  float m_mindPhiMETJet200;
  float m_mindPhiMET4Jet;

  float m_eventbalance;

  float m_dPhiMETBtag0;
  float m_dPhiMETBtag1;
  float m_dPhiMETBtag2;
  float m_mindPhiMETBtag;

  float m_leadJetPt;

  int m_nLeps35;
  
  float m_pTLep[3];
  float m_etaLep[3];
  float m_phiLep[3];
  float m_isoLep[3];
  int m_flavLep[3];
  float m_mTLep[3];

  float m_HTleptons_lep20;
  float m_HTleptons_lep10;
  float m_weight;

  TTree* m_outputTree;
  TTree* m_outputTree_metsignifcut;

  float m_pTTruth[50];
  float m_etaTruth[50];
  float m_phiTruth[50];
  float m_massTruth[50];
  int m_pdgIdTruth[50];
  int m_chargeTruth[50];
  int m_M1Truth[50];
  int m_M2Truth[50];
  int m_D1Truth[50];
  int m_D2Truth[50];
  int m_statusTruth[50];

  std::vector<float> m_mT2vals;

  std::pair<double*,double*> BisectEvent(EventCandidate&, bool useAllJets=true, bool useBJets=false);

  void FillTreeVars(EventCandidate& event, float weight);
};

#endif
