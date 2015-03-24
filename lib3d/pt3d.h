// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef PT3D_h
#define PT3D_h


#include <iostream.h>
#include "general.h"
#include "pt2d.h"
#include "ang3d.h"


/*=========================================================================*/
struct pt3d : pt2d {
  enum {numDims=3};
 private:
  typedef double xformMtrx[numDims+1][numDims+1];
 public:

  coord	z;

  	pt3d() {};
  	pt3d(coord nx, coord ny, coord nz=0) : z(nz) {x=nx; y=ny;}
  	pt3d(const coord* ptr) {x= *ptr++; y= *ptr++; z= *ptr++;}
  	pt3d(coord*& ptr) {x= *ptr++; y= *ptr++; z= *ptr++;}
  	pt3d(pt3d*& ptr) {*this= *ptr++;}

  // inline functions
  bool	operator==(const pt3d& p) const	{return (x==p.x and y==p.y and z==p.z);}
  bool	operator!=(const pt3d& p) const	{return (x!=p.x or  y!=p.y or  z!=p.z);}
  pt3d	operator- () const		{return pt3d(-x,-y,-z);}
  void	operator+=(const pt3d& p)	{x+=p.x; y+=p.y; z+=p.z;}
  void	operator-=(const pt3d& p)	{x-=p.x; y-=p.y; z-=p.z;}
  pt3d	operator+ (const pt3d& p) const	{return pt3d(x+p.x, y+p.y, z+p.z);}
  pt3d	operator- (const pt3d& p) const	{return pt3d(x-p.x, y-p.y, z-p.z);}
  void	operator*=(coord sc)		{x*=sc; y*=sc; z*=sc;}
  void  operator*=(pt3d sc)             {x*=sc.x; y*=sc.y; z*=sc.z;}
  pt3d  operator* (coord s) const       {return pt3d(x*s, y*s, z*s);}
  pt3d	operator* (const pt3d& p) const	{return pt3d(x*p.x, y*p.y, z*p.z);}
  coord	Dot       (const pt3d& p) const	{return x*p.x +y*p.y +z*p.z;}
  void	operator/=(coord dv)		{x/=dv; y/=dv; z/=dv;}
  pt3d	operator/ (coord dv) const	{return pt3d(x/dv,y/dv,z/dv);}
  pt3d	operator>>(ang2d) const;
  void	operator>>=(ang2d a)		{pt2d::operator>>=(a);}
  void	operator>>=(double a)		{pt2d::operator>>=(a);}
  void	operator>>=(const ang3d& a) {*this= *this >> a;}
  void	operator<<=(const ang3d& a) {*this= *this << a;}
  coord& operator[](int);
  coord	Dist() const			{return sqrt((double)(sqr(x)+sqr(y)+sqr(z)));}
  coord	FastDist() const;
  pt2d  Project() const			{coord r=1/x; return pt2d(y*r,z*r);}
  ang3d Ang3d() const;
  void	GetXCrossing(const pt3d&,coord);
  void	GetXCrossing(const pt3d&,const pt3d&,coord);
  pt3d	CrossProd(const pt3d&) const;
  void	SetMax(const pt3d&);
  void	SetMin(const pt3d&);
  bool	IsBetween           (const pt3d&,const pt3d&) const;
  bool	IsBetweenInclusively(const pt3d&,const pt3d&) const;
  bool	IsBetweenExclusively(const pt3d&,const pt3d&) const;
  
  // non-inline functions
  pt3d	operator>> (const ang3d& a) const;	// >> is Rotate
  pt3d	operator<< (const ang3d& a) const;	// << is Rotate Reverse
  pt3d  operator>> (const double[3][3]) const;
  pt3d  operator<< (const double[3][3]) const;
  pt3d  operator>> (const xformMtrx) const;
  pt3d  operator<< (const xformMtrx) const;
  pt3d	operator>> (double a) const; // {pt3d p= *this; p.pt2d::operator>>=(a); return p;}
  void  Interpolate(const pt3d& p1,const pt3d& p2,int i,int r);
  pt3d  Normalized(coord=1) const;
  void  Normalize(coord=1);
  void  FastNormalize(coord=1);
};


