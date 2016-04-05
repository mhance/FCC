// Dear emacs, this is -*- c++ -*-

#ifndef COMMON_MYHIST_H
#define COMMON_MYHIST_H

#include <string>
#include <vector>
#include "HistBase1D.h"

template <typename T>
class Hist : public HistBase1D {
public:
  Hist(T* var, 
       std::string name, 
       int nbins, float low, float high, 
       std::string tag="", std::string title="", 
       float scale=1.);
  virtual ~Hist(){};
  
  virtual Hist<T>* Clone(std::string);

  void Fill(int iter, int hash, float weight);
  void Fill(int iter, std::vector<int> &binnos, float weight);
  void Fill(int hash, float weight);
  
  T* GetVar() { return m_var; }

private:
  T* m_var;    
};
#endif

#ifdef INCLUDE_TEMPLATES
#include "Hist.icc"
#endif
