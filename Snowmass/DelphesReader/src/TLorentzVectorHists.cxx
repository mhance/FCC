// Dear emacs, this is -*- c++ -*-

#include "TLorentzVectorHists.h"

#include "Hist.icc"

TLorentzVectorHists::TLorentzVectorHists(std::string name,
					 int nbins_M,
					 int max_M,
					 int nbins_pt,
					 int max_pt,
					 int nbins_eta,
					 int nbins_phi,
					 int nbins_rap):
  HistCollection(name),
  m_nbins_M(nbins_M),
  m_max_M(max_M),
  m_nbins_pt(nbins_pt),
  m_max_pt(max_pt),
  m_nbins_eta(nbins_eta),
  m_nbins_phi(nbins_phi),
  m_nbins_rap(nbins_rap),
  m_hists_created(false),
  m_bins_set(false){
  CreateHists();
}

TLorentzVectorHists::~TLorentzVectorHists(){}

// This can be either leading/subleading or tag/probe.  Up to the user
// to input the iterators in some sensible order.
void TLorentzVectorHists::Fill(float weight){

  SetBinning();

  int hash = GetBinnings()->GetHash();

  std::vector<HistBase*>* hists = GetHists();
  for(unsigned int i=0; i<hists->size(); i++){
    (*hists)[i]->Fill(hash, weight);
  }
      
}
  
void TLorentzVectorHists::CreateHists(){
  if(TLorentzVectorHists::GetHistsCreated()) return;
  std::vector<HistBase*>* hists = GetHists();
  hists->push_back(new ::Hist<float>(&m_m,   "M"        , m_nbins_M  ,  0, m_max_M , GetName(), Form("Mass; Invariant Mass [GeV]; Entries/%d GeV",m_max_M/m_nbins_M), 1./1000.));
  hists->push_back(new ::Hist<float>(&m_pt,  "pT"       , m_nbins_pt ,  0, m_max_pt, GetName(), Form("p_{T}; p_{T} [GeV]; Entries/%d GeV",m_max_pt/m_nbins_pt), 1./1000.));
  hists->push_back(new ::Hist<float>(&m_eta, "eta"      , m_nbins_eta,-10,       10, GetName(), Form("#eta; #eta; Entries/%1.2f",(float)m_nbins_eta/20.)));
  hists->push_back(new ::Hist<float>(&m_phi, "phi"      , m_nbins_phi, -5,        5, GetName(), Form("#phi; #phi; Entries/%1.2f",(float)m_nbins_phi/10.)));
  hists->push_back(new ::Hist<float>(&m_rap, "rapidity" , m_nbins_rap,-10,       10, GetName(), Form("Rapidity; Rapidity; Entries/%1.2f",(float)m_nbins_rap/20.)));
    
  this->TLorentzVectorHists::SetHistsCreated(true);
  return;
}

void TLorentzVectorHists::SetBinning(){
  if(m_bins_set) return;

  std::vector<HistBase*>* hists = GetHists();
  for(unsigned int i=0; i<hists->size(); i++) 
    (*hists)[i]->SetBinnings(GetBinnings());
    
  std::vector<HistCollection*>* histcolls = GetHistCollections();
  if(histcolls){
    std::vector<BinBase*>* binnings = GetBinnings()->GetBinnings();
    for(unsigned int i=0; i<histcolls->size(); i++){

      // MH why do I need to clear the binnings here?  Commenting this so I can
      // have sub-collections with a finer binning than whole collections.
      // histcolls->at(i)->GetBinnings()->Clear();
      for(unsigned int j=0; j<binnings->size(); j++){
	histcolls->at(i)->AddBinning(binnings->at(j));
      }
      histcolls->at(i)->SetBinning();
    }
  }
  m_bins_set=true;
}

