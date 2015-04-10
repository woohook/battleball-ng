// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef XPANEL3D_h
#define XPANEL3D_h


#include "general.h"
#include "gfxtarget.h"
#include "bsppanel3d.h"

// the minimum viewable distance; objects any closer are clipped
#define MINDIST 1
#define MAXPTNUMS 100



/***************************************************************************/
struct xpanel3d : bspPanel3d {
  int	colorNum;		// Index into a table of X Colors (ulongs)
  
 public:
  	xpanel3d() {};
        xpanel3d(int* &nums,pt3d pts[]);
  	xpanel3d(const Polygon3f&, fastPts&, const xpanel3d&);
  	xpanel3d(const region2d& rgn, fastPts& fp,
		 const pt3d& offset=pt3d(0,0,0), const ang3d& ang=ang3d(0),
		 const pt3d& scale =pt3d(1,1,1), int color=0, bool ds=false)
	  : bspPanel3d(rgn,fp,offset,ang,scale,ds)
	    {colorNum= color;}
  	xpanel3d(const xpanel3d& p, pt3d panelPts[], fastPts& fp,
		 const pt3d& offset=pt3d(0,0,0), const ang3d& ang=ang3d(0),
		 const pt3d& scale =pt3d(1,1,1))
	  : bspPanel3d(p,panelPts,fp,offset,ang,scale)
	    {colorNum= p.colorNum;}

 private:
  void	Init(int*&,pt3d[]);

 public:
  void	Draw(const pt3d viewPts[], const pt2d& clip, const pt2d prjPts[],
	     const ulong colors[], const gfxTarget& gt);
  void	DrawFast(const pt2d prjPts[], const ulong colors[],
		 const gfxTarget& gt);
};


#endif
