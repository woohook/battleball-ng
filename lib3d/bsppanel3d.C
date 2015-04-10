// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include <iostream>
#include "bsppanel3d.h"

using namespace std;

/***************************************************************************/
bspPanel3d::bspPanel3d(const Polygon3f& poly, fastPts& pts,
		       const bspPanel3d& panel) {
  table<int> newPtNums;
  newPtNums.Add( (int) panel.doubleSided);

  forii(poly.numverts) {
    int ptNum= pts.Add(pt3d(poly.verts[i][0],
			    poly.verts[i][1],
			    poly.verts[i][2]));
    newPtNums.Add(ptNum);
  }
  newPtNums.Add(-1);
  int *ptNumsArray= newPtNums.Array();
  panel3d::Init(ptNumsArray,pts.Array());
}


/*-------------------------------------------------------------------------*/
Polygon3f bspPanel3d::asPolygon3f(pt3d pts[]) {
  Polygon3f poly;
  
  if (ptNums.Num() >MAXPOLYVERT) {
    cerr << "too many panel3d points (" << ptNums.Num()
	 << ") for a Polygon3f (max " << MAXPOLYVERT << ")." << endl;
    poly.numverts= 0;
  }
  else {
    poly.numverts= ptNums.Num();
    forii(ptNums.Num())
      pt3d2Vec3f(pts[ptNums[i]],poly.verts[i]);
  }

  pt3d2Vec3f(normal,poly.plane_normal);
  poly.material_index= (size_t) this;
  return poly;
}

