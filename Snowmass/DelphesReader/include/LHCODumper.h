#ifndef LHCODUMPER_H
#define LHCODUMPER_H

#include <fstream>

#include "DelphesReaderBase.h"

class LHCODumper: public DelphesReaderBase{
 public:
  LHCODumper();
  ~LHCODumper();

  int ProcessEvent(float weight);
  int BookHists() {return 0;};

  void SetOutputFileName(std::string name) { m_outputFileBase = name; }

 private:

  int m_processedEvents;
  int m_outputFiles;
  std::ofstream m_LHCOfile;
  std::string m_outputFileBase;

};

#endif
