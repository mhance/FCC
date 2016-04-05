#include <iostream>

#include "TruthFunctions.h"

using namespace std;

// ------------------------------------------------
//
// Helper function for determining if this lepton is "prompt",
// i.e. from the decay of a W or Z boson (or something like that),
// and not a "fake" lepton from a hadron decay or a "non-prompt"
// lepton from a heavy-flavor meson decay
//
bool IsTruePromptLepton(int index,
			std::vector<float>* mc_pt,
			std::vector<float>* mc_eta,
			std::vector<float>* mc_phi,
			std::vector<float>* mc_m,
			std::vector<float>* mc_charge,
			std::vector<int>* mc_status,
			std::vector<int>* mc_pdgId,
			bool verbose,
			float caloIsoCut,
			float trackIsoCut){

  if(index < 0){
    if(verbose) cout << "Cannot test MC particle with index=" << index << "!" << endl;
    return false;
  }
  if(index > (int)mc_pt->size()){
    if(verbose) cout << "Cannot test MC particle with index=" << index << "!  Larger than MC collection." << endl;
    return false;
  }

  // information about the particle we're testing
  TLorentzVector thisPart;
  thisPart.SetPtEtaPhiM(mc_pt->at(index),
			mc_eta->at(index),
			mc_phi->at(index),
			mc_m->at(index));

  // to say if a lepton is prompt, make a few requirements:
  // -- needs to be stable
  if(mc_status->at(index) != 1){
    if(verbose) cout << "This particle has status=" << mc_status->at(index) << " != 1.  Not prompt." << endl;
    return false;
  }

  // -- it has to be isolated (using tracks)
  float trackIso = 0;

  // -- it has to be isolated (using neutrals)
  float caloIso = 0;

  // -- should not be near a meson
  bool isNearHF = false;

  // loop over all the other particles in the event, figure out if this
  // is a good lepton
  int numMC = mc_pt->size();
  for(int i=0; 
      i<numMC && 
	caloIso < thisPart.Pt()*caloIsoCut && 
	trackIso < thisPart.Pt()*trackIsoCut; 
      i++){

    // don't need to check the particle we're probing
    if(i==index) continue; 

    // ignore geant particles
    if(mc_pdgId->at(i) > 20000000) continue;

    // ignore anything with pT < 1 MeV
    if(mc_pt->at(i) < 1.) continue;

    // if this is a neutrino, then it's not going to contribute to anything
    if(abs(mc_pdgId->at(i)) == 12 ||
       abs(mc_pdgId->at(i)) == 14 ||
       abs(mc_pdgId->at(i)) == 16) continue;
    

    TLorentzVector mcpart;
    mcpart.SetPtEtaPhiM(mc_pt->at(i),mc_eta->at(i),mc_phi->at(i),mc_m->at(i));

    // if it's not close to the particle we're probing, then don't bother
    if(thisPart.DrEtaPhi(mcpart) > 0.3) continue;

    // determine the track and calo isolation
    if(mc_status->at(i)==1 && fabs(mc_charge->at(i))>0){
      if(mc_pt->at(i) > 1000)
	trackIso += mc_pt->at(i);
      caloIso += mc_pt->at(i);
    }
    else if(mc_status->at(i)==1 && mc_charge->at(i)==0){
      caloIso += mc_pt->at(i);
    }

    // check if it's near a HF quark
    if(abs(mc_pdgId->at(i)) == 3 ||
       abs(mc_pdgId->at(i)) == 4 ||
       abs(mc_pdgId->at(i)) == 5 ) isNearHF = true;
  }

  if(trackIso < thisPart.Pt()*trackIsoCut &&
     caloIso < thisPart.Pt()*caloIsoCut &&
     !isNearHF)
    return true;
  else{
    if(trackIso > thisPart.Pt()*0.2){
      if(verbose) cout << "This particle has trackIso = " << trackIso << " > " << thisPart.Pt() << "*0.2." << endl;
    }
    if(caloIso > thisPart.Pt()*0.2){
      if(verbose) cout << "This particle has caloIso = " << caloIso << " > " << thisPart.Pt() << "*0.2." << endl;
    }
    if(isNearHF){
      if(verbose) cout << "This particle is near a heavy-flavor quark!  Not prompt." << endl;
    }
    return false;
  }
}
// ------------------------------------------------


