#ifndef COMMON_BINBASE_H
#define COMMON_BINBASE_H

#include <iostream>
#include <cmath>
#include <vector>
#include <string>

#include "TVector.h"

// some arbitrarily large number
#define MAX_BINS 256

class BinBase {
 public:
  BinBase(std::string name, std::vector<float> bin_bounds, float scale=1.) :
    m_bin_bounds(bin_bounds),
    m_name(name),
    m_isAbsValue(false),
    m_scale(scale){
  };
    
    virtual ~BinBase(){};
    
  int bin(float val){
    if(m_isAbsValue) val = std::fabs(val);
    for(unsigned int i=0; i<m_bin_bounds.size()-1; i++){
      if(val >= m_bin_bounds[i] && val < m_bin_bounds[i+1]) return i;
    }
    if(val >= m_bin_bounds[m_bin_bounds.size()-1]) return m_bin_bounds.size()-1;
    return -1;
  }

  int GetBinNumber(float val) { return bin(val); }
  int GetNBins() { return m_bin_bounds.size(); }

  void SetBinCalculator (int (*binCalculator)(void*)){
    m_binCalculator = binCalculator;
  }

  std::string GetName() { return m_name; }
  
  void SetAbsValue(bool useabsval=true) { m_isAbsValue=useabsval; }

  // MH: I think I can take this out....
  //void WriteBins(){
  //  GetTVectorBins().Write(Form("%s_binning",m_name.c_str()));
  //}

  TVector GetTVectorBins(){
    TVector v(m_bin_bounds.size());
    for(unsigned int i=0; i<m_bin_bounds.size(); i++) v[i]=m_bin_bounds[i];
    return v;
  }

  float GetScale() { return m_scale; }

 private:
  std::vector<float> m_bin_bounds;
  std::string m_name;

  int (*m_binCalculator)(void*);

  bool m_isAbsValue;

  float m_scale;
};

#endif
