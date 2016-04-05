#include "ExampleDelphesReader.h"

#include "TLorentzVector.h"


// ---------------------------------------------------------------------------------
// Constructor.
//
ExampleDelphesReader::ExampleDelphesReader() :
  DelphesReaderBase(){
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// Destructor.
//
ExampleDelphesReader::~ExampleDelphesReader(){
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// Histograms to book.
//
int ExampleDelphesReader::BookHists(){
  ClearHists(true);

  m_hists["e_pt"]  = new TH1F("e_pt" ,"Electron p_{T};p_{T} [GeV];Entries/GeV",1000,0,1000);
  m_hists["mu_pt"] = new TH1F("mu_pt","Muon p_{T};p_{T} [GeV];Entries/GeV"    ,1000,0,1000);
  
  return 0;
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// What to do in each event....
// Note that energies, masses, etc are in units of GeV
//
int ExampleDelphesReader::ProcessEvent(float weight){
  DEBUG("In ProcessEvent");

  TLorentzVector part;

  TIter elecItr(Electrons());
  Electron* elec;
  elecItr.Reset();
  while((elec = (Electron*)elecItr.Next())){
    part.SetPtEtaPhiE(elec->PT,elec->Eta,elec->Phi,0.000511);
    if(part.Pt() > 100){
      DEBUG("electron has pT = " << part.Pt());
      m_hists["e_pt"]->Fill(part.Pt(),weight);
    }
  }

  TIter muonItr(Muons());
  Muon* muon;
  muonItr.Reset();
  while((muon = (Muon*)muonItr.Next())){
    part.SetPtEtaPhiE(muon->PT,muon->Eta,muon->Phi,0.1057);
    DEBUG("muon has pT = " << part.Pt());
    m_hists["mu_pt"]->Fill(part.Pt(),weight);
  }
 
  TIter jetItr(Jets());
  Jet* jet;
  jetItr.Reset();
  while((jet = (Jet*)jetItr.Next())){
    part.SetPtEtaPhiE(jet->PT,jet->Eta,jet->Phi,jet->Mass);
  }
 
  return 0;
}
// ---------------------------------------------------------------------------------
