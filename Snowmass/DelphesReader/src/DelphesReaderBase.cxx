
#include <iomanip>

#include "Logging.h"
#include "DelphesReaderBase.h"

#include "TParameter.h"

using namespace std;

// ---------------------------------------------------------------------------------
// For verbose output, when we want it.
int PRINT_DEBUG=0;
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// Constructor.
//
DelphesReaderBase::DelphesReaderBase():
  m_units(GeV),
  m_reader(0),
  m_chain("Delphes"),
  m_outputFile(0),
  m_HistCollections(){
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// Destructor.
//
DelphesReaderBase::~DelphesReaderBase(){
  DEBUG("Closing output.");
  CloseOutput();

  DEBUG("Clearing histograms.");
  ClearHists();

  DEBUG("Cleaning reader.");
  if(m_reader) delete m_reader;
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// Setting the inputs
//
int DelphesReaderBase::SetInput(TChain* c){
  if(&m_chain != c){
    m_chain.Clear();
    m_chain.SetName("Delphes");
    m_chain.Add(c);
  }
  if(m_reader) delete m_reader;
  m_reader = new ExRootTreeReader(c);
  return 0;
}


int DelphesReaderBase::SetInput(std::string filename){
  m_chain.Clear();
  m_chain.SetName("Delphes");
  m_chain.AddFile(filename.c_str());
  SetInput(&m_chain);
  return 0;
}


int DelphesReaderBase::SetInput(std::vector<std::string> filenames){
  m_chain.Clear();
  m_chain.SetName("Delphes");
  for(unsigned int i=0; i<filenames.size(); i++){
    m_chain.AddFile(filenames[i].c_str());
  }
  SetInput(&m_chain);
  return 0;
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// The main event loop.  Note that the derived class should put something meaningful
// into "ProcessEvent"....
//
int DelphesReaderBase::ProcessEvents(){
  DEBUG("Booking Histograms");
  BookHists();
  DelphesReaderBase::BookHists();  


  DEBUG("Processing Events.");

  Int_t numberOfEntries = m_reader->GetEntries();

  if(numberOfEntries<=0) return -1;

  m_weightCounter=0;
  INFO("Going to process " << std::setw(6) << numberOfEntries << " events.");
  for(m_eventCounter=0; m_eventCounter < (unsigned int)numberOfEntries; ++m_eventCounter){
    m_reader->ReadEntry(m_eventCounter);
    
    float weight = GetEventWeight();

    float SF = m_sample_scale_factor;
    if(m_scale_by_NEvents){
      SF /= (float)numberOfEntries;
    }

    DEBUG("Weight = " << weight << ", SF = " << SF);
    
    ProcessEvent(weight*SF);

    if(((m_eventCounter+1) % (numberOfEntries/50)) == 0){
      INFO("...Processed " << std::setw(6) << m_eventCounter << " events, " << std::setw(3) << (int)(100.*m_eventCounter/numberOfEntries) << "% complete.");
    }

    m_weightCounter+=weight*SF;
  }
  return 0;
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// For writing histograms to a file.
//
int DelphesReaderBase::WriteOutput() { 
  if(m_outputFile){
    m_outputFile->cd();
    for(std::map<string,TH1*>::iterator h_iter = m_hists.begin(); 
	h_iter != m_hists.end(); ++h_iter){
      if(h_iter->second) h_iter->second->Write();
    }

    // write some TParameters
    TParameter<double> total_events("TotalProcessEvents",m_eventCounter);
    WriteObj(total_events);

    TParameter<double> total_weights("TotalProcessWeights",m_weightCounter);
    WriteObj(total_weights);
    
    // also write histogram collections
    for(unsigned int i=0; i<m_HistCollections.size(); i++){
      HistCollection* hc = m_HistCollections[i];
      WriteHistCollection(hc);
    }

    // and finally the trees
    for(std::vector<TTree*>::iterator tree_iter = m_Trees.begin();
	tree_iter != m_Trees.end(); ++tree_iter){
      (*tree_iter)->Write();
    }
    
  }
  return 0;
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// For adding histogram collections to the output
//
void DelphesReaderBase::BookHistCollection(HistCollection* hc, string parent){
  hc->SetBinning();

  hc->CreateHists();

  string directory;
  std::vector<HistBase*>* hists = hc->GetHists();
  for(unsigned int h=0; h<hists->size(); h++){
    HistBase* hb = (*hists)[h];
    
    for(std::map<int,TH1*>::iterator h_iter = hb->GetHists()->begin(); 
	h_iter != hb->GetHists()->end(); ++h_iter){
      if(parent == ""){
	directory=Form("%s/%s",hb->GetTag().c_str(),hb->GetName().c_str());
      }
      else{
	directory=Form("%s/%s/%s",parent.c_str(),hb->GetTag().c_str(),hb->GetName().c_str());
      }

      // check to see if the output directory exists
      if(m_outputFile){
	TDirectory* newdir=m_outputFile->GetDirectory(directory.c_str());
	if(!newdir)
	  newdir = m_outputFile->mkdir(directory.c_str());
	h_iter->second->SetDirectory(newdir);
      }
    }
  }

  // also loop over this collection's children:
  std::vector<HistCollection*>* children = hc->GetHistCollections();
  for(unsigned int j=0; j<children->size(); j++){
    if(parent==""){
      BookHistCollection((*children)[j],hc->GetName());
    }
    else{
      BookHistCollection((*children)[j],Form("%s/%s",parent.c_str(),hc->GetName().c_str()));
    }
  }

  // I need this for writing binnings....
  if(hists->begin() != hists->end()){
    if((*hists->begin())->GetHists()->begin() != (*hists->begin())->GetHists()->end()){
      TDirectory* histdir = (*hists->begin())->GetHists()->begin()->second->GetDirectory();
      hc->SetOutFileDirectory(histdir->GetMotherDir());
    }
  }
  else if(children->size()>0){
    std::vector<HistBase*>* child_hists = children->at(0)->GetHists();
    if(child_hists->begin() != child_hists->end()){
      if((*child_hists->begin())->GetHists()->begin() != (*child_hists->begin())->GetHists()->end()){
	TDirectory* histdir = (*child_hists->begin())->GetHists()->begin()->second->GetDirectory();
	hc->SetOutFileDirectory(histdir->GetMotherDir());
      }
    }    
  }
}

void DelphesReaderBase::WriteObj(const TObject* obj, string directory){
  if(directory!=""){
    TDirectory* newdir=m_outputFile->GetDirectory(directory.c_str());
    if(!newdir)
      newdir = m_outputFile->mkdir(directory.c_str());
    TDirectory* currentDir = gDirectory;
    newdir->cd();

    obj->Write();
    currentDir->cd();
  }
  else{
    obj->Write();
  }
  return;
}
void DelphesReaderBase::WriteObj(const TObject& obj, string directory){
  WriteObj(&obj, directory);
}

void DelphesReaderBase::WriteHistCollection(HistCollection* hc, string parent){
  hc->SetBinning();

  hc->CreateHists();

  string directory;
  std::vector<HistBase*>* hists = hc->GetHists();
  for(unsigned int h=0; h<hists->size(); h++){
    HistBase* hb = (*hists)[h];
    
    for(std::map<int,TH1*>::iterator h_iter = hb->GetHists()->begin(); 
	h_iter != hb->GetHists()->end(); ++h_iter){
      if(parent == ""){
	directory=Form("%s/%s",hb->GetTag().c_str(),hb->GetName().c_str());
      }
      else{
	directory=Form("%s/%s/%s",parent.c_str(),hb->GetTag().c_str(),hb->GetName().c_str());
      }

      // check to see if the output directory exists
      if(m_outputFile){
	TDirectory* newdir=m_outputFile->GetDirectory(directory.c_str());
	if(!newdir)
	  newdir = m_outputFile->mkdir(directory.c_str());
	TDirectory* currentDir = gDirectory;
	newdir->cd();
	h_iter->second->Write();
	currentDir->cd();
      }
    }
  }

  // also loop over this collection's children:
  std::vector<HistCollection*>* children = hc->GetHistCollections();
  for(unsigned int j=0; j<children->size(); j++){
    if(parent==""){
      WriteHistCollection((*children)[j],hc->GetName());
    }
    else{
      WriteHistCollection((*children)[j],Form("%s/%s",parent.c_str(),hc->GetName().c_str()));
    }
  }
}

int DelphesReaderBase::BookHists() {

  for(unsigned int i=0; i<m_HistCollections.size(); i++){
    HistCollection* hc = m_HistCollections[i];
    BookHistCollection(hc);
  }

  return 0;
}

void DelphesReaderBase::AddHistCollection(HistCollection* nhc){ 
  m_HistCollections.push_back(nhc); 
}

void DelphesReaderBase::AddOutputTree(TTree* tree){
  tree->SetAutoFlush(-30000000);
  m_Trees.push_back(tree);
}

void DelphesReaderBase::WriteBinnings(HistCollection* hc) {
  std::vector<WriteableBin> writeableBins = hc->GetWriteableBinnings();

  for(unsigned int i=0; i<writeableBins.size(); i++){
    ERROR("Code not set up for binnings yet.");
    //WriteObj(writeableBins[i].binvec,writeableBins[i].name.c_str());
  }
}
// ---------------------------------------------------------------------------------


// ---------------------------------------------------------------------------------
// Function to get the weight of the event.  Depending on how the events were generated,
// the weights may or may not exist.
// 
float DelphesReaderBase::GetEventWeight() {
  TIter eventIter(Event());
  eventIter.Reset();
  ::Event* ev = (::Event*)(eventIter.Next());
  if(ev){
    // figure out what kind of file this is.
    LHEFEvent* lhefev = dynamic_cast<LHEFEvent*>(ev);
    if(lhefev){
      return lhefev->Weight;
    }
    else{
      return 1.;
    }
  }
  return 1.;
}
// ---------------------------------------------------------------------------------
