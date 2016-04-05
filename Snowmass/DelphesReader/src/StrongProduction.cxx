#include "StrongProduction.h"
#include "mt2_bisect.h"

#include "TLorentzVector.h"
#include <map>

// ---------------------------------------------------------------------------------
// Constructor.
//
StrongProduction::StrongProduction() :
  DelphesReaderBase(){
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// Destructor.
//
StrongProduction::~StrongProduction(){
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// Histograms to book.
//
int StrongProduction::BookHists(){
  ClearHists(true);

  DEBUG("Creating output TTree");
  m_outputTree = new TTree("OutputTree","OutputTree");
  AddOutputTree(m_outputTree); // this is important!

  m_outputTree->Branch("weight"     , &m_weight      , 'F');
  m_outputTree->Branch("MET"        , &m_met         , 'F');
  m_outputTree->Branch("MET_phi"    , &m_met_phi     , 'F');
  m_outputTree->Branch("HTjets"     , &m_HTjets      , 'F');
//   m_outputTree->Branch("HTjets_30_3p5"     , &m_HTjets_30_3p5      , 'F');
//   m_outputTree->Branch("HTjets_30_2p5"     , &m_HTjets_30_2p5      , 'F');
//   m_outputTree->Branch("HTjets_60_3p5"     , &m_HTjets_60_3p5      , 'F');
//   m_outputTree->Branch("HTjets_60_2p5"     , &m_HTjets_60_2p5      , 'F');
//   m_outputTree->Branch("HTjets_100_3p5"    , &m_HTjets_100_3p5     , 'F');
//   m_outputTree->Branch("HTjets_100_2p5"    , &m_HTjets_100_2p5     , 'F');
  m_outputTree->Branch("HTleptons"  , &m_HTleptons_lep10   , 'F');
  m_outputTree->Branch("HTleptons_lep20"  , &m_HTleptons_lep20   , 'F');
  m_outputTree->Branch("m_eff"      , &m_m_eff       , 'F');
  m_outputTree->Branch("METsignif"  , &m_metsignif   , 'F');
  m_outputTree->Branch("nBtags"     , &m_nbtags      , 'i');
  //m_outputTree->Branch("nJets25_eta2p5"    , &m_njets25_eta2p5     , 'i');
  m_outputTree->Branch("nJets30"           , &m_njets30            , 'i');
  //m_outputTree->Branch("nJets30_eta3p5"    , &m_njets30_eta3p5     , 'i');
  //m_outputTree->Branch("nJets30_eta2p5"    , &m_njets30_eta2p5     , 'i');
  m_outputTree->Branch("nJets60"           , &m_njets60            , 'i');
  //m_outputTree->Branch("nJets60_eta2p5"    , &m_njets60_eta2p5     , 'i');
  //m_outputTree->Branch("nJets60_eta3p5"    , &m_njets60_eta3p5     , 'i');
  m_outputTree->Branch("nJets100"           , &m_njets100            , 'i');
  m_outputTree->Branch("nJets100_eta2p5"    , &m_njets100_eta2p5     , 'i');
  m_outputTree->Branch("nJets100_eta3p5"    , &m_njets100_eta3p5     , 'i');
  m_outputTree->Branch("nJets200"           , &m_njets200            , 'i');
//   m_outputTree->Branch("nJets200_eta2p5"    , &m_njets200_eta2p5     , 'i');
//   m_outputTree->Branch("nJets200_eta3p5"    , &m_njets200_eta3p5     , 'i');
  m_outputTree->Branch("nJets500"           , &m_njets500            , 'i');
  m_outputTree->Branch("nJets1000"          , &m_njets1000           , 'i');
  m_outputTree->Branch("mT2"              , &m_mT2             , Form("mT2[%d]/F",(int)m_mT2vals.size()));
  m_outputTree->Branch("mT2_leading"      , &m_mT2_leading     , Form("mT2_leading[%d]/F",(int)m_mT2vals.size()));
  m_outputTree->Branch("mT2_bjets"        , &m_mT2_bjets       , Form("mT2_bjets[%d]/F",(int)m_mT2vals.size()));
  m_outputTree->Branch("mT2LSP"           , &m_mT2LSP          , Form("mT2LSP[%d]/F",(int)m_mT2vals.size()));

  m_outputTree->Branch("pTJet"            , m_pTJet            , "pTJet[10]/F");
  m_outputTree->Branch("etaJet"           , m_etaJet           , "etaJet[10]/F");
  m_outputTree->Branch("phiJet"           , m_phiJet           , "phiJet[10]/F");
  m_outputTree->Branch("massJet"          , m_massJet          , "massJet[10]/F");
  m_outputTree->Branch("btagJet"          , m_btagJet          , "btagJet[10]/I"); 
  m_outputTree->Branch("tautagJet"        , m_tautagJet        , "tautagJet[10]/I"); 

  m_outputTree->Branch("Tau1Jet"          , m_Tau1Jet          , "Tau1Jet[10]/F"); 
  m_outputTree->Branch("Tau2Jet"          , m_Tau2Jet          , "Tau2Jet[10]/F"); 
  m_outputTree->Branch("Tau3Jet"          , m_Tau3Jet          , "Tau3Jet[10]/F"); 
  m_outputTree->Branch("WTagJet"          , m_WTagJet          , "WTagJet[10]/I"); 
  m_outputTree->Branch("TrimmedMassJet"   , m_TrimmedMassJet   , "TrimmedMassJet[10]/F"); 
  m_outputTree->Branch("TopTagJet"        , m_TopTagJet        , "TopTagJet[10]/I"); 

  // add corresponding b-tag branches
  m_outputTree->Branch("pTBJet"            , m_pTBJet            , "pTBJet[10]/F");
  m_outputTree->Branch("etaBJet"           , m_etaBJet           , "etaBJet[10]/F");
  m_outputTree->Branch("phiBJet"           , m_phiBJet           , "phiBJet[10]/F");
  m_outputTree->Branch("massBJet"          , m_massBJet          , "massBJet[10]/F");

  m_outputTree->Branch("mindPhiMetJet30"       , &m_mindPhiMETJet30         , 'F');
  m_outputTree->Branch("mindPhiMetJet60"       , &m_mindPhiMETJet60         , 'F');
  m_outputTree->Branch("mindPhiMetJet100"      , &m_mindPhiMETJet100        , 'F');
  m_outputTree->Branch("mindPhiMetJet200"      , &m_mindPhiMETJet200        , 'F');
  m_outputTree->Branch("mindPhiMet4Jet"        , &m_mindPhiMET4Jet          , 'F');

  m_outputTree->Branch("eventbalance"       , &m_eventbalance        , 'F');

  m_outputTree->Branch("dPhiMETBtag0"       , &m_dPhiMETBtag0        , 'F');
  m_outputTree->Branch("dPhiMETBtag1"       , &m_dPhiMETBtag1        , 'F');
  m_outputTree->Branch("dPhiMETBtag2"       , &m_dPhiMETBtag2        , 'F');
  m_outputTree->Branch("mindPhiMETBtag"     , &m_mindPhiMETBtag      , 'F');
  
  m_outputTree->Branch("nLeps35"            , &m_nLeps35             , 'i');

  m_outputTree->Branch("pTLep"             , m_pTLep              , "pTLep[3]/F");
  m_outputTree->Branch("etaLep"            , m_etaLep             , "etaLep[3]/F");
  m_outputTree->Branch("phiLep"            , m_phiLep             , "phiLep[3]/F");
  //m_outputTree->Branch("isoLep"            , m_isoLep             , "isoLep[3]/F");
  m_outputTree->Branch("flavLep"           , m_flavLep            , "flavLep[3]/I");
  m_outputTree->Branch("mTLep"             , &m_mTLep             , "mTLep[3]/F");

  m_outputTree->Branch("pTTruth"             , m_pTTruth              , "pTTruth[50]/F");
  m_outputTree->Branch("etaTruth"            , m_etaTruth             , "etaTruth[50]/F");
  m_outputTree->Branch("phiTruth"            , m_phiTruth             , "phiTruth[50]/F");
  m_outputTree->Branch("massTruth"           , m_massTruth            , "massTruth[50]/F");
  m_outputTree->Branch("pdgIdTruth"          , m_pdgIdTruth           , "pdgIdTruth[50]/I");
  //m_outputTree->Branch("chargeTruth"          , m_chargeTruth           , "chargeTruth[50]/i");
  m_outputTree->Branch("M1Truth"          , m_M1Truth           , "M1Truth[50]/I");
  m_outputTree->Branch("M2Truth"          , m_M2Truth           , "M2Truth[50]/I");
  m_outputTree->Branch("D1Truth"          , m_D1Truth           , "D1Truth[50]/I");
  m_outputTree->Branch("D2Truth"          , m_D2Truth           , "D2Truth[50]/I");
  m_outputTree->Branch("statusTruth"          , m_statusTruth           , "statusTruth[50]/I");

  return 0;
}
// ---------------------------------------------------------------------------------


std::pair<double*,double*> StrongProduction::BisectEvent(EventCandidate& event, 
							 bool useAllJets,
							 bool useBJets){

  if(event.jets.size()<2){
    return std::make_pair((double*)0,(double*)0);
  }

  bool verbose=false;

  std::vector<TLorentzVector> alljets;
  for(unsigned int i=0; i<event.jets.size(); i++){
    if(event.jets[i].Pt()>40*GeV && fabs(event.jets[i].Eta())<3.) 
      alljets.push_back(event.jets[i]);
  }

  if(alljets.size()<2){
    return std::make_pair((double*)0,(double*)0);
  }

  if(useAllJets){

    // okay, first identify seed jets.  take jets with largest dijet invariant mass as seeds.
    TLorentzVector jet[2];
    unsigned int seed[]={0,0};
    float maxmass=0;
    for(unsigned int i=0; i<alljets.size(); i++){
      if(verbose) std::cout << "event has jet " << i << " with pT=" << alljets[i].Pt() << ", eta=" << alljets[i].Eta() << ", phi=" << alljets[i].Phi() << std::endl;
      for(unsigned int j=i+1; j<alljets.size(); j++){
	float m=(alljets[i]+alljets[j]).M();
	if(m>maxmass){
	  maxmass=m;
	  seed[0]=i;
	  seed[1]=j;
	}
      }
    }
    jet[0]=alljets[seed[0]];
    jet[1]=alljets[seed[1]];
    if(verbose) std::cout << "seeds are " << seed[0] << " and " << seed[1] << " with mass=" << maxmass << std::endl;

    // now we're supposed to take all the other jets and cluster them according to
    // some metric, e.g. what CMS uses (Lund distance):
    /*
      E_i * (E_i - p_i * cos(theta_ik)) / (E_i + E_k)^2
    */
  
    bool stillsorting=true;
    float metric[2];
    std::vector<TLorentzVector> jets[2],newjets[2];

    jets[0].push_back(jet[0]);
    jets[1].push_back(jet[1]);
    for(unsigned int i=0; i<alljets.size(); i++){
      if(i!=seed[0]&&i!=seed[1])
	jets[1].push_back(alljets[i]);
    }

    int iterations=0;
    static int failures=0;
    TLorentzVector oldjet[2];
    while(stillsorting){
      if(verbose) std::cout << "Seed jet 0 has pT=" << jet[0].Pt() << ", eta=" << jet[0].Eta() << ", phi=" << jet[0].Phi() << std::endl;
      if(verbose) std::cout << "Seed jet 1 has pT=" << jet[1].Pt() << ", eta=" << jet[1].Eta() << ", phi=" << jet[1].Phi() << std::endl;

      if(iterations>0)
	stillsorting=false;

      // sort through each of the two jet collections, see whether any jets should be reassigned
      // to the other seed jet.
      // if even one jet is reassigned, we want to enter this loop again.
      for(unsigned int j=0; j<2; j++){

	if(verbose) std::cout << "Checking " << jets[j].size() << " jets for seed jet " << j << std::endl;

	// keep the seed jets where they are.
	std::vector<TLorentzVector>::iterator i=jets[j].begin();
	newjets[j].push_back(*i);
	i++;
	for(;i!=jets[j].end();++i){
	  if(verbose) std::cout << "Checking event jet with pT=" << i->Pt() << std::endl;
	  for(unsigned int k=0; k<2; k++){
	    float costheta=jet[k].Px()*i->Px() + jet[k].Py()*i->Py() + jet[k].Pz()*i->Pz();
	    costheta /= i->P();
	    metric[k] = jet[k].E() * (jet[k].E() - jet[k].P()*costheta) / ((jet[k].E()+i->E())*(jet[k].E()+i->E()));
	  }
	
	  if(metric[(j+1)%2]<=metric[j]){
	    if(verbose) std::cout << "Moving event jet with m=" << i->M() << ", pT=" << i->Pt() << " to " << (j+1)%2 << " because " << metric[(j+1)%2] << " < " << metric[j] << std::endl;
	    newjets[(j+1)%2].push_back(*i);
	    stillsorting=true;
	  }
	  else{
	    if(verbose) std::cout << "Not moving jet with pT=" << i->Pt() << std::endl;
	    newjets[j].push_back(*i);
	  }
	}
	jets[j].clear();
      }
      if(verbose) std::cout << "=============================================================================" << std::endl;

      // now make new seed jets
      for(unsigned int j=0; j<2; j++){
	if(verbose) std::cout << "populating " << j << " with " << newjets[j].size() << " jets" << std::endl;
	oldjet[j]=jet[j];
	jet[j]=newjets[j][0];
	jets[j].push_back(newjets[j][0]);
	for(unsigned int k=1; k<newjets[j].size(); k++){
	  if(verbose) std::cout << "adding jet to " << j << std::endl;
	  jet[j]+=newjets[j][k];
	  jets[j].push_back(newjets[j][k]);
	}
	newjets[j].clear();
      }

      if(jet[0].DrEtaPhi(oldjet[1])<0.01 && jet[1].DrEtaPhi(oldjet[0])<0.01){
	if(verbose) std::cout << "Flopping jets.  continuing." << std::endl;
	break;
      }
      if(iterations++>100){ 
	failures++;
	break;
      }
    }
  
    if(iterations>100){
      if(verbose) std::cout << "Failed to converge after 100 iterations." << std::endl;
      return std::make_pair((double*)0,(double*)0);
    }
    else{
      if(verbose) std::cout << "Converged after " << iterations << " iterations." << std::endl;
    }

    //if(failures>10) exit(0);

    // we should now have two super-jets.
    double pa[] = {jet[0].M(), jet[0].Px(), jet[0].Py()};
    double pb[] = {jet[1].M(), jet[1].Px(), jet[1].Py()};
    return std::make_pair(pa,pb);
  }
  else{
    if(!useBJets){
      double pa[] = {alljets[0].M(), alljets[0].Px(), alljets[0].Py()};
      double pb[] = {alljets[1].M(), alljets[1].Px(), alljets[1].Py()};
      return std::make_pair(pa,pb);
    }
    else{
      if(event.btaggedjets.size()<2){
	return std::make_pair((double*)0,(double*)0);
      }

      double pa[] = {event.btaggedjets[0].M(), event.btaggedjets[0].Px(), event.btaggedjets[0].Py()};
      double pb[] = {event.btaggedjets[1].M(), event.btaggedjets[1].Px(), event.btaggedjets[1].Py()};
      return std::make_pair(pa,pb);
    }
  }
  
}

void StrongProduction::FillTreeVars(EventCandidate& event, float weight){

  m_weight = weight;

  m_met = event.MET;
  m_met_phi = event.MET_phi;
  m_HTjets = event.HT;
  m_m_eff = event.ST;

  // truth content
  TIter partItr(Particles());
  GenParticle* part;
  partItr.Reset();
  int count=0;
  for(int i=0; i<50; i++){
    m_pTTruth[i]=-1;
    m_etaTruth[i]=-20;
    m_phiTruth[i]=-10;
    m_massTruth[i]=-1;
    m_statusTruth[i]=0;
    m_pdgIdTruth[i]=0;
    m_chargeTruth[i]=-10;
    m_M1Truth[i]=-1;
    m_M2Truth[i]=-1;
    m_D1Truth[i]=-1;
    m_D2Truth[i]=-1;
  }
  while((part = (GenParticle*)partItr.Next()) && count<50){
    m_pTTruth[count]=part->PT;
    m_etaTruth[count]=part->Eta;
    m_phiTruth[count]=part->Phi;
    m_massTruth[count]=part->Mass;
    m_statusTruth[count]=part->Status;
    m_pdgIdTruth[count]=part->PID;
    m_chargeTruth[count]=part->Charge;
    m_M1Truth[count]=part->M1;
    m_M2Truth[count]=part->M2;
    m_D1Truth[count]=part->D1;
    m_D2Truth[count]=part->D2;
    count++;
  }

  TLorentzVector met;
  met.SetPtEtaPhiM(event.MET, 0, event.MET_phi, 0);

  std::vector<LeptonBase> leps;
  leps.insert(leps.end(),event.electrons.begin(),event.electrons.end());
  leps.insert(leps.end(),event.muons.begin(),event.muons.end());
  std::sort(leps.begin(), leps.end(), ParticleCandidate::PtSortDescending);

  m_HTleptons_lep20=0;
  m_HTleptons_lep10=0;
  m_nLeps35=0;
  for(unsigned int i=0; i<3; i++){
    m_pTLep[i]=0;
    m_etaLep[i]=0;
    m_phiLep[i]=0;
    //m_isoLep[i]=0;
    m_flavLep[i]=-10;
    m_mTLep[i]=0;
  }
  for(unsigned int i=0; i<leps.size(); i++){
    if(i<3){
      m_pTLep[i]=leps[i].Pt();
      m_etaLep[i]=leps[i].Eta();
      m_phiLep[i]=leps[i].Phi();
      //m_isoLep[i]=leps[i].TrackIso();
      m_flavLep[i]=leps[i].Flavor()*leps[i].Charge();
      if(event.MET>0){
	m_mTLep[i]=pow(leps[i].Et()+met.Et(),2.)-
	  pow(leps[i].Px()+met.Px(),2.)-
	  pow(leps[i].Py()+met.Py(),2.);
	if(m_mTLep[i]<0)
	  m_mTLep[i]=TMath::Sqrt(-1.*m_mTLep[i]);
	else
	  m_mTLep[i]=TMath::Sqrt(m_mTLep[i]);
      }
    }

    if(leps[i].Pt() >= 10*GeV){
      m_HTleptons_lep10+=leps[i].Pt();
      if(leps[i].Pt() >= 20*GeV){
	m_HTleptons_lep20+=leps[i].Pt();
	if(leps[i].Pt() >= 35*GeV){
	  m_nLeps35++;
	}
      }
    }
  }

  //std::cout << m_mTLep[0] << "\t" << event.MET << "\t" << leps.size() << std::endl;

  m_metsignif = event.MET/TMath::Sqrt(event.HT);
  m_njets25_eta2p5 = 0;
  m_njets30 = 0;
  m_njets30_eta2p5 = 0;
  m_njets30_eta3p5 = 0;
  m_njets60 = 0;
  m_njets60_eta2p5 = 0;
  m_njets60_eta3p5 = 0;
  m_njets100 = 0;
  m_njets100_eta2p5 = 0;
  m_njets100_eta3p5 = 0;
  m_njets200 = 0;
  m_njets200_eta2p5 = 0;
  m_njets200_eta3p5 = 0;
  m_njets500 = 0;
  m_njets1000 = 0;
  m_leadJetPt = 0;
  m_HTjets_30_2p5 = 0;
  m_HTjets_30_3p5 = 0;
  m_HTjets_60_2p5 = 0;
  m_HTjets_60_3p5 = 0;
  m_HTjets_100_2p5 = 0;
  m_HTjets_100_3p5 = 0;
   

  for(unsigned int i=0; i<10; i++){
    m_massJet[i]=0;
    m_pTJet[i]=0;
    m_etaJet[i]=-99;
    m_phiJet[i]=-99;
    m_btagJet[i]=0;
    m_tautagJet[i]=0;
    m_Tau1Jet[i]=-2000;
    m_Tau2Jet[i]=-2000;
    m_Tau3Jet[i]=-2000;
    m_WTagJet[i]=0;
    m_TopTagJet[i]=0;
    m_TrimmedMassJet[i]=-2000;

    m_massBJet[i]=0;
    m_pTBJet[i]=0;
    m_etaBJet[i]=-99;
    m_phiBJet[i]=-99;
  }

  m_mindPhiMETJet30 = 9;
  m_mindPhiMETJet60 = 9;
  m_mindPhiMETJet100 = 9;
  m_mindPhiMETJet200 = 9;
  m_mindPhiMET4Jet = 9;

  std::sort(event.jets.begin(), event.jets.end(), ParticleCandidate::PtSortDescending);

  TLorentzVector jetsum;

  for(unsigned int i=0; i<event.jets.size(); i++){
    float dPhiMETJet = event.jets[i].DeltaPhi(met);

    if(i==0){
      jetsum=event.jets[i];
    }
    else{
      jetsum+=event.jets[i];
    }

    if(i<10){
      m_pTJet[i]=event.jets[i].Pt();
      m_etaJet[i]=event.jets[i].Eta();
      m_phiJet[i]=event.jets[i].Phi();
      m_massJet[i]=event.jets[i].M();
      m_btagJet[i]=(int)event.jets[i].BTag();
      m_tautagJet[i]=(int)event.jets[i].TauTag();
      m_Tau1Jet[i]=event.jets[i].Tau1();
      m_Tau2Jet[i]=event.jets[i].Tau2();
      m_Tau3Jet[i]=event.jets[i].Tau3();
      m_WTagJet[i]=event.jets[i].WTag();
      m_TopTagJet[i]=event.jets[i].TopTag();
      m_TrimmedMassJet[i]=event.jets[i].TrimmedMass();
    }

    if(event.jets[i].Pt() > 25.*GeV && fabs(event.jets[i].Eta()) < 2.5) 
      m_njets25_eta2p5++;


    if((fabs(dPhiMETJet) < fabs(m_mindPhiMET4Jet)) && i<4)
      m_mindPhiMET4Jet = dPhiMETJet;

    if(event.jets[i].Pt() > 30.*GeV){
      m_njets30++;
      if(fabs(event.jets[i].Eta())<3.5){
	m_HTjets_30_3p5 += event.jets[i].Pt();
	m_njets30_eta3p5++;
	if(fabs(event.jets[i].Eta())<2.5){
	  m_HTjets_30_2p5 += event.jets[i].Pt();
	  m_njets30_eta2p5++;
	}
      }

      if(fabs(dPhiMETJet) < fabs(m_mindPhiMETJet30))
	m_mindPhiMETJet30 = dPhiMETJet;
    }

    if(event.jets[i].Pt() > 60.*GeV){
      m_njets60++;
      if(fabs(event.jets[i].Eta())<3.5){
	m_HTjets_60_3p5 += event.jets[i].Pt();
	m_njets60_eta3p5++;
	if(fabs(event.jets[i].Eta())<2.5){
	  m_HTjets_60_2p5 += event.jets[i].Pt();
	  m_njets60_eta2p5++;
	}
      }

      if(fabs(dPhiMETJet) < fabs(m_mindPhiMETJet60))
	m_mindPhiMETJet60 = dPhiMETJet;
    }

    if(event.jets[i].Pt() > 100.*GeV){
      m_njets100++;
      if(fabs(event.jets[i].Eta())<3.5){
	m_HTjets_100_3p5 += event.jets[i].Pt();
	m_njets100_eta3p5++;
	if(fabs(event.jets[i].Eta())<2.5){
	  m_HTjets_100_2p5 += event.jets[i].Pt();
	  m_njets100_eta2p5++;
	}
      }

      if(fabs(dPhiMETJet) < fabs(m_mindPhiMETJet100))
	m_mindPhiMETJet100 = dPhiMETJet; 
    }

    if(event.jets[i].Pt() > 200.*GeV){
      m_njets200++;
      if(fabs(event.jets[i].Eta())<3.5){
	m_njets200_eta3p5++;
	if(fabs(event.jets[i].Eta())<2.5){
	  m_njets200_eta2p5++;
	}
      }

      if(fabs(dPhiMETJet) < fabs(m_mindPhiMETJet200))
	m_mindPhiMETJet200 = dPhiMETJet;
    }

    if(event.jets[i].Pt() > 500.*GeV && fabs(event.jets[i].Eta())<3.5){
      m_njets500++;
    }

    if(event.jets[i].Pt() > 1000.*GeV && fabs(event.jets[i].Eta())<3.5){
      m_njets1000++;
    }
  }

  m_eventbalance=(jetsum+met).Pt();

  m_mindPhiMETBtag = 9;
  m_dPhiMETBtag0 = 9;
  m_dPhiMETBtag1 = 9;
  m_dPhiMETBtag2 = 9;
  
  m_nbtags = event.btaggedjets.size();
  std::sort(event.btaggedjets.begin(), event.btaggedjets.end(), ParticleCandidate::PtSortDescending);

  for(unsigned int i=0; i<event.btaggedjets.size(); i++){
    float dPhiMETJet = event.jets[i].DeltaPhi(met);

    if(i<10){
      m_pTBJet[i]=event.btaggedjets[i].Pt();
      m_etaBJet[i]=event.btaggedjets[i].Eta();
      m_phiBJet[i]=event.btaggedjets[i].Phi();
      m_massBJet[i]=event.btaggedjets[i].M();
    }

    switch(i){
    case 0: 
      m_dPhiMETBtag0 = dPhiMETJet;
      break;
    case 1: 
      m_dPhiMETBtag1 = dPhiMETJet;
      break;
    case 2: 
      m_dPhiMETBtag2 = dPhiMETJet;
      break;
    default:
      break;
    }

    if(fabs(dPhiMETJet) < fabs(m_mindPhiMETBtag))
      m_mindPhiMETBtag = dPhiMETJet;
  }


  // calculate mT2
  if(m_mT2vals.size()>0){
    // need to do the sorting of jets/etc into hemispheres
    std::pair<double*,double*> bisect=BisectEvent(event);

    // if we got a valid bisection, then go ahead and compute mT2 for all the LSP masses under study
    if(bisect.first==0 || bisect.second==0){
      for(unsigned int i=0; i<m_mT2vals.size(); i++){
	m_mT2[i] = -1;
      }
    }
    else{
      double pmiss[3]={0, met.Px(), met.Py()};
      mt2_bisect::mt2 mt2_event;
      mt2_event.set_momenta(bisect.first,bisect.second,pmiss);
      for(unsigned int i=0; i<m_mT2vals.size(); i++){
	mt2_event.set_mn(m_mT2vals[i]);
	m_mT2[i] = mt2_event.get_mt2();
	m_mT2LSP[i] = m_mT2vals[i];
      }
    }

    // do the whole thing again, but this time just do it based on the leading two jets
    bisect=BisectEvent(event,false);

    // if we got a valid bisection, then go ahead and compute mT2 for all the LSP masses under study
    if(bisect.first==0 || bisect.second==0){
      for(unsigned int i=0; i<m_mT2vals.size(); i++){
	m_mT2_leading[i] = -1;
      }
    }
    else{
      double pmiss[3]={0, met.Px(), met.Py()};
      mt2_bisect::mt2 mt2_event;
      mt2_event.set_momenta(bisect.first,bisect.second,pmiss);
      for(unsigned int i=0; i<m_mT2vals.size(); i++){
	mt2_event.set_mn(m_mT2vals[i]);
	m_mT2_leading[i] = mt2_event.get_mt2();
      }
    }

    // do the whole thing again, but this time just do it based on the leading two b-jets
    bisect=BisectEvent(event,false,true);

    // if we got a valid bisection, then go ahead and compute mT2 for all the LSP masses under study
    if(bisect.first==0 || bisect.second==0){
      for(unsigned int i=0; i<m_mT2vals.size(); i++){
	m_mT2_bjets[i] = -1;
      }
    }
    else{
      double pmiss[3]={0, met.Px(), met.Py()};
      mt2_bisect::mt2 mt2_event;
      mt2_event.set_momenta(bisect.first,bisect.second,pmiss);
      for(unsigned int i=0; i<m_mT2vals.size(); i++){
	mt2_event.set_mn(m_mT2vals[i]);
	m_mT2_bjets[i] = mt2_event.get_mt2();
      }
    }

  }
}

// ---------------------------------------------------------------------------------
// What to do in each event....
// Note that energies, masses, etc are in units of GeV
//
int StrongProduction::ProcessEvent(float weight){
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
    if(part.Pt() > 25.*GeV)
      hasLepPt25 = true;
  }
 
  TIter jetItr(Jets());
  Jet* jet;
  jetItr.Reset();
  while((jet = (Jet*)jetItr.Next())){
    part.SetPtEtaPhiM(m_units*jet->PT,jet->Eta,jet->Phi,m_units*jet->Mass);
    if(part.Pt() > 25.*GeV && fabs(part.Eta())<4.5){
      if((jet->BTag & 0x1) && fabs(part.Eta())<2.6)
	event.btaggedjets.push_back(BJetCandidate(part));
      event.jets.push_back(JetCandidate(part,jet->BTag,jet->TauTag,jet->WTag,jet->TopTag,jet->TrimmedMass,jet->Tau1,jet->Tau2,jet->Tau3));
      if(part.Pt() > 30.*GeV){
	event.HT += part.Pt();
	event.ST += part.Pt();
      }
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


  // fill the tree
  if(event.MET > 100.*GeV){
    FillTreeVars(event,weight);
    m_outputTree->Fill();
  }

  return 0;
}
// ---------------------------------------------------------------------------------
