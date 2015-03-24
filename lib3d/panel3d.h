// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef PANEL3D_h
#define PANEL3D_h


#include "general.h"
#include "pt3d.h"
#include "region2d.h"
#include "fastpts.h"
#include "table.h"



/************************************************************************/
struct panel3d {
  typedef pt3d::coord coord;	// coordinate type

  table<int>	ptNums;		// table of point numbers; last pt num is
 				// the same as the first pt num
  pt3d		normal;		// normal of the plane which contains the
  				// panel3d; if the panel3d is single-sided,
  				// points to the outside of the panel3d
  bool		doubleSided;	// true  -> panel3d is double-sided (i.e.
  				//          is visible from both sides)
  				// false -> panel3d is single-sided (i.e.
  				//          is a side of a solid object)
protected:
  bool		isConvex;	// true  -> panel3d is convex
  
public:
  	panel3d() {};
  	panel3d(int* &nums,pt3d pts[])
	       {Init(nums,pts);};
  	panel3d(const region2d& rgn, fastPts& fp,
		const pt3d& offset=pt3d(0,0,0), const ang3d& ang=ang3d(0),
		const pt3d& scale =pt3d(1,1,1), bool ds=0);
  	panel3d(const panel3d& p, pt3d panelPts[], fastPts& fp,
		const pt3d& offset=pt3d(0,0,0), const ang3d& ang=ang3d(0),
		const pt3d& scale =pt3d(1,1,1));

 protected:
  void	Init(int*&,pt3d[]);

 public:
  void	Clip(const pt3d src[],const pt2d prj[],pt2d dst[],coord,
	     const pt2d&,bool,int&);
  bool	Faces(pt3d pts[],const pt3d& p)
          {return ( (pts[ptNums[0]] - p).Dot(normal) <=0) or doubleSided;}
  bool	IsConvex() const {return isConvex;}
  
 private:
  void	SetNormalAndConvexity(const pt3d[]);
};


/*-------------------------------------------------------------------------*/
/* In:  ...
   Out: numDestPts= number of pts in the clipped panel3d
 
  Clipping algorithm:
   for each point
     if coord & prev coord are not on the same side of x==planex, then 
       find intersection w/ the x==planex plane
       copy intersection to new points
     if coord.x >=planex, then
       copy coord to new points
   if the new points form an unclosed polygon,
     add an extra new point to close the polygon

  Warning!  The number of points in the clipped panel3d can be, at most,
            exactly 50% *more* than the number of points in the original
            panel3d.  Make sure destPts[] is large enough to hold all
            the points in the clipped panel3d.

  This function is inlined because the arguments for 'greaterPts' and
  'planex' are likely to be constants, in which case an optimizing compiler
  would make this function small & fast.
*/
inline void panel3d::Clip(const pt3d srcPts[], const pt2d prjPts[],
			  pt2d destPts[],
			  coord planex, const pt2d& windowCenter,
			  bool greaterPts, int& numDestPts) {
  if (ptNums.Num()==0) {
    numDestPts= 0;
    return;
  }

  int *ptNum= ptNums.Array();
  int *ptNumsEnd= ptNum +ptNums.Num();
  pt2d *destPt= destPts;

  if ((srcPts[*ptNum].x >=planex and greaterPts) or 
      (srcPts[*ptNum].x <=planex and not greaterPts))
    *(destPt++)= prjPts[*ptNum];

  int *prevNum= ptNum++;

  while (ptNum != ptNumsEnd) {
    if ((srcPts[*ptNum].x >=planex) != (srcPts[*prevNum].x >=planex)) {
      pt3d temp;
      temp.GetXCrossing(srcPts[*prevNum], srcPts[*ptNum], planex);
      *(destPt++)= temp.Project() +windowCenter;
    }
    
    if ((srcPts[*ptNum].x >=planex and greaterPts) or 
        (srcPts[*ptNum].x <=planex and not greaterPts))
      *(destPt++)= prjPts[*ptNum];

    prevNum= ptNum++;
  }
  
  if (destPt != destPts and *(destPt-1) != destPts[0])
    *(destPt++)= destPts[0];
  
  numDestPts= destPt -destPts;
}


#endif
