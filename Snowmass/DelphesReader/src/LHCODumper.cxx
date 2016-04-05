#include <iostream>
#include <iomanip>

#include "LHCODumper.h"

#include "TLorentzVector.h"
#include "EventCandidate.h"

using namespace std;

// ---------------------------------------------------------------------------------
// Constructor.
//
LHCODumper::LHCODumper() :
  DelphesReaderBase(){
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// Destructor.
//
LHCODumper::~LHCODumper(){
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// What to do in each event....
// Note that energies, masses, etc are in units of GeV
//
int LHCODumper::ProcessEvent(float weight){
  DEBUG("In ProcessEvent");

  TLorentzVector part;

  EventCandidate event;

  TIter photItr(Photons());
  Photon* phot;
  photItr.Reset();
  while((phot = (Photon*)photItr.Next())){
    part.SetPtEtaPhiM(m_units*phot->PT,phot->Eta,phot->Phi,.511*MeV);
    PhotonCandidate ph(part);
    ph.SetHadOverEM(phot->EhadOverEem);
    event.photons.push_back(ph);
  }

  TIter elecItr(Electrons());
  Electron* elec;
  elecItr.Reset();
  while((elec = (Electron*)elecItr.Next())){
    part.SetPtEtaPhiM(m_units*elec->PT,elec->Eta,elec->Phi,.511*MeV);
    ElectronCandidate el(part);
    el.SetCharge(elec->Charge);
    el.SetHadOverEM(elec->EhadOverEem);
    event.electrons.push_back(el);
  }

  TIter muonItr(Muons());
  Muon* muon;
  muonItr.Reset();
  while((muon = (Muon*)muonItr.Next())){
    part.SetPtEtaPhiM(m_units*muon->PT,muon->Eta,muon->Phi,105.7*MeV);
    MuonCandidate mu(part);
    mu.SetCharge(muon->Charge);
    mu.SetHadOverEM(0);
    event.muons.push_back(mu);
  }
 
  TIter jetItr(Jets());
  Jet* jet;
  jetItr.Reset();
  while((jet = (Jet*)jetItr.Next())){
    part.SetPtEtaPhiM(m_units*jet->PT,jet->Eta,jet->Phi,m_units*jet->Mass);
    if(jet->BTag==1){
      // add n-tracks?
      event.btaggedjets.push_back(BJetCandidate(part));
      event.btaggedjets[event.btaggedjets.size()-1].SetHadOverEM(jet->EhadOverEem);
    }
    else if(jet->TauTag==1){
      // add n-prong information?
      event.taus.push_back(TauCandidate(part));
      event.taus[event.taus.size()-1].SetHadOverEM(jet->EhadOverEem);
      event.taus[event.taus.size()-1].SetCharge(jet->Charge);
    }
    else{
      // add n-tracks?
      event.jets.push_back(JetCandidate(part));
      event.jets[event.jets.size()-1].SetHadOverEM(jet->EhadOverEem);
    }
  }
 
  TIter metItr(MET());
  MissingET* met(0);
  metItr.Reset();
  while((met = (MissingET*)metItr.Next())){
    part.SetPtEtaPhiE(m_units*met->MET,0,met->Phi,m_units*met->MET);
    event.MET = part.Pt();
    event.MET_phi = part.Phi();
  }



  // now, we go into the dumping phase.
  // * instantiate the output file if it doesn't exist yet, or if the original
  //   file has gotten too big.
  if(EventsProcessed()%1000000==0){
    if(m_LHCOfile.is_open()){
      m_LHCOfile.close();
    }
    m_outputFiles++;
    m_LHCOfile.open(Form("%s.LHCO.%05d.txt",m_outputFileBase.c_str(),m_outputFiles));

    m_LHCOfile << setw( 3) << "#"
	       << setw( 3) << "typ"
	       << setw(15) << "eta"
	       << setw(15) << "phi"
	       << setw(15) << "pt"
	       << setw(15) << "jmass"
	       << setw(15) << "ntrk"
	       << setw(15) << "btag"
	       << setw(15) << "had/em"
	       << setw(15) << "dummy"
	       << setw(15) << "dummy"
	       << endl;
  }

  int totalparticles=0;
  // * dump event information
  m_LHCOfile << setw( 3) << totalparticles++
	     << setw( 3) << ""
	     << setw(15) << EventsProcessed()
	     << setw(15) << 0
	     << setw(15) << weight
	     << endl;

  // * dump MET
  m_LHCOfile << setw( 3) << totalparticles++
	     << setw( 3) << 6
	     << setw(15) << 0
	     << setw(15) << event.MET_phi
	     << setw(15) << event.MET
	     << setw(15) << 0
	     << setw(15) << 0
	     << setw(15) << 0
	     << setw(15) << 0
	     << setw(15) << 0
	     << setw(15) << 0
	     << endl;

  // * dump photon information
  for(unsigned int i=0; i<event.photons.size(); i++){
    m_LHCOfile << setw( 3) << totalparticles++
		 << setw( 3) << 0
		 << setw(15) << event.photons[i].Eta()
		 << setw(15) << event.photons[i].Phi()
		 << setw(15) << event.photons[i].Pt()
		 << setw(15) << 0
		 << setw(15) << 0
		 << setw(15) << 0
		 << setw(15) << event.photons[i].HadOverEM()
		 << setw(15) << 0
		 << setw(15) << 0
		 << endl;
  }

  // * dump electron information
  for(unsigned int i=0; i<event.electrons.size(); i++){
    m_LHCOfile << setw( 3) << totalparticles++
	       << setw( 3) << 1
	       << setw(15) << event.electrons[i].Eta()
	       << setw(15) << event.electrons[i].Phi()
	       << setw(15) << event.electrons[i].Pt()
	       << setw(15) << event.electrons[i].M()
	       << setw(15) << event.electrons[i].Charge()
	       << setw(15) << 0
	       << setw(15) << event.electrons[i].HadOverEM()
	       << setw(15) << 0
	       << setw(15) << 0
	       << endl;
  }

  // * dump muon information
  for(unsigned int i=0; i<event.muons.size(); i++){
    m_LHCOfile << setw( 3) << totalparticles++
	       << setw( 3) << 2
	       << setw(15) << event.muons[i].Eta()
	       << setw(15) << event.muons[i].Phi()
	       << setw(15) << event.muons[i].Pt()
	       << setw(15) << event.muons[i].M()
	       << setw(15) << event.muons[i].Charge()
	       << setw(15) << 0
	       << setw(15) << event.muons[i].HadOverEM()
	       << setw(15) << 0
	       << setw(15) << 0
	       << endl;
  }

  // * dump tau information
  for(unsigned int i=0; i<event.taus.size(); i++){
    m_LHCOfile << setw( 3) << totalparticles++
	       << setw( 3) << 3
	       << setw(15) << event.taus[i].Eta()
	       << setw(15) << event.taus[i].Phi()
	       << setw(15) << event.taus[i].Pt()
	       << setw(15) << event.taus[i].M()
	       << setw(15) << event.taus[i].Charge()
	       << setw(15) << 0
	       << setw(15) << event.taus[i].HadOverEM()
	       << setw(15) << 0
	       << setw(15) << 0
	       << endl;
  }

  // * dump jet information
  for(unsigned int i=0; i<event.jets.size(); i++){
    m_LHCOfile << setw( 3) << totalparticles++
	       << setw( 3) << 4
	       << setw(15) << event.jets[i].Eta()
	       << setw(15) << event.jets[i].Phi()
	       << setw(15) << event.jets[i].Pt()
	       << setw(15) << event.jets[i].M()
	       << setw(15) << 0
	       << setw(15) << 0
	       << setw(15) << event.jets[i].HadOverEM()
	       << setw(15) << 0
	       << setw(15) << 0
	       << endl;
  }

  // * dump b-tag information
  for(unsigned int i=0; i<event.btaggedjets.size(); i++){
    m_LHCOfile << setw( 3) << totalparticles++
	       << setw( 3) << 4
	       << setw(15) << event.btaggedjets[i].Eta()
	       << setw(15) << event.btaggedjets[i].Phi()
	       << setw(15) << event.btaggedjets[i].Pt()
	       << setw(15) << event.btaggedjets[i].M()
	       << setw(15) << 0
	       << setw(15) << 1
	       << setw(15) << event.btaggedjets[i].HadOverEM()
	       << setw(15) << 0
	       << setw(15) << 0
	       << endl;
  }

  return 0;
}
// ---------------------------------------------------------------------------------
