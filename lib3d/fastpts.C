// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include "fastpts.h"



/*=========================================================================*/
// Returns the index of the point added
FASTPTS_TEMPL_DECL
int FASTPTS::Add(const pnt& pt) {
  pt3dPtr ptPtr;
  bool    wasAbsent;

  int ptNum= pts.AddIfAbsent(pt,wasAbsent);
  if (wasAbsent)
  { ptPtr.x= xdims.Add(pt.x,sdims);
    ptPtr.y= ydims.Add(pt.y,sdims);
    ptPtr.z= zdims.Add(pt.z,sdims);
    ptPtrs.Add(ptPtr);
  }
  return ptNum;
}


/*-------------------------------------------------------------------------*/
// Add all points from 'newPts'
FASTPTS_TEMPL_DECL
void FASTPTS::Add(const pntTable& newPts) {
  forii(newPts.Num())
    Add(newPts[i]);
}


/*-------------------------------------------------------------------------*/
// This could easily be templatized to work w/ either tmtrx or double[4][4]
FASTPTS_TEMPL_DECL
void FASTPTS::TransformTo(const tmtrx& m, destPnt destPts[]) {
  // compute the partial products for values in each dimension.
  xdims.MakePartialProds(m[0]);
  ydims.MakePartialProds(m[1]);
  zdims.MakePartialProds(m[2]);
  sdims.MakePartialProds();

  // rotate points by adding the appropriate partial products
  forii(ptPtrs.Num())
    destPts[i]= ptPtrs[i].x->prod +
                ptPtrs[i].y->prod +
                ptPtrs[i].z->prod +pt3d(m[3]);

  /*
  pt3dPtr* ptPtr= ptPtrs.Array();
  pt3dPtr* ptPtrsEnd= ptPtr + ptPtrs.Num();

  while (ptPtr != ptPtrsEnd) {
    *destPts++= ptPtr->x->prod +
                ptPtr->y->prod +
                ptPtr->z->prod; // +pt3d(m[3]);
    ptPtr++;
  }
  */

}


/*-------------------------------------------------------------------------*/
FASTPTS_TEMPL_DECL
FASTPTS&
  FASTPTS::operator=(const FASTPTS& fp) {
  pts.Empty();
  ptPtrs.Empty();
  xdims.Empty();
  ydims.Empty();
  zdims.Empty();
  sdims.Empty();
  Add(fp.pts);
  return *this;
}


/*-------------------------------------------------------------------------*/
FASTPTS_TEMPL_DECL
ostream& operator<<(ostream& out, const FASTPTS& fp) {
  int dimProps[dimension::numDimProps];

  out << "Points:" << fp.pts.Num()
      << "   xdims:" << fp.xdims.size()
      << "   ydims:" << fp.ydims.size()
      << "   zdims:" << fp.zdims.size() << "\n";

  forii(dimension::numDimProps) dimProps[i]= 0;
  fp.xdims.CountProps(dimProps);
  fp.ydims.CountProps(dimProps);
  fp.zdims.CountProps(dimProps);
  out << "non-0 dims:" << fp.xdims.size() +fp.ydims.size() +fp.zdims.size();
  out << "  unique:" << dimProps[0] << "  symmetric:" << dimProps[1]
      << "  half:" << dimProps[2] << "  quarter:" << dimProps[3];
  return out << "\n";
}
