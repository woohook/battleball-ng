// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.

#ifndef TRAIN_h
#define TRAIN_h

#include "gob.h"  // for gobList

extern gobList* g_track;

class Train
{
 public:
  Train();
  virtual ~Train();
  virtual void initialize(int argc, char** argv);
  virtual void processFrame(gobList& gobs);
};

#endif // TRAIN_h
