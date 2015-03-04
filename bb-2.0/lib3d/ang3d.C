// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include "ang3d.h"



/************************************************************************/
void ang3d::MakeRevMtrx(double m[3][3]) const
{ double	sinxy, cosxy,
	sinxz, cosxz,
	sinyz, cosyz;
  
  sinxy= sinTbl[xy&(2*MA_PI-1)];
  cosxy= cosTbl[xy&(2*MA_PI-1)];
  sinxz= sinTbl[xz&(2*MA_PI-1)];
  cosxz= cosTbl[xz&(2*MA_PI-1)];
  sinyz= sinTbl[yz&(2*MA_PI-1)];
  cosyz= cosTbl[yz&(2*MA_PI-1)];

  m[0][0]= cosxy*cosxz;
  m[0][1]= sinxy*cosxz;
  m[0][2]= sinxz;
  m[1][0]= -cosxy*sinxz*sinyz -sinxy*cosyz;
  m[1][1]= -sinxy*sinxz*sinyz +cosxy*cosyz;
  m[1][2]= cosxz*sinyz;
  m[2][0]= -cosxy*sinxz*cosyz +sinxy*sinyz;
  m[2][1]= -sinxy*sinxz*cosyz -cosxy*sinyz;
  m[2][2]= cosxz*cosyz;
}


/*-------------------------------------------------------------------------*/
// Returns an equivalent ang3d such that xy, xz, & yz are all within the
// range [-MA_PI, MA_PI)
// Assumes that xy, xz, & yz are all >= -3*MA_PI to begin with
ang3d ang3d::Normalized() const {
  return ang3d((xy+3*MA_PI)%(2*MA_PI) -MA_PI,
	       (xz+3*MA_PI)%(2*MA_PI) -MA_PI,
	       (yz+3*MA_PI)%(2*MA_PI) -MA_PI);
}
