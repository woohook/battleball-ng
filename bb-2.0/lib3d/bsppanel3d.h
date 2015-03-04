// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef BSPPANEL3D_h
#define BSPPANEL3D_h


#include "polygon.h"   // from the bsp library
#include "general.h"
#include "panel3d.h"
#include "fastpts.h"



/************************************************************************/
struct bspPanel3d : panel3d {

  // no additional data members

 	bspPanel3d() {};
  	bspPanel3d(int* &nums,pt3d pts[]) : panel3d(nums,pts) {};
  	bspPanel3d(const Polygon3f&, fastPts&, const bspPanel3d&);
  	bspPanel3d(const region2d& rgn, fastPts& fp,
		   const pt3d& offset=pt3d(0,0,0), const ang3d& ang=ang3d(0),
		   const pt3d& scale =pt3d(1,1,1), bool ds=0)
	  : panel3d(rgn,fp,offset,ang,scale,ds) {};
  	bspPanel3d(const bspPanel3d& p, pt3d panelPts[],
		   fastPts& fp,
		   const pt3d& offset=pt3d(0,0,0), const ang3d& ang=ang3d(0),
		   const pt3d& scale =pt3d(1,1,1))
	  : panel3d(p,panelPts,fp,offset,ang,scale) {};

  Polygon3f asPolygon3f(pt3d pts[]);
};


/*-------------------------------------------------------------------------*/
inline void pt3d2Vec3f(const pt3d& p, Vec3f& v)
{ Vset3f((float) p.x, (float) p.y, (float) p.z, v);
}

#endif
