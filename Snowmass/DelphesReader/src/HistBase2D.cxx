// Dear emacs, this is -*- c++ -*-

#include <iostream>
#include <iomanip>
#include <cmath>
#include "HistBase2D.h"
#include "TROOT.h"

using namespace std;

HistBase2D::HistBase2D(std::string name, std::string tag, 
		       int nbinsx, float lowx, float highx, 
		       int nbinsy, float lowy, float highy, 
		       std::string title, float scale) :
  HistBase(name, tag, title, scale),
  m_n_bins_x(nbinsx),
  m_bin_low_x(lowx),
  m_bin_high_x(highx),
  m_n_bins_y(nbinsy),
  m_bin_low_y(lowy),
  m_bin_high_y(highy){

  // make the first hist, somehow
  string th2_name=Form("h_%s_%s",GetTag().c_str(),GetName().c_str());
  TH2F* hist = (TH2F*)gROOT->FindObject(th2_name.c_str());
  if(!hist)
    (*GetHists())[0] = new TH2F(th2_name.c_str(),
				GetTitle().c_str(), 
				nbinsx, lowx, highx,
				nbinsy, lowy, highy);
  else
    (*GetHists())[0] = hist;
  
}

HistBase2D::HistBase2D(std::string name, std::string tag, 
		       int nbinsx, double* xbins,
		       int nbinsy, double* ybins,
		       std::string title, float scale) :
  HistBase(name, tag, title, scale),
  m_n_bins_x(nbinsx),
  m_bin_low_x(xbins[0]),
  m_bin_high_x(xbins[nbinsx-1]),
  m_n_bins_y(nbinsx),
  m_bin_low_y(ybins[0]),
  m_bin_high_y(ybins[nbinsy-1]){

  // make the first hist, somehow
  string th2_name=Form("h_%s_%s",GetTag().c_str(),GetName().c_str());
  TH2F* hist = (TH2F*)gROOT->FindObject(th2_name.c_str());
  if(!hist)
    (*GetHists())[0] = new TH2F(th2_name.c_str(),
				GetTitle().c_str(), 
				nbinsx, xbins,
				nbinsy, ybins);
  else
    (*GetHists())[0] = hist;
  
}

HistBase2D::~HistBase2D(){}

void HistBase2D::Clear(){
  int nhists = GetHists()->size();
  for(int i=0; i<nhists; i++){
    GetHists()->at(i)->Clear();
  }
}
