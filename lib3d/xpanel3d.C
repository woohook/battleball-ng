// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include "xpanel3d.h"



/************************************************************************/

void xpanel3d::Init(int *&srcPtNums, pt3d pts[])
{
  bspPanel3d::Init(srcPtNums,pts);
  colorNum= *srcPtNums++;
}

xpanel3d::xpanel3d(int* &nums,pt3d pts[]) {Init(nums,pts);}


/*-------------------------------------------------------------------------*/
xpanel3d::xpanel3d(const Polygon3f& poly, fastPts& pts,
		 const xpanel3d& panel) : bspPanel3d(poly,pts,panel)
{
  colorNum= panel.colorNum;
}


/*-------------------------------------------------------------------------*/
// Draws xpanel without clipping it to the near viewing plane or to the
// edges of drawable
void xpanel3d::DrawFast(const pt2d prjPts[], const ulong colors[],
			       const gfxTarget& gt) {
  static pt2d xpts[MAXPTNUMS];
  gt.SetForeground(colors[colorNum]);
  
  // 4-sided polygons are so common that we handle them as a special case
  // for greater speed.  Note that a 4-sided poly has _5_ ptNums, since the
  // last ptNum is redundant!
  if (ptNums.Num()==5) {
    xpts[0].x= prjPts[ptNums[0]].x;
    xpts[0].y= prjPts[ptNums[0]].y;
    xpts[1].x= prjPts[ptNums[1]].x;
    xpts[1].y= prjPts[ptNums[1]].y;
    xpts[2].x= prjPts[ptNums[2]].x;
    xpts[2].y= prjPts[ptNums[2]].y;
    xpts[3].x= prjPts[ptNums[3]].x;
    xpts[3].y= prjPts[ptNums[3]].y;
    gt.FillPolygon(xpts,4,IsConvex());
    return;
  }

  int *ptNum= ptNums.Array();
  int *ptNumsEnd= ptNum +ptNums.Num();
  pt2d *xpt= xpts;
  while (ptNum != ptNumsEnd) {
    xpt->x= prjPts[*ptNum].x;
    xpt->y= prjPts[*ptNum].y;
    ptNum++;
    xpt++;
  }
  gt.FillPolygon(xpts,ptNums.Num(),IsConvex());
}

void xpanel3d::Draw(const pt3d viewPts[], const pt2d& clip,
		    const pt2d prjPts[], const ulong colors[],
		    const gfxTarget& gt){
  static pt2d pts1[3*MAXPTNUMS+1];
  static pt2d pts2[3*MAXPTNUMS+1];
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

  gt.SetForeground(colors[colorNum]);
  gt.FillPolygon(pts1,numClippedPts,IsConvex());
}

