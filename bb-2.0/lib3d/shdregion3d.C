// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include "shdregion3d.h"


/*=========================================================================*/
/* In:  exactFirst = true -> init rgns[0] to correspond exactly to firstPts &
                             firstScale (no morphing of rgns[0])
                     false-> init rgns[0] to be the first morphed shape,
                             corresponding most nearly to firstPts &
                             firstScale
        exactLast  = similar to exactFirst, but instead with respect to
                     rgns[numRgns-1], lastPts, & lastScale
*/
void InitMorphRegion3ds(shdRegion3d rgns[], int numRgns,
                        shdRegion3d::coord *firstPts,
			shdRegion3d::coord firstScale,
                        shdRegion3d::coord *lastPts,
			shdRegion3d::coord lastScale, int *panels,
                        bool exactFirst, bool exactLast)
{ table<shdRegion3d::coord> ptsData;

  forii(numRgns) {
    shdRegion3d::coord *pts1= firstPts;
    shdRegion3d::coord *pts2= lastPts;
    ptsData.Empty();
    while (*pts1 != COORD_MAX) {
      pt3d pt;
      pt.Interpolate(pt3d(pts1)*firstScale,
                     pt3d(pts2)*lastScale,
                     i +(not exactFirst),
                     numRgns -1 +(not exactFirst) +(not exactLast));
      ptsData.Add(pt.x);
      ptsData.Add(pt.y);
      ptsData.Add(pt.z);
    }
    ptsData.Add(COORD_MAX);
    rgns[i]= shdRegion3d(ptsData.Array(),panels);
  }
}
