// Dear emacs, this is -*- c++ -*-

#include "GenericHistCollection.h"

GenericHistCollection::GenericHistCollection(std::string name):
  HistCollection(name),
  m_hists_created(false){
  CreateHists();
}

GenericHistCollection::~GenericHistCollection(){}

// This can be either leading/subleading or tag/probe.  Up to the user
// to input the iterators in some sensible order.
void GenericHistCollection::Fill(float weight){

  SetBinning();

  m_nLeptons = m_event.electrons.size()+m_event.muons.size();
  m_htleptons = 0;
  for(unsigned int i=0; i<m_event.electrons.size(); i++){
    m_htleptons+=m_event.electrons[i].Pt();
  }
  for(unsigned int i=0; i<m_event.muons.size(); i++){
    m_htleptons+=m_event.muons[i].Pt();
  }

  m_nJets30 = 0;
  m_nJets50 = 0;
  m_nJets100 = 0;
  m_nBJets = m_event.btaggedjets.size();
  for(unsigned int i=0; i<m_event.jets.size(); i++){ 
    if(m_event.jets[i].Pt() > 30){
      m_nJets30++;
      if(m_event.jets[i].Pt() > 50){
	m_nJets50++;
	if(m_event.jets[i].Pt() > 100){
	  m_nJets100++;
	}
      }
    }
  }
  
  int hash = GetBinnings()->GetHash();
  std::vector<HistBase*>* hists = GetHists();
  for(unsigned int i=0; i<hists->size(); i++){
    (*hists)[i]->Fill(hash, weight);
  }
}
  
void GenericHistCollection::CreateHists(){
  if(GenericHistCollection::GetHistsCreated()) return;

  std::vector<HistBase*>* hists = GetHists();
  hists->push_back(new ::Hist<int>(&m_nJets30,  "nJets30" , 10, 0, 10, GetName(), "nJets30;Jets (p_{T} > 30 GeV);Entries/Bin"));
  hists->push_back(new ::Hist<int>(&m_nJets50,  "nJets50" , 10, 0, 10, GetName(), "nJets50;Jets (p_{T} > 50 GeV);Entries/Bin"));
  hists->push_back(new ::Hist<int>(&m_nJets100, "nJets100", 10, 0, 10, GetName(), "nJets100;Jets (p_{T} > 100 GeV);Entries/Bin"));
  hists->push_back(new ::Hist<int>(&m_nBJets,   "nBJets"  , 10, 0, 10, GetName(), "nBJets;b-tags;Entries/Bin")); 
  hists->push_back(new ::Hist<int>(&m_nLeptons, "nLeptons", 10, 0, 10, GetName(), "nLeptons;Number of Leptons;Entries/Bin")); 

  hists->push_back(new ::Hist<float>(&m_htleptons, "SumLeptonPt", 1000, 0, 1000, GetName(), "HTLeptons;H_{T}^{leptons} [GeV];Entries/GeV"));
  hists->push_back(new ::Hist<float>(&m_event.MET, "MET", 5000, 0, 5000, GetName(), "MET;MET [GeV];Entries/GeV"));
  hists->push_back(new ::Hist<float>(&m_event.HT, "HT", 5000, 0, 5000, GetName(), "HT;H_{T}^{jets} [GeV];Entries/GeV"));
  hists->push_back(new ::Hist<float>(&m_event.ST, "m_eff", 5000, 0, 5000, GetName(), "m_eff;m_{eff} [GeV];Entries/GeV"));

  this->GenericHistCollection::SetHistsCreated(true);
  return;
}

void GenericHistCollection::Fill(EventCandidate* event, float weight){
  m_event = *event;
  Fill(weight);
}

void GenericHistCollection::Fill(EventCandidate& event, float weight){
  m_event = event;
  Fill(weight);
}
