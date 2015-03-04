// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef PT2D_h
#define PT2D_h


#include <math.h>    // to get sqrt(), M_SQRT2, sin(), etc.
#include "general.h"
#include "ang2d.h"


/*======================================================================*/
struct pt2d {
  enum {numDims=2};
  typedef double coord;
  coord	x,y;

  	pt2d() {};
  	pt2d(coord nx, coord ny) : x(nx), y(ny) {}
  	pt2d(const coord* ptr) {x= *ptr++; y= *ptr++;}
  	pt2d(coord*& ptr)      {x= *ptr++; y= *ptr++;}
  	pt2d(const pt2d*& ptr) {*this= *ptr++;}
  	pt2d(pt2d*& ptr)       {*this= *ptr++;}

  // inline functions
  bool	operator==(const pt2d& p) const	{return (x==p.x and y==p.y);}
  bool	operator!=(const pt2d& p) const	{return (x!=p.x or y!=p.y);}
  void	operator+=(const pt2d& p)	{x+=p.x; y+=p.y;}
  void	operator-=(const pt2d& p)	{x-=p.x; y-=p.y;}
  pt2d	operator+ (const pt2d& p) const {return pt2d(x+p.x,y+p.y);}
  pt2d	operator- (const pt2d& p) const {return pt2d(x-p.x,y-p.y);}
  void	operator*=(const pt2d& sc)	{x*=sc.x; y*=sc.y;}
  void	operator*=(coord sc)		{x*=sc; y*=sc;}
  pt2d	operator* (coord sc) const	{return pt2d(x*sc, y*sc);}
  pt2d	operator* (const pt2d& p) const	{return pt2d(x*p.x, y*p.y);}
  coord	Dot       (const pt2d& p) const	{return x*p.x +y*p.y;}
  void	operator/=(coord dv)		{x/=dv; y/=dv;}
  pt2d	operator/ (coord sc) const	{return pt2d(x/sc, y/sc);}
  coord& operator[](int i)              {if (i==0) return x; else return y;}
  const
  coord& operator[](int i) const        {if (i==0) return x; else return y;}
  coord	Dist() const		{return sqrt((double)(sqr(x) +sqr(y)));}
  coord	FastDist() const;
  pt2d	operator>>(ang2d) const;
  void	operator>>=(ang2d);
  pt2d	Left90() const		{return pt2d(-y,x);}
  pt2d	Right90() const		{return pt2d(y,-x);}

  // non-inline functions
  ang2d	Ang2d() const;
  void	Interpolate(const pt2d&,const pt2d&,int,int);
  pt2d  Normalized(coord=1) const;
  void	Normalize(coord);
  void	FastNormalize(coord);
  pt2d	operator>>(double) const;
  void	operator>>=(double);
};


/*----------------------------------------------------------------------*/
inline pt2d::coord pt2d::FastDist() const
{ pt2d absPt;

  absPt.x= ABS(x);
  absPt.y= ABS(y);
  if (absPt.x >absPt.y)
    return absPt.x +(M_SQRT2-1)*absPt.y;
  else
    return absPt.y +(M_SQRT2-1)*absPt.x;
}


/*----------------------------------------------------------------------*/
inline pt2d pt2d::operator>>(ang2d ang) const
{ double sinAng= sinTbl[ang&(2*MA_PI-1)];
  double cosAng= cosTbl[ang&(2*MA_PI-1)];
  return pt2d((coord)(cosAng*x - sinAng*y),(coord)(cosAng*y + sinAng*x));
}


/*----------------------------------------------------------------------*/
inline void pt2d::operator>>=(ang2d ang)
{ double sinAng= sinTbl[ang&(2*MA_PI-1)];
  double cosAng= cosTbl[ang&(2*MA_PI-1)];
  coord	tempx= (coord)(cosAng*x - sinAng*y);
  y= (coord)(cosAng*y + sinAng*x);
  x= tempx;
}


/*======================================================================*/
// Destination point is pt1 for function below
inline void Get2dXCrossing(pt2d& pt1,const pt2d& pt2,pt2d::coord planex)
{ pt1.y= pt1.y +(planex - pt1.x) * (pt2.y - pt1.y) / (pt2.x - pt1.x);
  pt1.x= planex;
}


// Destination point is pt1 for function below
inline void Get2dYCrossing(pt2d& pt1,const pt2d& pt2,pt2d::coord planey)
{ pt1.x= pt1.x +(planey - pt1.y) * (pt2.x - pt1.x) / (pt2.y - pt1.y);
  pt1.y= planey;
}


/*----------------------------------------------------------------------*/
/* In:	numPts = number of pts in pts[] array
   Out:	numPts = number of pts in destPts[] array

   Clipping algorithm:
   for each point
     if coord & prev coord are not on the same side of x==planex, then 
       find intersection w/ the x==planex plane
       copy intersection to new points
     if coord.x >=planex, then
       copy coord to new points
   if the new points form an unclosed polygon,
     add an extra new point to close the polygon
   
   Presumably, the arguments for the parameters 'greaterPts' and 'axis'
   will usually be constants, in which case an optimizing compiler should
   be able to make this function small & fast.  (This is the
   reason for declaring this function 'inline'.)
*/

inline void ClipPts(const pt2d *srcPt, pt2d destPts[], int& numPts,
		    double plane, bool greaterPts, int axis) {
  const pt2d *srcPtsEnd= srcPt +numPts;
  pt2d *destPt= destPts;
#define axis2 (not axis)

  if (numPts==0) return;
  if (((*srcPt)[axis] >= plane and greaterPts) or 
      ((*srcPt)[axis] <= plane and not greaterPts))
    *(destPt++)= *srcPt;
    
  const pt2d *prevPt= srcPt;
  srcPt++;

  while (srcPt != srcPtsEnd) {
    if (((*srcPt)[axis] >=plane) != ((*prevPt)[axis] >=plane)) {
      (*destPt)[axis]= plane;
      (*destPt)[axis2]= (*prevPt)[axis2] + (plane - (*prevPt)[axis]) * 
	((*srcPt)[axis2] - (*prevPt)[axis2]) /
	((*srcPt)[axis]  - (*prevPt)[axis]);
      destPt++;
    }
    if (((*srcPt)[axis] >=plane and greaterPts) or 
        ((*srcPt)[axis] <=plane and not greaterPts))
      *(destPt++)= *srcPt;
    prevPt= srcPt++;
  }
  
  if (destPt != destPts and *(destPt-1) != *destPts)
    *(destPt++)= *destPts;
  
  numPts= destPt -destPts;
}

#endif
