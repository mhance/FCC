#include "DirectGaugino.h"

#include "TLorentzVector.h"


// ---------------------------------------------------------------------------------
// Constructor.
//
DirectGaugino::DirectGaugino() :
  DelphesReaderBase(){
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// Destructor.
//
DirectGaugino::~DirectGaugino(){
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// Histograms to book.
//
int DirectGaugino::BookHists(){
  ClearHists(true);

  DEBUG("Creating output TTree");
  m_outputTree = new TTree("OutputTree","OutputTree");
  AddOutputTree(m_outputTree); // this is important!

  m_outputTree->Branch("weight"     , &m_weight      , 'F');
  m_outputTree->Branch("Class"      , &m_eventClass  , 'i');
  m_outputTree->Branch("MET"        , &m_met         , 'F');
  m_outputTree->Branch("HTjets"     , &m_HTjets      , 'F');
  m_outputTree->Branch("HTleptons"  , &m_HTleptons   , 'F');
  m_outputTree->Branch("m_eff"      , &m_m_eff       , 'F');

  m_outputTree->Branch("pt_l0"      , &m_pt_l0       , 'F');
  m_outputTree->Branch("pt_l1"      , &m_pt_l1       , 'F');
  m_outputTree->Branch("pt_l2"      , &m_pt_l2       , 'F');
  m_outputTree->Branch("mT_lllmet"  , &m_mT_lllmet   , 'F');
  m_outputTree->Branch("Z_M"        , &m_Z_M         , 'F');
  m_outputTree->Branch("Z_pt"       , &m_Z_pt        , 'F');
  m_outputTree->Branch("W_mT"       , &m_W_mT        , 'F');
  m_outputTree->Branch("dPhi_ZoffZ" , &m_dPhi_ZoffZ  , 'F');
  m_outputTree->Branch("dPhi_min"   , &m_dPhi_min    , 'F');
  m_outputTree->Branch("W_charge"   , &m_W_charge    , 'F');

  m_outputTree->Branch("isoLepLep"  , &m_isoLepLep30 , 'i');
  m_outputTree->Branch("isoLepLep30", &m_isoLepLep30 , 'i');
  m_outputTree->Branch("isoLepLep20", &m_isoLepLep20 , 'i');
  m_outputTree->Branch("isoLepLep10", &m_isoLepLep10 , 'i');
  m_outputTree->Branch("isoLepJet"  , &m_isoLepJet   , 'i');
  m_outputTree->Branch("lowMll"     , &m_lowMll      , 'i');
  m_outputTree->Branch("nBtags"     , &m_nBtags      , 'i');
  m_outputTree->Branch("nJets"      , &m_nJets       , 'i');
  m_outputTree->Branch("nLeptons"   , &m_nLeptons    , 'i');

  return 0;
}
// ---------------------------------------------------------------------------------


void DirectGaugino::FillTreeVars(EventCandidate& event, float weight){

  m_weight = weight;

  m_met = event.MET;
  m_HTjets = event.HT;
  m_m_eff = event.ST;
  
  std::vector<LeptonBase> leps;
  leps.insert(leps.end(),event.electrons.begin(),event.electrons.end());
  leps.insert(leps.end(),event.muons.begin(),event.muons.end());
  std::sort(leps.begin(), leps.end(), ParticleCandidate::PtSortDescending);

  m_nLeptons = leps.size();

  m_pt_l0 = leps[0].Pt();
  m_pt_l1 = leps[1].Pt();
  m_pt_l2 = leps[2].Pt();
  m_HTleptons = m_pt_l0 + m_pt_l1 + m_pt_l2;

  // find the W and the Z
  TLorentzVector Z;
  TLorentzVector Z1,Z2;
  LeptonBase Wlep;
  m_dPhi_min   = 10;
  if(event.muons.size() >= 3){

    TLorentzVector comb0 = event.muons[0]+event.muons[1];
    TLorentzVector comb1 = event.muons[0]+event.muons[2];
    TLorentzVector comb2 = event.muons[1]+event.muons[2];

    // close to the Z pole?
    float dmZ0 = fabs(comb0.M()-91.112*GeV);
    float dmZ1 = fabs(comb1.M()-91.112*GeV);
    float dmZ2 = fabs(comb2.M()-91.112*GeV);
    
    if(dmZ0 < dmZ1 && dmZ0 < dmZ2){
      Z = comb0;
      Z1 = event.muons[0];
      Z2 = event.muons[1];
      Wlep = event.muons[2];
    }
    else if(dmZ1 < dmZ0 && dmZ1 < dmZ2){
      Z = comb1;
      Z1 = event.muons[0];
      Z2 = event.muons[2];
      Wlep = event.muons[1];
    }
    else if(dmZ2 < dmZ0 && dmZ2 < dmZ1){
      Z = comb2;
      Z1 = event.muons[1];
      Z2 = event.muons[2];
      Wlep = event.muons[0];
    }
    else{
      ERROR ( __FILE__ << __LINE__ << "what am I doing here?");
    }

    m_eventClass = 0;
  }
  else if(event.muons.size() >= 2 && event.electrons.size() >= 1){
    Z = event.muons[0]+event.muons[1];
    Z1 = event.muons[0];
    Z2 = event.muons[1];
    Wlep = event.electrons[0];
    m_eventClass = 1;
  }
  else if(event.muons.size() >= 1 && event.electrons.size() >= 2){
    Z = event.electrons[0]+event.electrons[1];
    Z1 = event.electrons[0];
    Z2 = event.electrons[1];
    Wlep = event.muons[0];
    m_eventClass = 2;
  }
  else if(event.electrons.size() >= 3){
    TLorentzVector comb0 = event.electrons[0]+event.electrons[1];
    TLorentzVector comb1 = event.electrons[0]+event.electrons[2];
    TLorentzVector comb2 = event.electrons[1]+event.electrons[2];

    // close to the Z pole?
    float dmZ0 = fabs(comb0.M()-91.112*GeV);
    float dmZ1 = fabs(comb1.M()-91.112*GeV);
    float dmZ2 = fabs(comb2.M()-91.112*GeV);
    
    if(dmZ0 < dmZ1 && dmZ0 < dmZ2){
      Z = comb0;
      Z1 = event.electrons[0];
      Z2 = event.electrons[1];
      Wlep = event.electrons[2];
    }
    else if(dmZ1 < dmZ0 && dmZ1 < dmZ2){
      Z = comb1;
      Z1 = event.electrons[0];
      Z2 = event.electrons[2];
      Wlep = event.electrons[1];
    }
    else if(dmZ2 < dmZ0 && dmZ2 < dmZ1){
      Z = comb2;
      Z1 = event.electrons[1];
      Z2 = event.electrons[2];
      Wlep = event.electrons[0];
    }
    else{
      ERROR ( __FILE__ << __LINE__ << "what am I doing here?");
    }
    m_eventClass = 3;
  }
  else{
    ERROR ( __FILE__ << __LINE__ << "what am I doing here?");
  }

  float mT2;
  TLorentzVector met = event.GetMET();

  mT2 = pow(met.Et()+Wlep.Pt(),2.)-pow(met.Px()+Wlep.Px(),2.)-pow(met.Py()+Wlep.Py(),2.);
  if(mT2 > 0) m_W_mT = sqrt(mT2);
  else        m_W_mT = sqrt(-1.*mT2);

  // or, alternatively....
  // mT2 = 2*Wlep.Pt()*event.MET*(1.-TMath::Cos(Wlep.Phi()-event.MET_phi));
  // m_W_mt = TMath::Sqrt(mT2);

  m_Z_M  = Z.M();
  m_Z_pt = Z.Pt();

  m_W_charge = Wlep.Charge();

  m_dPhi_ZoffZ = (Z.Pt()>0 && Wlep.Pt()>0) ? (fabs(Z.DeltaPhi(Wlep))):(10);
  m_dPhi_min = 10;
  if(Wlep.Pt()>0){
    if(fabs(Wlep.DeltaPhi(Z1)) < m_dPhi_min) m_dPhi_min = fabs(Wlep.DeltaPhi(Z1));
    if(fabs(Wlep.DeltaPhi(Z2)) < m_dPhi_min) m_dPhi_min = fabs(Wlep.DeltaPhi(Z2));
  }

  mT2 = 
    pow(met.Et()+leps[0].Et()+leps[1].Et()+leps[2].Et(), 2.) -
    pow(met.Px()+leps[0].Px()+leps[1].Px()+leps[2].Px(), 2.) -
    pow(met.Py()+leps[0].Py()+leps[1].Py()+leps[2].Py(), 2.);
  
  if(mT2 > 0) m_mT_lllmet = sqrt(mT2);
  else        m_mT_lllmet = sqrt(-1.*mT2);

  // require isolation of leptons from each other
  m_isoLepLep30=1;
  m_isoLepLep20=1;
  m_isoLepLep10=1;
  for(unsigned int i=0; i<leps.size(); i++){
    for(unsigned int j=0; j<leps.size(); j++){
      if(i==j) continue;
      float separation = leps[i].DrEtaPhi(leps[j]);

      float mass = (leps[i]+leps[j]).M();
      bool onZ = (mass > 75.*GeV) && (mass < 115.*GeV);
      bool SFOS = 
	(leps[i].Flavor() == leps[j].Flavor()) && 
	(leps[i].Charge() != leps[j].Charge());

      if(separation < 0.3 && !(onZ && SFOS)) m_isoLepLep30=0;
      if(separation < 0.2 && !(onZ && SFOS)) m_isoLepLep20=0;
      if(separation < 0.1 && !(onZ && SFOS)) m_isoLepLep10=0;
    }
  }

  // require isolation of leptons from jets
  m_isoLepJet=1;
  for(unsigned int i=0; i<leps.size(); i++){
    for(unsigned int j=0; j<event.jets.size(); j++){
      if(i==j) continue;
      if(leps[i].DrEtaPhi(event.jets[j]) < 0.3) m_isoLepJet=0;
    }
  }
 
  // m(ll)<20
  m_lowMll=0;
  for(unsigned int i=0; i<event.muons.size(); i++){
    for(unsigned int j=0; j<event.muons.size(); j++){
      if(i==j) continue;
      if((leps[i]+leps[j]).M() < 20.*GeV && 
	 leps[i].Charge() != leps[j].Charge()) m_lowMll=1;
    }
  }
  for(unsigned int i=0; i<event.electrons.size(); i++){
    for(unsigned int j=0; j<event.electrons.size(); j++){
      if(i==j) continue;
      if((leps[i]+leps[j]).M() < 20.*GeV && 
	 leps[i].Charge() != leps[j].Charge()) m_lowMll=1;
    }
  }

  m_nBtags = event.btaggedjets.size();
  m_nJets  = event.jets.size();

}

// ---------------------------------------------------------------------------------
// What to do in each event....
// Note that energies, masses, etc are in units of GeV
//
int DirectGaugino::ProcessEvent(float weight){
  DEBUG("In ProcessEvent");

  EventCandidate event;

  TLorentzVector part;

  bool hasLepPt25 = false;

  TIter elecItr(Electrons());
  Electron* elec;
  elecItr.Reset();
  while((elec = (Electron*)elecItr.Next())){
    part.SetPtEtaPhiM(m_units*elec->PT,elec->Eta,elec->Phi,.511*MeV);
    if(part.Pt() > 10.*GeV && fabs(part.Eta())<2.6){
      ElectronCandidate el(part);
      el.SetCharge(elec->Charge);
      event.electrons.push_back(el);
      event.ST += part.Pt();
    }
    if(part.Pt() > 25.*GeV)
      hasLepPt25 = true;
  }

  TIter muonItr(Muons());
  Muon* muon;
  muonItr.Reset();
  while((muon = (Muon*)muonItr.Next())){
    part.SetPtEtaPhiM(m_units*muon->PT,muon->Eta,muon->Phi,105.7*MeV);
    if(part.Pt() > 10.*GeV && fabs(part.Eta())<2.6){
      MuonCandidate mu(part);
      mu.SetCharge(muon->Charge);
      event.muons.push_back(mu);
      event.ST += part.Pt();
    }
    if(part.Pt() > 30.*GeV)
      hasLepPt25 = true;
  }
 
  TIter jetItr(Jets());
  Jet* jet;
  jetItr.Reset();
  while((jet = (Jet*)jetItr.Next())){
    part.SetPtEtaPhiM(m_units*jet->PT,jet->Eta,jet->Phi,m_units*jet->Mass);
    if(part.Pt() > 25.*GeV/* && fabs(part.Eta())<4.5*/){
      if(jet->BTag && fabs(part.Eta())<2.6)
	event.btaggedjets.push_back(BJetCandidate(part));
      event.jets.push_back(JetCandidate(part));
      event.HT += part.Pt();
      event.ST += part.Pt();
    }
  }
 
  TIter metItr(MET());
  MissingET* met(0);
  metItr.Reset();
  while((met = (MissingET*)metItr.Next())){
    part.SetPtEtaPhiE(m_units*met->MET,0,met->Phi,m_units*met->MET);
    event.MET = part.Pt();
    event.MET_phi = part.Phi();
    event.ST += part.Pt();
  }


  // sanity check.
  if(!hasLepPt25 || (event.electrons.size()+event.muons.size()) < 3) return 0;

  // calculate a bunch of event variables
  FillTreeVars(event,weight);

  // fill the tree.
  m_outputTree->Fill();

  // in principle we can also fill some histograms after making some cuts....
  if(!m_isoLepLep) return 0;
  if(!m_isoLepJet) return 0;
  if(event.btaggedjets.size()>0) return 0;
  if(m_lowMll) return 0;

  // fill hists here, if you want.
  // ...

  return 0;
}
// ---------------------------------------------------------------------------------
