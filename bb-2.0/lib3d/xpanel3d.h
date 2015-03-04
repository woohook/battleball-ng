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
protected:
  int	xShapeType;		// type of polygon; equal to one of the
  				// X Windows constants Convex, Nonconvex, or
  				// Complex
  
 public:
  	xpanel3d() {};
        xpanel3d(int* &nums,pt3d pts[]);
  	xpanel3d(const Polygon3f&, fastPts&, const xpanel3d&);
  	xpanel3d(const region2d& rgn, fastPts& fp,
		 const pt3d& offset=pt3d(0,0,0), const ang3d& ang=ang3d(0),
		 const pt3d& scale =pt3d(1,1,1), int color=0, bool ds=false)
	  : bspPanel3d(rgn,fp,offset,ang,scale,ds)
	    {colorNum= color;
	     xShapeType= IsConvex()? Convex : Nonconvex;}
  	xpanel3d(const xpanel3d& p, pt3d panelPts[], fastPts& fp,
		 const pt3d& offset=pt3d(0,0,0), const ang3d& ang=ang3d(0),
		 const pt3d& scale =pt3d(1,1,1))
	  : bspPanel3d(p,panelPts,fp,offset,ang,scale)
	    {colorNum= p.colorNum;
	     xShapeType= IsConvex()? Convex : Nonconvex;}

 private:
  void	Init(int*&,pt3d[]);

 public:
  void	Draw(const pt3d viewPts[], const pt2d& clip, const pt2d prjPts[],
	     const ulong colors[], const gfxTarget& gt);
  void	DrawFast(const XPoint xprjPts[], const ulong colors[],
		 const gfxTarget& gt);
};


/*-------------------------------------------------------------------------*/
inline void xpanel3d::Init(int *&srcPtNums, pt3d pts[])
{ bspPanel3d::Init(srcPtNums,pts);
  colorNum= *srcPtNums++;
  xShapeType= IsConvex()? Convex : Nonconvex;
}

inline xpanel3d::xpanel3d(int* &nums,pt3d pts[]) {Init(nums,pts);}


/*-------------------------------------------------------------------------*/
inline xpanel3d::xpanel3d(const Polygon3f& poly, fastPts& pts,
		 const xpanel3d& panel) : bspPanel3d(poly,pts,panel)
{ colorNum= panel.colorNum;
  xShapeType= IsConvex()? Convex : Nonconvex;
}


/*-------------------------------------------------------------------------*/
// Draws xpanel without clipping it to the near viewing plane or to the
// edges of drawable
inline void xpanel3d::DrawFast(const XPoint xprjPts[], const ulong colors[],
			       const gfxTarget& gt) {
  static XPoint xpts[MAXPTNUMS];
  gt.SetForeground(colors[colorNum]);
  
  // 4-sided polygons are so common that we handle them as a special case
  // for greater speed.  Note that a 4-sided poly has _5_ ptNums, since the
  // last ptNum is redundant!
  if (ptNums.Num()==5) {
    xpts[0].x= xprjPts[ptNums[0]].x;
    xpts[0].y= xprjPts[ptNums[0]].y;
    xpts[1].x= xprjPts[ptNums[1]].x;
    xpts[1].y= xprjPts[ptNums[1]].y;
    xpts[2].x= xprjPts[ptNums[2]].x;
    xpts[2].y= xprjPts[ptNums[2]].y;
    xpts[3].x= xprjPts[ptNums[3]].x;
    xpts[3].y= xprjPts[ptNums[3]].y;
    XFillPolygon(gt.disp,gt.win,gt.gc,xpts,4,xShapeType,CoordModeOrigin);
    return;
  }

  int *ptNum= ptNums.Array();
  int *ptNumsEnd= ptNum +ptNums.Num();
  XPoint *xpt= xpts;
  while (ptNum != ptNumsEnd) {
    xpt->x= xprjPts[*ptNum].x;
    xpt->y= xprjPts[*ptNum].y;
    ptNum++;
    xpt++;
  }
  XFillPolygon(gt.disp,gt.win,gt.gc,xpts,ptNums.Num(),xShapeType,
	       CoordModeOrigin);
}


#endif
