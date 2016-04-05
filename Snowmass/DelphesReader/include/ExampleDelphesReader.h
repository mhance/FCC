#ifndef EXAMPLEDELPHESREADER_H
#define EXAMPLEDELPHESREADER_H

#include "DelphesReaderBase.h"

class ExampleDelphesReader: public DelphesReaderBase{
 public:
  ExampleDelphesReader();
  ~ExampleDelphesReader();

  int ProcessEvent(float weight);
  int BookHists();

 private:

};

#endif
