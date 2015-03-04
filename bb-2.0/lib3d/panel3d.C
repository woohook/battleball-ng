// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include "panel3d.h"



/*=========================================================================*/
void panel3d::SetNormalAndConvexity(const pt3d pts[]) {
  isConvex= true;

  if (ptNums.Num() >=3) {

    // compute normal
    // use Newell's formulation (Foley, Van Dam, Feiner, & Hughes, pp. 477)

    normal= pt3d(0,0,0);
    // Remember, ptNums[0] is the same as ptNums[ptNums.Num()-1],
    // so we have to be careful below:
    int j= ptNums.Num()-2;
    forii(ptNums.Num()-1) {
      const pt3d& pt1= pts[ptNums[j]];
      const pt3d& pt2= pts[ptNums[i]];
      normal.x += (pt1.y - pt2.y) *(pt1.z + pt2.z);
      normal.y += (pt1.z - pt2.z) *(pt1.x + pt2.x);
      normal.z += (pt1.x - pt2.x) *(pt1.y + pt2.y);
      j =i;
    }
    normal.Normalize();
    
    // Remember, ptNums[0] is the same as ptNums[ptNums.Num()-1],
    // so we have to be careful below:
    fori(ptNums.Num()-1) {
      j= (i+1) % (ptNums.Num()-1);  // index of second point number
      int k= (i+2) % (ptNums.Num()-1);  // index of third point number
      // compute the normal for this particular sequence of three points
      pt3d norm3= (pts[ptNums[j]]-pts[ptNums[i]]).CrossProd(
		   pts[ptNums[k]]-pts[ptNums[j]]);
      // if the normal for these 3 pts is in the opposite direction as
      // the official normal for the polygon, then either these 3 pts form
      // a concave bend, so we know that this polygon is _not_ convex!
      if (norm3.Dot(normal) <0)
	isConvex= false;
    }
  }
}


/*-------------------------------------------------------------------------*/
/* In:  srcPtNums = array of point numbers which define this panel,
                    terminated by a negative value
   Out: srcPtNums = points one element past the terminating negative value
   
   Reads an initial number that indicates if the panel is single-sided
   (zero) or double-sided (non-zero).  Then reads point numbers which define
   the panel until a negative point number is seen.  The number of pt nums
   should equal the number of points in the panel.
*/
void panel3d::Init(int *&srcPtNums, pt3d pts[])
{ if (ptNums.Num() >0) ptNums.Empty();
  
  doubleSided= *(srcPtNums++);
  while (*srcPtNums >=0)
    ptNums.Add(*srcPtNums++);
  ptNums.Add(ptNums[0]);
  srcPtNums++;  // advance past negative value
  SetNormalAndConvexity(pts);
}


/*-------------------------------------------------------------------------*/
// The region2d must have at least 2 sides, or else the panel3d will
// simply be empty.
panel3d::panel3d(const region2d& rgn, fastPts& pts,
		 const pt3d& offset, const ang3d& ang, const pt3d& scale,
		 bool isDoubleSided)
{ pt3d	pt;
  int	ptNum,firstPtNum;

  doubleSided= isDoubleSided;
  if (rgn.numSides >=2) {
    forii(rgn.numSides) {
      pt.x= rgn.sides[i].pt.x;
      pt.y= rgn.sides[i].pt.y;
      pt.z= 0;
      ptNum= pts.Add((pt*scale >> ang) +offset);
      if (i==0) firstPtNum= ptNum;
      ptNums.Add(ptNum);
    }
    ptNums.Add(firstPtNum);
    SetNormalAndConvexity(pts.Array());
  }
}


/*-------------------------------------------------------------------------*/
panel3d::panel3d(const panel3d& p, pt3d panelPts[], fastPts& pts,
		 const pt3d& offset, const ang3d& ang, const pt3d& scale) {
  doubleSided= p.doubleSided;
  forii(p.ptNums.Num())
  { pt3d *pt= &panelPts[p.ptNums[i]];
    int ptNum= pts.Add( ((*pt)*scale >> ang) +offset);
    ptNums.Add(ptNum);
  }
  SetNormalAndConvexity(pts.Array());
}  
