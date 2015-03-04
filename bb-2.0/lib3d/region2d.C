// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include "region2d.h"


/************************************************************************/
void side2d::Init(const pt2d& pt0, const pt2d& pt1, coord scale)
{ pt= pt0 * scale;
  // to get normal, take vector from pt0 to pt1 and rotate -90 deg 
  normal= (pt1 -pt0).Right90();
  length= normal.Dist();
  unitNormal= normal;
  unitNormal.Normalize(1);
}



/************************************************************************/
// The number of srcPts should be equal to the number of points (and the
// number of sides) in the region2d.  In other words, region2d's are self-
// closing; the last src pt need not be, and should not be, the same as the
// first src pt in order to close the region2d

region2d::region2d(coord *srcPts, coord scale) {
  Init(srcPts,scale);
}


/*-------------------------------------------------------------------------*/
void region2d::Init(coord *srcPts, coord scale) {
  numSides= 0;

  if (srcPts) {
    pt2d firstPt(srcPts);
    pt2d pt= firstPt;
    
    while (*srcPts != COORD_MAX) {
      pt2d prevPt= pt;
      pt= pt2d(srcPts);
      sides[numSides++].Init(prevPt,pt,scale);
    }
    sides[numSides++].Init(pt,firstPt,scale);
  }
}


/*-------------------------------------------------------------------------*/
void region2d::Init(pt2d *srcPts, coord scale) {
  numSides= 0;

  if (srcPts) {
    pt2d firstPt(srcPts);
    pt2d pt= firstPt;
    
    while (srcPts->x != COORD_MAX) {
      pt2d prevPt= pt;
      pt= pt2d(srcPts);
      sides[numSides++].Init(prevPt,pt,scale);
    }
    sides[numSides++].Init(pt,firstPt,scale);
  }
}


/*-------------------------------------------------------------------------*/
void region2d::operator+=(const pt2d& offset) {
  forii(numSides)
    sides[i].pt += offset;
}


void region2d::operator*=(coord scale) {
  forii(numSides)
    sides[i].pt *= scale;
}


void region2d::TransformTo(const pt2d& offset, ang2d ang, const pt2d& sc,
			   pt2d destPts[]) {
  forii(numSides) {
    pt2d p= sides[i].pt;
    p *= sc;
    destPts[i]= (p >> ang) + offset;
  }
}


// In:	pt       = point which this region2d may contain
// Out:	dist <=0 = neg. dist to nearest side; region does contain pt
//	      >0 = dist to a side which faces pt; region does not contain pt
//      s        = side
void region2d::CheckIfContains(const pt2d& pt, coord& dist, side2d*& s) {
  coord	nearestInsideDist;
  side2d	*nearestInsideSide;
  
  s= &sides[0];
  dist= ((pt - s->pt).Dot(s->normal)) / s->length;
  if (dist >0) return;

  nearestInsideDist= dist;
  nearestInsideSide= s;
  for(int i=1; i <numSides; i++)
  { s= &sides[i];
    dist= ((pt - s->pt).Dot(s->normal)) / s->length;
    if (dist >0) return;
    if (dist >nearestInsideDist) nearestInsideDist= dist;
  }
  dist= nearestInsideDist;
  s=    nearestInsideSide;
}