// ------------------------------------------------
//
// Helper function to determine the transverse mass (mT) of a multilepton+MET
// system.
//
float GetMT(std::vector<TLorentzVector> leptons,
	    TLorentzVector MET){

  float totalEt = leptons[0].Et();
  float totalPx = leptons[0].Px();
  float totalPy = leptons[0].Py();

  for(unsigned int i=1; i<leptons.size(); i++){
    totalEt += leptons[i].Et();
    totalPx += leptons[i].Px();
    totalPy += leptons[i].Py();
  }

  float mT = sqrt(pow(MET.Et()+totalEt,2.) -
		  pow(MET.Px()+totalPx,2.) -
		  pow(MET.Py()+totalPy,2.));
  return mT;
}
// ------------------------------------------------

// ------------------------------------------------
//
// Helper function to get the M_T^R variable for Razor analyses
//
float GetMTR(TLorentzVector Z0,
	     TLorentzVector Z1,
	     TLorentzVector W0,
	     TLorentzVector MET){
  TLorentzVector Z = Z0+Z1;

  double dotp = MET.Px()*(Z.Px()+W0.Px()) + MET.Py()*(Z.Py()+W0.Py());

  return sqrt((MET.Et()*(Z.Pt()+W0.Pt()) - fabs(dotp))
	      /2.);
}

// ------------------------------------------------

// ------------------------------------------------
//
// Helper function to get the M_T^R variable for Razor analyses
//
float GetMR(TLorentzVector Z0,
	    TLorentzVector Z1,
	    TLorentzVector W0){
  TLorentzVector Z = Z0+Z1;

  return sqrt(pow((Z.E()+W0.E()),2.) - pow((Z.Pz()+W0.Pz()),2.));
}
// ------------------------------------------------

// ------------------------------------------------
//
// Helper function to get the M_T^R variable for Razor analyses
//
float GetRazor(TLorentzVector Z0,
	       TLorentzVector Z1,
	       TLorentzVector W0,
	       TLorentzVector MET){
  return GetMTR(Z0,Z1,W0,MET)/GetMR(Z0,Z1,W0);
}


float GetRazor(std::vector<TLorentzVector> leptons,
	       std::vector<int> flavors,
	       std::vector<int> charges,
	       TLorentzVector MET){
  
  if(leptons.size() != flavors.size() || leptons.size() != charges.size() || flavors.size() != charges.size())
    return 0;
  if(leptons.size() < 3)
    return 0;

  // find all SFOS pairs.
  std::vector<std::pair<int,int> > pairs;
  for(unsigned int i=0; i<leptons.size(); i++){
    for(unsigned int j=i; j<leptons.size(); j++){
      if(flavors[i] == flavors[j] &&
	 charges[i] == -1*charges[j]){
	pairs.push_back(make_pair(i,j));
      }
    }
  }

  // in the case of no pairs, let's just return 0 for now, to avoid any ambiguity
  if(pairs.size() == 0) return 0;

  int Zpair = -1;

  // if we have exactly one SFOS pair, then it's easy:
  if(pairs.size() == 1){
    Zpair = 0;
  }

  // if we have multiple pairs, then what do we do?
  // in the CMS analysis, they take the combination with the smallest sum of masses
  // squared.  In our case, we'll have one object without any mass (the W),
  // so that's not so elegant. 
  // for now, we'll take the one that's closest to the Z.
  int closestToZ=-1;
  float bestZmass=0;
  for(unsigned int i=0; i<pairs.size(); i++){
    float mass = (leptons[pairs[i].first]+leptons[pairs[i].second]).M();
    if(fabs(mass-91188)<fabs(bestZmass-91188)){
      bestZmass=mass;
      closestToZ=i;
    }
  }
  Zpair = closestToZ;

  // find the vector *not* in the pair
  TLorentzVector W;
  for(unsigned int i=0; i<leptons.size(); i++){
    if((int)i!=pairs[Zpair].first && (int)i!=pairs[Zpair].second){
      W = leptons[i];
      break;
    }
  }
  
  // return the razor
  return GetRazor(leptons[pairs[Zpair].first],
		  leptons[pairs[Zpair].second],
		  W,
		  MET);
  
}
// ------------------------------------------------
