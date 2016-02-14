// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.

#ifndef FLYBY_h
#define FLYBY_h

#include "gob.h"  // for gobList

class FlyBy
{
 public:
  FlyBy();
  virtual ~FlyBy();
  virtual void initialize(int argc, char** argv);
  virtual void processFrame(gobList& gobs);

 private:
  int mode;
  long cycles;
};

#endif // FLYBY_h
