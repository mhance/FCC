// Dear emacs, this is -*- c++ -*-

#ifndef COMMON_BIN_H
#define COMMON_BIN_H

#include <vector>
#include <string>

#include "BinBase.h"

template<typename T> class Bin : public BinBase {
public:

  // FIXME: use T instead of float in vector definition
  Bin(std::string name, std::vector<float> bounds, T* var, float scale=1.);
  ~Bin(){};

  int GetBinNumber();
  int GetBinNumber(int iter);
    
private:
  T* m_var;
    
};
#endif

#ifdef INCLUDE_TEMPLATES
#include "Bin.icc"

#endif
