// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef ANG3D_h
#define ANG3D_h


#include <iostream.h>
#include "general.h"
#include "ang2d.h"


struct ang3d
{ ang2d	xy,		// angle in XY plane
   	xz,		// angle in XZ plane
  	yz;		// angle in YZ plane
  
  	ang3d() {};
  	ang3d(ang2d a,ang2d b=0,ang2d c=0) : xy(a), xz(b), yz(c) {};
  	
  bool	operator==(const ang3d& a) const
  			{return (xy==a.xy and xz==a.xz and yz==a.yz);}
  bool	operator!=(const ang3d& a) const
  			{return (xy!=a.xy or  xz!=a.xz or  yz!=a.yz);}
  ang3d	operator- ()               const{return ang3d(-xy,-xz,-yz);}
  void	operator+=(const ang3d& a)	{xy+=a.xy; xz+=a.xz; yz+=a.yz;}
  void	operator-=(const ang3d& a)	{xy-=a.xy; xz-=a.xz; yz-=a.yz;}
  ang3d	operator+ (const ang3d& a) const;
  ang3d	operator- (const ang3d& a) const;

  void	MakeRevMtrx(double[3][3]) const;
  void	SetMax(const ang3d& a);
  void	SetMin(const ang3d& a);

  	// Non-inline functions
  ang3d	Normalized() const;
};


/*-------------------------------------------------------------------------*/
inline ang3d ang3d::operator+(const ang3d& ang) const
{ ang3d	result;
  result.xy= xy +ang.xy;
  result.xz= xz +ang.xz;
  result.yz= yz +ang.yz;
  return result;
}


/*-------------------------------------------------------------------------*/
inline ang3d ang3d::operator-(const ang3d& ang) const
{ ang3d	result;
  result.xy= xy -ang.xy;
  result.xz= xz -ang.xz;
  result.yz= yz -ang.yz;
  return result;
}


/*-------------------------------------------------------------------------*/
inline void ang3d::SetMax(const ang3d& a)
{ if (a.xy >xy) xy= a.xy;
  if (a.xz >xz) xz= a.xz;
  if (a.yz >yz) yz= a.yz;
}


/*-------------------------------------------------------------------------*/
inline void ang3d::SetMin(const ang3d& a)
{ if (a.xy <xy) xy= a.xy;
  if (a.xz <xz) xz= a.xz;
  if (a.yz <yz) yz= a.yz;
}


/*=========================================================================*/
inline ostream& operator<< (ostream& o, const ang3d& a) {
  return o << a.xy << " " << a.xz << " " << a.yz;
}
inline istream& operator>>(istream& in, ang3d& a) {
  return in >> a.xy >> a.xz >> a.yz;
}


#endif
