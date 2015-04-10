// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include "xpanel3d.h"



/************************************************************************/

void xpanel3d::Init(int *&srcPtNums, pt3d pts[])
{ bspPanel3d::Init(srcPtNums,pts);
  colorNum= *srcPtNums++;
  xShapeType= IsConvex()? Convex : Nonconvex;
}

xpanel3d::xpanel3d(int* &nums,pt3d pts[]) {Init(nums,pts);}


/*-------------------------------------------------------------------------*/
xpanel3d::xpanel3d(const Polygon3f& poly, fastPts& pts,
		 const xpanel3d& panel) : bspPanel3d(poly,pts,panel)
{ colorNum= panel.colorNum;
  xShapeType= IsConvex()? Convex : Nonconvex;
}


/*-------------------------------------------------------------------------*/
// Draws xpanel without clipping it to the near viewing plane or to the
// edges of drawable
void xpanel3d::DrawFast(const XPoint xprjPts[], const ulong colors[],
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
    gt.FillPolygon(xpts,4,xShapeType,CoordModeOrigin);
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
  gt.FillPolygon(xpts,ptNums.Num(),xShapeType,CoordModeOrigin);
}

void xpanel3d::Draw(const pt3d viewPts[], const pt2d& clip,
		    const pt2d prjPts[], const ulong colors[],
		    const gfxTarget& gt){
  static pt2d pts1[3*MAXPTNUMS+1];
  static pt2d pts2[3*MAXPTNUMS+1];
  static XPoint xpts[3*MAXPTNUMS+1];
  int numClippedPts;
  
  Clip(viewPts,prjPts,pts1,MINDIST,clip/2,true,numClippedPts);
    
  // ClipPts could probably be rewritten to clip between _two_ planes,
  // instead of against only one, for better speed.  It might also be made
  // into a function template so that the last call to it would write the
  // destination points directly into xpts[]. -PAH
  
  // below, seems like we should clip things to (clip.x-1,clip.y-1), not
  // to (clip.x,clip.y), but that doesn't work.  Why not??? -PAH
  ClipPts(pts1,pts2,numClippedPts,0,     true, 0);
  ClipPts(pts2,pts1,numClippedPts,clip.x,false,0);
  ClipPts(pts1,pts2,numClippedPts,0,     true, 1);
  ClipPts(pts2,pts1,numClippedPts,clip.y,false,1);
  
  if (numClippedPts==0) return;

  forii(numClippedPts) {
    xpts[i].x= (short) pts1[i].x;
    xpts[i].y= (short) pts1[i].y;
  }
  gt.SetForeground(colors[colorNum]);
  gt.FillPolygon(xpts,numClippedPts,xShapeType,CoordModeOrigin);
}

