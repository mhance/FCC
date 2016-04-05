// Dear emacs, this is -*- c++ -*-

#ifndef COMMON_HISTBASE_H
#define COMMON_HISTBASE_H

#include <string>
#include <vector>
#include <map>
#include <cmath>

#include "TH1.h"
#include "TH2.h"

#include "BinCollection.h"

class HistBase {
 public:

  HistBase(std::string name, std::string tag, std::string title="", float scale=1.);
  virtual ~HistBase();

  virtual HistBase* Clone(std::string) = 0;

  virtual void Clear() = 0;

  virtual TH1* GetHist(std::vector<int> &binnos);
  virtual TH1* GetHist(int hash);

  virtual void Fill(int iter, std::vector<int> &binnos, float weight) = 0;
  virtual void Fill(int iter, int hash, float weight) = 0;
  virtual void Fill(int hash, float weight) = 0;

  virtual void SetBinnings(BinCollection* binnings);

  virtual std::map<int,TH1*>* GetHists() { return &m_histmap; }

  std::string GetName()  { return m_name; }
  std::string GetTag()   { return m_tag; }
  std::string GetTitle() { return m_title; }

  void SetName(std::string name)   { m_name = name; }
  void SetTag(std::string tag)     { m_tag = tag; }
  void SetTitle(std::string title) { m_title = title; }

  float GetScale() { return m_scale; }

  double* GetXbins() { 
    return ((double*)(m_histmap.begin()->second->GetXaxis()->GetXbins()->GetArray())); 
  }
  double* GetYbins() { 
    if(dynamic_cast<TH2*>(m_histmap.begin()->second))
      return (double*)(m_histmap[0]->GetXaxis()->GetXbins()->GetArray()); 
    else
      return 0;
  }


 private:
  
  BinCollection* m_binnings;

  std::map<int,TH1*> m_histmap;

  std::string m_name;

  std::string m_tag;

  int m_n_bins;
  float m_bin_low;
  float m_bin_high;

  std::string m_title;

  float m_scale;
};

#endif
