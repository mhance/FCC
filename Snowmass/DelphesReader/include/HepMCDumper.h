#ifndef HepMCDUMPER_H
#define HepMCDUMPER_H

#include <fstream>

#include "DelphesReaderBase.h"

class HepMCDumper: public DelphesReaderBase{
 public:
  HepMCDumper();
  ~HepMCDumper();

  int ProcessEvent(float weight);
  int BookHists() {return 0;};

  void SetOutputFileName(std::string name) { m_outputFileBase = name; }

 private:

  int m_processedEvents;
  int m_outputFiles;
  std::ofstream m_HepMCfile;
  std::string m_outputFileBase;

};

#endif
