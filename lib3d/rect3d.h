// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef RECT3D_h
#define RECT3D_h


#include "general.h"
#include "pt3d.h"


/************************************************************************/
/* pnt   = N-dimensional point type which delimits the bounds of the rect
   pnts  = container of pts
*/
template <class pnt, class pnts>
struct rect {
  pnt	low,
  	high;
  bool	Contains(const pnt& p) const
        {return p.IsBetween(low,high);};
  bool	ContainsInclusively(const pnt& p) const
        {return p.IsBetweenInclusively(low,high);};
  bool	ContainsExclusively(const pnt& p) const
        {return p.IsBetweenExclusively(low,high);};
//  Center used to return "const pnt& "
  pnt	Center() const
        {return (low+high)/2;};

  void	MakeBoundingBox(const pnts& pts, double & farthestDist) {
    double d;
    
    farthestDist= 0;
    if (pts.Num()==0) {
      low= pnt(0,0,0);
      high= pnt(0,0,0);
      return;
    }

    low= pts[0];
    high= pts[0];
    
    forii(pts.Num()) {
      d= (double) pts[i].Dist();
      if (d >farthestDist)
	farthestDist= d;
      low.SetMin(pts[i]);
      high.SetMax(pts[i]);
    }
  };

};


typedef rect<pt3d,table<pt3d> > rect3d;

#endif
