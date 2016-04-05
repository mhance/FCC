// Dear emacs, this is -*- c++ -*-

#ifndef COMMON_MYHIST2D_H
#define COMMON_MYHIST2D_H

#include <string>
#include <vector>
#include "HistBase2D.h"

template <typename Tx, typename Ty>
class Hist2D : public HistBase2D {
public:
  Hist2D(Tx* varx, Ty* vary,
	 std::string name, 
 	 int nbinsx, float lowx, float highx, 
	 int nbinsy, float lowy, float highy, 
	 std::string tag="", std::string title="", 
	 float scale=1.) :
    HistBase2D(name, tag, nbinsx, lowx, highx, nbinsy, lowy, highy, title, scale),
    m_varX(varx),
    m_varY(vary){
  }

  Hist2D(Tx* varx, Ty* vary,
	 std::string name, 
 	 int nbinsx, double* xbins,
	 int nbinsy, double* ybins,
	 std::string tag="", std::string title="", 
	 float scale=1.) :
    HistBase2D(name, tag, nbinsx, xbins, nbinsy, ybins, title, scale),
    m_varX(varx),
    m_varY(vary){
  }

  virtual ~Hist2D(){};
  
  virtual Hist2D<Tx,Ty>* Clone(std::string newtag){
    return new Hist2D<Tx,Ty>(m_varX, m_varY,
			     this->GetName(),
			     this->GetNBinsX(),
			     this->GetXbins(),
			     this->GetNBinsY(),
			     this->GetYbins(),
			     newtag,
			     this->GetTitle(),
			     this->GetScale());
  }

  void Fill(int iter, int hash, float weight){
    return Fill(hash, weight);
  }
  void Fill(int iter, std::vector<int> &binnos, float weight) {
    ((TH2*)GetHist(binnos))->Fill(*m_varX*GetScale(), 
				  *m_varY*GetScale(),
				  weight);
  }
  void Fill(int hash, float weight){
    ((TH2*)GetHist(hash))->Fill(*m_varX*GetScale(), 
				*m_varY*GetScale(),
				weight);
  }
  
  Tx* GetVarX() { return m_varX; }
  Ty* GetVarY() { return m_varY; }

private:
  Tx* m_varX;    
  Ty* m_varY;    
};
#endif

#ifdef INCLUDE_TEMPLATES
// this seems to cause all sorts of problems with linking....
//#include "Hist2D.icc"
#endif
