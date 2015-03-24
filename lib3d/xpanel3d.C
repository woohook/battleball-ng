// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include "xpanel3d.h"



/************************************************************************/
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
  XFillPolygon(gt.disp,gt.win,gt.gc,xpts,numClippedPts,xShapeType,
	       CoordModeOrigin);
}
