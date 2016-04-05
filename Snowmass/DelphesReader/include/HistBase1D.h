// Dear emacs, this is -*- c++ -*-

#ifndef COMMON_HISTBASE1D_H
#define COMMON_HISTBASE1D_H

#include <string>
#include <vector>
#include <map>
#include <cmath>

#include "HistBase.h"

class HistBase1D : public HistBase {
 public:

  HistBase1D(std::string name, std::string tag, 
	     int nbins=0, float low=0, float high=0, 
	     std::string title="", float scale=1.);
  virtual ~HistBase1D();

  virtual HistBase1D* Clone(std::string) = 0;

  virtual void Fill(int iter, std::vector<int> &binnos, float weight) = 0;
  virtual void Fill(int iter, int hash, float weight) = 0;
  virtual void Fill(int hash, float weight) = 0;

  inline int GetNBins() { return m_n_bins; };
  inline float GetBinLow() { return m_bin_low; };
  inline float GetBinHigh() { return m_bin_high; };

  virtual void Clear();

 private:
  
  int m_n_bins;
  float m_bin_low;
  float m_bin_high;

};

#endif
