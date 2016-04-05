// Dear emacs, this is -*- c++ -*-

#include "ParticleHists.h"

int  ParticleHists::m_HistDetailLevel = 100;

ParticleHists::ParticleHists(std::string name):
  TLorentzVectorHists(name),
  m_particle(),
  m_bins_set(false)
{
  CreateHists();
}

ParticleHists::~ParticleHists(){}

void ParticleHists::Fill(int iter, float weight){
  SetBinning();

  bool verbose=false;

  int hash = GetBinnings()->GetHash();

  std::vector<HistBase*>* hists = GetHists();
  for(unsigned int i=0; i<hists->size(); i++){
    (*hists)[i]->Fill(hash, weight);
  }
}
  
void ParticleHists::Fill(float weight){
  Fill(0, weight);
}

void ParticleHists::CreateHists(){
  if(GetHistsCreated()) return;

  std::vector<HistBase*>* hists = GetHists();

  if(m_HistDetailLevel >= 2 || m_LocalHistDetailLevel >= 2){
    //hists->push_back(new ::Hist2D<float,float>(&m_eta, &m_phi, "#eta vs #phi", 20, -5, 5, 20, -2.*TMath::Pi(), 2.*TMath::Pi(), GetName(), "#eta vs #phi; #eta; #phi; Events / Bin"));
  }

  this->SetHistsCreated(true);
  return;
}
