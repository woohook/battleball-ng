// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef REGION2D_h
#define REGION2D_h


#include <stdlib.h>   // to get NULL
#include "general.h"
#include "ang2d.h"
#include "pt2d.h"

extern const pt2d::coord COORD_MAX;



/************************************************************************/
struct side2d {
  typedef pt2d::coord coord;
  pt2d	pt;
  pt2d	normal;		// normal points right w.r.t. the vector
			// from pt to the next point in the region2d
  pt2d	unitNormal;	// unit normal of length 1.0
  coord	length;		// length of side (same as length of normal)
  
  void  Init(const pt2d&,const pt2d&,coord);
};


/************************************************************************/
// A region2d is a _closed_ sequence of line segments.

struct region2d {
  enum {maxPts= 20};
  typedef pt2d::coord coord;

  side2d sides[maxPts];
  int	 numSides;
  
       region2d(coord* =NULL,coord scale=1);
  void Init(coord*,coord scale);
  void Init(pt2d*,coord scale);
  void operator+=(const pt2d&);
  void operator*=(coord);
  void TransformTo(const pt2d& offset, ang2d ang, const pt2d& sc,
		   pt2d destPts[]);
  void CheckIfContains(const pt2d&,coord&,side2d*&);
};


#endif
