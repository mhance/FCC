// Dear emacs, this is -*- c++ -*-

#ifndef COMMON_HISTBASE2D_H
#define COMMON_HISTBASE2D_H

#include <string>
#include <vector>
#include <map>
#include <cmath>

#include "TH2.h"

#include "BinCollection.h"

#include "HistBase.h"

class HistBase2D : public HistBase {
 public:

  HistBase2D(std::string name, std::string tag, 
	     int nbinsx=0, float lowx=0, float highx=0, 
	     int nbinsy=0, float lowy=0, float highy=0, 
	     std::string title="", float scale=1.);
  HistBase2D(std::string name, std::string tag, 
	     int nbinsx, double* xbins,
	     int nbinsy, double* ybins,
	     std::string title="", float scale=1.);
  virtual ~HistBase2D();

  virtual HistBase2D* Clone(std::string) = 0;

  virtual void Fill(int iter, std::vector<int> &binnos, float weight) = 0;
  virtual void Fill(int iter, int hash, float weight) = 0;
  virtual void Fill(int hash, float weight) = 0;

  inline int GetNBinsX() { return m_n_bins_x; }
  inline float GetBinLowX() { return m_bin_low_x; }
  inline float GetBinHighX() { return m_bin_high_x; }
  inline int GetNBinsY() { return m_n_bins_y; }
  inline float GetBinLowY() { return m_bin_low_y; }
  inline float GetBinHighY() { return m_bin_high_y; }

  virtual void Clear();

 private:
  
  int m_n_bins_x;
  float m_bin_low_x;
  float m_bin_high_x;

  int m_n_bins_y;
  float m_bin_low_y;
  float m_bin_high_y;
};

#endif
