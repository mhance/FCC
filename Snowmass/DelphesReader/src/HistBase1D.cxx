// Dear emacs, this is -*- c++ -*-

#include "HistBase1D.h"
#include "TROOT.h"

using namespace std;

HistBase1D::HistBase1D(std::string name, std::string tag, int nbins, float low, float high, std::string title, float scale) :
  HistBase(name, tag, title, scale),
  m_n_bins(nbins),
  m_bin_low(low),
  m_bin_high(high){

  // make the first hist, somehow
  string th1_name=Form("h_%s_%s",GetTag().c_str(),GetName().c_str());
  TH1F* hist = (TH1F*)gROOT->FindObject(th1_name.c_str());
  if(!hist){
    (*GetHists())[0] = new TH1F(th1_name.c_str(), 
				GetTitle().c_str(), 
				nbins, low, high);
  }
  else{
    (*GetHists())[0] = hist;
  }
}

HistBase1D::~HistBase1D(){}

void HistBase1D::Clear(){
  int nhists = GetHists()->size();
  for(int i=0; i<nhists; i++){
    GetHists()->at(i)->Clear();
  }
}