/*=========================================================================*/
inline pt3d::coord pt3d::FastDist() const
{ pt2d	p2;
  p2.x= pt2d::FastDist();
  p2.y= z;
  return p2.FastDist();
}


/*-------------------------------------------------------------------------*/
inline pt3d::coord& pt3d::operator[](int i) {
  if (i==0) return x;
  else if (i==1) return y;
  else return z;
}


/*-------------------------------------------------------------------------*/
inline ang3d pt3d::Ang3d() const
{ ang3d ang;
  pt2d	pt2;
  
  ang.xy= pt2d::Ang2d();
  pt2.x= pt2d::Dist();
  pt2.y= z;
  ang.xz= pt2.Ang2d();
  ang.yz= 0;
  return ang;
}


/*-------------------------------------------------------------------------*/
inline pt3d pt3d::operator>>(ang2d ang) const {
  double sinAng= sinTbl[ang & (2*MA_PI-1)];
  double cosAng= cosTbl[ang & (2*MA_PI-1)];
  return pt3d((coord)(cosAng*x - sinAng*y),(coord)(cosAng*y + sinAng*x),z);
}


/*-------------------------------------------------------------------------*/
// Computes the intersection of the plane x=planex and the infinite line
// from this point thru endPt.  Stores the intersection in *this point.
inline void pt3d::GetXCrossing(const pt3d& endPt, coord planex) {
  coord temp= (planex -x)/(endPt.x -x);  // assumes denominator is nonzero!
  x= planex;
  y += temp * (endPt.y -y);
  z += temp * (endPt.z -z);
}


/*-------------------------------------------------------------------------*/
// Computes the intersection of the plane x=planex and the infinite line
// from beginPt thru endPt.  Stores the intersection in *this point.
inline void pt3d::GetXCrossing(const pt3d& beginPt, const pt3d& endPt,
			       coord planex) {
  coord temp= (planex -beginPt.x)/(endPt.x -beginPt.x);
  x= planex;
  y= beginPt.y + temp * (endPt.y -beginPt.y);
  z= beginPt.z + temp * (endPt.z -beginPt.z);
}


/*-------------------------------------------------------------------------*/
inline pt3d pt3d::CrossProd(const pt3d& p) const {
  return pt3d(  y*p.z - z*p.y,
	      -(x*p.z - z*p.x),
		x*p.y - y*p.x );
}


/*-------------------------------------------------------------------------*/
inline void pt3d::SetMax(const pt3d& pt)
{ if (pt.x >x) x= pt.x;
  if (pt.y >y) y= pt.y;
  if (pt.z >z) z= pt.z;
}


/*-------------------------------------------------------------------------*/
inline void pt3d::SetMin(const pt3d& pt)
{ if (pt.x <x) x= pt.x;
  if (pt.y <y) y= pt.y;
  if (pt.z <z) z= pt.z;
}


/*-------------------------------------------------------------------------*/
inline bool pt3d::IsBetween(const pt3d& p1, const pt3d& p2) const
{ return
  (isBetween(p1.x,x,p2.x) and 
   isBetween(p1.y,y,p2.y) and 
   isBetween(p1.z,z,p2.z));
}


/*-------------------------------------------------------------------------*/
inline bool pt3d::IsBetweenInclusively(const pt3d& p1, const pt3d& p2) const
{ return
  (isBetweenInclusively(p1.x,x,p2.x) and 
   isBetweenInclusively(p1.y,y,p2.y) and 
   isBetweenInclusively(p1.z,z,p2.z));
}


/*-------------------------------------------------------------------------*/
inline bool pt3d::IsBetweenExclusively(const pt3d& p1, const pt3d& p2) const
{ return
  (isBetweenExclusively(p1.x,x,p2.x) and 
   isBetweenExclusively(p1.y,y,p2.y) and 
   isBetweenExclusively(p1.z,z,p2.z));
}


/*=========================================================================*/
inline ostream& operator<< (ostream& out, const pt3d& p) {
  return out << p.x << " " << p.y << " " << p.z;
}

inline istream& operator>> (istream& in, pt3d& p) {
  return in >> p.x >> p.y >> p.z;
}

  
#endif
