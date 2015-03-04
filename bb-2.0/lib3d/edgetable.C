// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include "edgetable.h"


/************************************************************************/
//edgeTable::edgeTable(const edgeTable& t)	// copy constructor
//: table<edge>(t)
//{}


void edgeTable::AddPanel(const panel3d& panel) {
  bool	found;
  edge	e;

  forii(panel.ptNums.num -1)
  { found= 0;
    forij(num)
      if (((*this)[j].beginPtNum == panel.ptNums[i] and
           (*this)[j].endPtNum   == panel.ptNums[i+1]) or
          ((*this)[j].endPtNum   == panel.ptNums[i] and
           (*this)[j].beginPtNum == panel.ptNums[i+1])) found= 1;
    if (not found)
    { e.beginPtNum= panel.ptNums[i];
      e.endPtNum  = panel.ptNums[i+1];
      Add(e);
    }
  }
}

