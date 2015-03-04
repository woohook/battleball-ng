// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include <math.h>
#include "pt3d.h"


/*-------------------------------------------------------------------------*/
pt3d pt3d::operator>>(const ang3d& ang) const
{ double	sinAng,
  	cosAng;
  coord	tempx;
  pt3d	newPt;
  
  sinAng= sinTbl[(ang.yz)&(2*MA_PI-1)];
  cosAng= cosTbl[(ang.yz)&(2*MA_PI-1)];
  newPt.y= (coord)(cosAng*y - sinAng*z);
  newPt.z= (coord)(cosAng*z + sinAng*y);
  
  sinAng= sinTbl[(ang.xz)&(2*MA_PI-1)];
  cosAng= cosTbl[(ang.xz)&(2*MA_PI-1)];
  tempx=   (coord)(cosAng*x - sinAng*newPt.z);
  newPt.z= (coord)(cosAng*newPt.z + sinAng*x);
  
  sinAng= sinTbl[(ang.xy)&(2*MA_PI-1)];
  cosAng= cosTbl[(ang.xy)&(2*MA_PI-1)];
  newPt.x= (coord)(cosAng*tempx - sinAng*newPt.y);
  newPt.y= (coord)(cosAng*newPt.y + sinAng*tempx);
  return newPt;
}


/*-------------------------------------------------------------------------*/
pt3d pt3d::operator<<(const ang3d& ang) const
{ double	sinAng,
  	cosAng;
  coord	tempz;
  pt3d	newPt;
  
  sinAng= sinTbl[(-(ang.xy))&(2*MA_PI-1)];
  cosAng= cosTbl[(-(ang.xy))&(2*MA_PI-1)];
  newPt.x= (coord)(cosAng*x - sinAng*y);
  newPt.y= (coord)(cosAng*y + sinAng*x);

  sinAng= sinTbl[(-(ang.xz))&(2*MA_PI-1)];
  cosAng= cosTbl[(-(ang.xz))&(2*MA_PI-1)];
  tempz  = (coord)(cosAng*z + sinAng*newPt.x);
  newPt.x= (coord)(cosAng*newPt.x - sinAng*z);

  sinAng= sinTbl[(-ang.yz)&(2*MA_PI-1)];
  cosAng= cosTbl[(-ang.yz)&(2*MA_PI-1)];
  newPt.z= (coord)(cosAng*tempz + sinAng*newPt.y);
  newPt.y= (coord)(cosAng*newPt.y - sinAng*tempz);
  return newPt;
}


/*-------------------------------------------------------------------------*/
pt3d pt3d::operator>>(const double m[3][3]) const {
  return pt3d((coord)(m[0][0]*x +m[0][1]*y +m[0][2]*z),
	      (coord)(m[1][0]*x +m[1][1]*y +m[1][2]*z),
	      (coord)(m[2][0]*x +m[2][1]*y +m[2][2]*z) );
}


/*-------------------------------------------------------------------------*/
/* To rotate in reverse, we take the inverse of the rotation mtrx
   (i.e. take its transpose and divide by its determinant) and multiply
   the mtrx against the point.  Actually we don't do exactly that; we do
   compute the determinant and divide each mtrx element by the determinant,
   but we simulate taking the transpose by changing which elements are
   multiplied against the point vector.
*/
pt3d pt3d::operator<<(const double m[3][3]) const {
  double det=  m[0][0]*(m[1][1]*m[2][2] - m[1][2]*m[2][1])
              -m[0][1]*(m[1][0]*m[2][2] - m[1][2]*m[2][0])
	      +m[0][2]*(m[1][0]*m[2][1] - m[1][1]*m[2][0]);
  if (det==0.0) {cerr << "determinant is 0.\n"; return *this;};
  double r= 1.0/det;   // reciprocal
  pt3d pt;
  pt.x= (coord)((m[0][0]*x +m[1][0]*y +m[2][0]*z)*r);
  pt.y= (coord)((m[0][1]*x +m[1][1]*y +m[2][1]*z)*r);
  pt.z= (coord)((m[0][2]*x +m[1][2]*y +m[2][2]*z)*r);
  return pt;
}


/*-------------------------------------------------------------------------*/
pt3d pt3d::operator>>(const xformMtrx m) const {
  return pt3d(m[0][0]*x +m[1][0]*y +m[2][0]*z +m[3][0],
	      m[0][1]*x +m[1][1]*y +m[2][1]*z +m[3][1],
	      m[0][2]*x +m[1][2]*y +m[2][2]*z +m[3][2]);
}


/*-------------------------------------------------------------------------*/
/* To untransform, we subtract the translational part of the transform
   from the point and then multiply by the inverse of the 3x3 part of the
   transform.
*/
pt3d pt3d::operator<<(const xformMtrx m) const {
  double det=  m[0][0]*(m[1][1]*m[2][2] - m[1][2]*m[2][1])
              -m[0][1]*(m[1][0]*m[2][2] - m[1][2]*m[2][0])
	      +m[0][2]*(m[1][0]*m[2][1] - m[1][1]*m[2][0]);
  if (det==0.0) {cerr << "determinant is 0.\n"; return *this;};
  pt3d p(x-m[3][0],y-m[3][1],z-m[3][2]);
  return pt3d(m[0][0]*p.x +m[0][1]*p.y +m[0][2]*p.z,
	      m[1][0]*p.x +m[1][1]*p.y +m[1][2]*p.z,
	      m[2][0]*p.x +m[2][1]*p.y +m[2][2]*p.z) / det;
}


/*-------------------------------------------------------------------------*/
pt3d pt3d::operator>>(double ang) const
{ double	sinAng= sin(ang),
  	cosAng= cos(ang);
  pt3d	newPt;
  
  newPt.x= (coord)(cosAng*x - sinAng*y);
  newPt.y= (coord)(cosAng*y + sinAng*x);
  newPt.z= z;
  return newPt;
}


/*-------------------------------------------------------------------------*/
void pt3d::Interpolate(const pt3d& firstPt, const pt3d& lastPt, int i,
		       int range)
{ if (range==0) return;
  x= firstPt.x + i*(lastPt.x -firstPt.x)/range;
  y= firstPt.y + i*(lastPt.y -firstPt.y)/range;
  z= firstPt.z + i*(lastPt.z -firstPt.z)/range;
}


/*-------------------------------------------------------------------------*/
pt3d pt3d::Normalized(coord len) const
// Treating this pt as a vector, normalize so that vector length= len
{ return (*this) * (len/Dist());
}


/*-------------------------------------------------------------------------*/
void pt3d::Normalize(coord len)
// Treating this pt as a vector, normalize so that vector length= len
{ *this *= len/Dist();
}


/*-------------------------------------------------------------------------*/
void pt3d::FastNormalize(coord len)
// Treating this pt as a vector, normalize so that vector length= len
{ *this *= len/FastDist();
}
