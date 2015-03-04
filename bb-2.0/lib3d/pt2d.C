// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


// pt2d class

#include "pt2d.h"



/*======================================================================*/
// Return value is in the range [-MA_PI/4, 7*MA_PI/4)
ang2d pt2d::Ang2d() const {
  if (x==0 and y==0) return 0;
  if (x  >=ABS(y)) return (ang2d)(      0   + (y * (MA_PI/4)) / x);
  if (-x >=ABS(y)) return (ang2d)(  MA_PI   + (y * (MA_PI/4)) / x);
  if (y  > ABS(x)) return (ang2d)(  MA_PI/2 - (x * (MA_PI/4)) / y);
                   return (ang2d)(3*MA_PI/2 - (x * (MA_PI/4)) / y);
}


/*----------------------------------------------------------------------*/
void pt2d::Interpolate(const pt2d& pt1, const pt2d& pt2, int i, int range)
{ if (range==0) return;
  x= pt1.x + i*(pt2.x -pt1.x)/range;
  y= pt1.y + i*(pt2.y -pt1.y)/range;
}


/*-------------------------------------------------------------------------*/
pt2d pt2d::Normalized(coord len) const
// Treating this pt as a vector, normalize so that vector length= len
{ return (*this) * (len/Dist());
}


/*----------------------------------------------------------------------*/
// Treating this pt as a vector, normalize so that vector length= len
void pt2d::Normalize(coord len)
{ *this *= len/Dist();
}


/*----------------------------------------------------------------------*/
// Treating this pt as a vector, normalize so that vector length= len
void pt2d::FastNormalize(coord len)
{ *this *= len/FastDist();
}


/*----------------------------------------------------------------------*/
pt2d pt2d::operator>>(double ang) const
{ double sinAng= sin(ang);
  double cosAng= cos(ang);
  return pt2d((coord)(cosAng*x - sinAng*y),(coord)(cosAng*y + sinAng*x));
}


/*----------------------------------------------------------------------*/
void pt2d::operator>>=(double ang)
{ double sinAng= sin(ang);
  double cosAng= cos(ang);
  coord	tempx= (coord)(cosAng*x - sinAng*y);
  y= (coord)(cosAng*y + sinAng*x);
  x= tempx;
}
