#ifndef DELPHESREADERBASE_H
#define DELPHESREADERBASE_H

#include <iostream>
#include "ExRootTreeReader.h"
#include "Logging.h"
#include "Units.h"
#include "DelphesClasses.h"
#include <vector>
#include <string>

#include "HistCollection.h"

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TH1.h"



class DelphesReaderBase {
 public:
  DelphesReaderBase();
  ~DelphesReaderBase();

  inline ExRootTreeReader* Reader() { return m_reader; }

  int SetInput(TChain* c);
  int SetInput(std::string filename);
  int SetInput(std::vector<std::string> filenames);

  int ProcessEvents();
  virtual int ProcessEvent(float weight=1.) = 0; // must be implemented in derived class;
  virtual int BookHists();        // must be implemented in derived class;

  void EnableEvent    ()  {m_EVENT  = m_reader->UseBranch("Event");}
  void EnableJets     ()  {m_JET    = m_reader->UseBranch("Jet");}
  void EnableElectrons()  {m_ELEC   = m_reader->UseBranch("Electron");}
  void EnableMuons    ()  {m_MUON   = m_reader->UseBranch("Muon");}
  void EnablePhotons  ()  {m_PHOTO  = m_reader->UseBranch("Photon");}
  void EnableTracks   ()  {m_TRACKS = m_reader->UseBranch("Tracks");}
  void EnableMET      ()  {m_MET    = m_reader->UseBranch("MissingET");}
  void EnableTruth    ()  {m_PARTICLE = m_reader->UseBranch("Particle");}
  void EnableLeptons(){
    EnableElectrons();
    EnableMuons();
  }
  void EnableAll(){
    using namespace std;
    EnableEvent();
    EnableJets();
    EnableLeptons();
    EnablePhotons();
    EnableMET();
    EnableTruth();
  }

  inline int SetOutput(std::string name){ 
    if(m_outputFile){
      m_outputFile->Close();
      delete m_outputFile;
    }

    m_outputFile = new TFile(name.c_str(),"RECREATE");

    return 0;
  }

  int WriteOutput();

  inline int CloseOutput() { 
    if(m_outputFile){
      WriteOutput();
      m_outputFile->Close(); 
    }
    return 0;
  }

  inline unsigned int EventsProcessed() { return m_eventCounter; }

  const TClonesArray* Event    () { return m_EVENT;  }
  const TClonesArray* Jets     () { return m_JET;    }
  const TClonesArray* Electrons() { return m_ELEC;   }
  const TClonesArray* Muons    () { return m_MUON;   }
  const TClonesArray* Photons  () { return m_PHOTO;  }
  const TClonesArray* Tracks   () { return m_TRACKS; }
  const TClonesArray* MET      () { return m_MET;    }
  const TClonesArray* Particles() { return m_PARTICLE; }

  TFile* OutputFile() { return m_outputFile; }

  virtual void AddHistCollection(HistCollection* hc);
  virtual void AddOutputTree(TTree* tree);

  virtual void BookHistCollection(HistCollection* hc, std::string parent="");
  virtual void WriteHistCollection(HistCollection* hc, std::string parent="");

  virtual void WriteBinnings(HistCollection* hc);

  virtual void WriteObj(const TObject& obj, std::string directory="");
  virtual void WriteObj(const TObject* obj, std::string directory="");

  float GetEventWeight();

  void SetSampleScale(float scale) { m_sample_scale_factor = scale; }
  void ScaleByNEvents(bool s)      { m_scale_by_NEvents    = s    ; }
  void SetUnits(double units)      { m_units               = units; }

 protected:
  std::map<std::string, TH1*> m_hists;
  void ClearHists(bool delete_hists=false){
    if(delete_hists){
      for(std::map<std::string,TH1*>::iterator h_iter = m_hists.begin(); 
	  h_iter != m_hists.end(); ++h_iter){
	if(h_iter->second) delete h_iter->second;
      }
    }
    m_hists.clear();
  }

  double m_units;


 private:
  ExRootTreeReader* m_reader;
  TChain m_chain;

  unsigned int m_eventCounter;
  double m_weightCounter;

  TFile* m_outputFile;

  const TClonesArray *m_EVENT  ;
  const TClonesArray *m_JET    ;
  const TClonesArray *m_PHOTO  ;
  const TClonesArray *m_ELEC   ;
  const TClonesArray *m_MUON   ;
  const TClonesArray *m_TRACKS ;
  const TClonesArray *m_CALO   ;
  const TClonesArray *m_MET    ;
  const TClonesArray *m_PARTICLE;

  std::vector<HistCollection*> m_HistCollections;
  std::vector<TTree*> m_Trees;
  
  float m_sample_scale_factor;
  bool  m_scale_by_NEvents;

};
#endif
