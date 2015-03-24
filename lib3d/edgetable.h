// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef EDGETABLE_h
#define EDGETABLE_h


#include "table.h"
#include "edge.h"
#include "panel3d.h"


/************************************************************************/
typedef table<edge> baseEdgeTable;

struct edgeTable : baseEdgeTable {

  // no additional data members

 	//edgeTable(const edgeTable&);		// copy constructor
  void	AddPanel(const panel3d&);
};


#endif
