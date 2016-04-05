// Dear emacs, this is -*- c++ -*-
#ifndef PARTICLEHISTS_H
#define PARTICLEHISTS_H

#include <string>
#include <vector>

#include "Hist.hh"
#include "Hist2D.hh"
#include "HistCollection.h"

#include "TLorentzVectorHists.h"

#include "ParticleCandidate.h"

// Don't need this if I'm going to make an instance in a .cxx file.
// template class ::Hist2D<float,float>;

class ParticleHists : public TLorentzVectorHists {
    
public:
  ParticleHists(std::string name);
    
  virtual ~ParticleHists();
    
  void ConnectTo(ParticleCandidate* particle) {
    m_particle = *particle;  
    SetTLorentzVector(m_particle);
    return;
  }
    
  void Fill(ParticleCandidate* particle, float weight){
    if(particle){
      ConnectTo(particle);
      Fill(weight);
    }
    return;
  }

  void Fill(float weight);
  void Fill(int iter, float weight);

  void CreateHists();
    
  virtual void SetHistsCreated(bool flag=true){ m_hists_created = flag;}
  virtual bool GetHistsCreated()              { return m_hists_created; }

  static void SetHistDetailLevel(int level) { m_HistDetailLevel = level; }
  void SetLocalHistDetailLevel(int level) { m_LocalHistDetailLevel = level; }

private:

  bool m_hists_created;

  static int m_HistDetailLevel;
  int m_LocalHistDetailLevel;

  ParticleCandidate m_particle;

  bool m_bins_set;

};
#endif
