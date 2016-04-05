#ifndef TRUTHFUNCTIONS_H
#define TRUTHFUNCTIONS_H

#include <vector>
#include "TLorentzVector.h"

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
			bool verbose=false,
			float caloIsoCut=0.2,
			float trackIsoCut=0.2);
// ------------------------------------------------


// ------------------------------------------------
//
// Helper function to determine the transverse mass (mT) of a multilepton+MET
// system.
//
float GetMT(std::vector<TLorentzVector> leptons,
	    TLorentzVector MET);
// ------------------------------------------------

// ------------------------------------------------
//
// Helper function to get the M_T^R variable for Razor analyses
//
float GetMTR(TLorentzVector Z0,
	     TLorentzVector Z1,
	     TLorentzVector W0,
	     TLorentzVector MET);
// ------------------------------------------------

// ------------------------------------------------
//
// Helper function to get the M_T^R variable for Razor analyses
//
float GetMR(TLorentzVector Z0,
	    TLorentzVector Z1,
	    TLorentzVector W0);
// ------------------------------------------------

// ------------------------------------------------
//
// Helper function to get the M_T^R variable for Razor analyses
//
float GetRazor(TLorentzVector Z0,
	       TLorentzVector Z1,
	       TLorentzVector W0,
	       TLorentzVector MET);


float GetRazor(std::vector<TLorentzVector> leptons,
	       std::vector<int> flavors,
	       std::vector<int> charges,
	       TLorentzVector MET);
// ------------------------------------------------

#endif
