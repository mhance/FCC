#include <iostream>
#include <iomanip>

#include "HepMCDumper.h"

#include "TLorentzVector.h"
#include "EventCandidate.h"

using namespace std;

// ---------------------------------------------------------------------------------
// Constructor.
//
HepMCDumper::HepMCDumper() :
  DelphesReaderBase(){
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// Destructor.
//
HepMCDumper::~HepMCDumper(){
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// What to do in each event....
// Note that energies, masses, etc are in units of GeV
//
int HepMCDumper::ProcessEvent(float weight){
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

  // now, we go into the dumping phase.
  // * instantiate the output file if it doesn't exist yet, or if the original
  //   file has gotten too big.
  if(EventsProcessed()%1000000==0){
    if(m_HepMCfile.is_open()){
      m_HepMCfile.close();
    }
    m_outputFiles++;
    m_HepMCfile.open(Form("%s.HepMC.%05d.txt",m_outputFileBase.c_str(),m_outputFiles));

    m_HepMCfile << setw( 3) << "#"
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
  m_HepMCfile << setw( 3) << totalparticles++
	     << setw( 3) << ""
	     << setw(15) << EventsProcessed()
	     << setw(15) << 0
	     << setw(15) << weight
	     << endl;

  // * dump MET
  m_HepMCfile << setw( 3) << totalparticles++
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
    m_HepMCfile << setw( 3) << totalparticles++
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

  return 0;
}
// ---------------------------------------------------------------------------------
