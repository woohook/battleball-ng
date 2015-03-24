// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include "view.h"


/*=========================================================================*/
view& view::Copy(const view& v) {
  maxPts= numPts=		v.numPts;
  modelToView=			v.modelToView;
  shape=			v.shape;

  if (maxPts >0) {
    pts= new pt3d[maxPts];
    forii(numPts) pts[i]=v.pts[i];
  }

  return *this;
}


/*-------------------------------------------------------------------------*/
// In:  mtv= new modelToView transformation matrix
void view::Update(xregion3d *newShape, const tmtrx& newMtv) {

  // if the shape has changed and the new shape has more pts than
  // the old one, allocate an adequately large array of pts
  if (newShape != shape) {
    numPts= newShape->pts.Num();
    if (numPts >maxPts) {
      if (maxPts >0) delete [] pts;
      maxPts= numPts;
      pts= new pt3d[maxPts];
    }
  }
  
  if (newShape != shape || !modelToView.RotationSame(newMtv)) {
    newShape->TransformTo(newMtv,pts);
  }
  else {
    pt3d delta= newMtv.Cart() - modelToView.Cart();
    if (delta != pt3d(0,0,0)) {
      forii(newShape->pts.Num())
	pts[i] += delta;
    }
  }
  modelToView= newMtv;
  shape= newShape;
}
