// Dear emacs, this is -*- c++ -*-

#ifndef BJET_CANDIDATE_H
#define BJET_CANDIDATE_H

#include "JetCandidate.h"

class BJetCandidate: public JetCandidate {
 public:
  BJetCandidate():
    JetCandidate(),
    m_score(0){
  }

  BJetCandidate(TLorentzVector& v):
    JetCandidate(v),
    m_score(0){
  }

  BJetCandidate(int i):
    JetCandidate(i),
    m_score(0){
  }

  BJetCandidate(TLorentzVector& v, int i):
    JetCandidate(v,i),
    m_score(0){
  }

  BJetCandidate(JetCandidate& jet, float score):
    JetCandidate(jet),
    m_score(score){
  }

  ~BJetCandidate(){};

  float Score() { return m_score; }
  void  SetScore(float score) { m_score = score; }

private:
  float m_score;
};

#endif
