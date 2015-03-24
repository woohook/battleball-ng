// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include <math.h>
#include <iomanip.h>
#include "xform.h"



//===========================================================================
// Extend the pt3d class!
// This must be done at the top of this file, because code below (may) assume
// the pt3d has these operators.


/*-------------------------------------------------------------------------*/
pt3d operator*(const pt3d& p, const tmtrx& t) {
  return pt3d(t[0][0]*p.x +t[1][0]*p.y +t[2][0]*p.z +t[3][0],
	      t[0][1]*p.x +t[1][1]*p.y +t[2][1]*p.z +t[3][1],
	      t[0][2]*p.x +t[1][2]*p.y +t[2][2]*p.z +t[3][2]);
}


/*-------------------------------------------------------------------------*/
/* To untransform, we subtract the translational part of the transform
   from the point and then multiply by the inverse of the 3x3 part of the
   transform.
   WARNING:  tmtrx is assumed to have unity scaling
*/
pt3d operator/(const pt3d& p, const tmtrx& t) {
  pt3d p2(p.x-t[3][0],p.y-t[3][1],p.z-t[3][2]);
  return pt3d(t[0][0]*p2.x +t[0][1]*p2.y +t[0][2]*p2.z,
	      t[1][0]*p2.x +t[1][1]*p2.y +t[1][2]*p2.z,
	      t[2][0]*p2.x +t[2][1]*p2.y +t[2][2]*p2.z);
}



//===========================================================================
void tmtrx::Copy(const fourby3 newm) {
  forii(4) forij(3) m[i][j]= newm[i][j];
}


//---------------------------------------------------------------------------
bool tmtrx::operator==(const tmtrx& t) const {
  forii(4) forij(3)
    if (m[i][j] != t[i][j]) return false;
  return true;
}


//---------------------------------------------------------------------------
// Returns true if the rotation parts (i.e. the 3x3 parts) of this matrix
// and t's matrix are the same; returns false otherwise

bool tmtrx::RotationSame(const tmtrx& t) const {
  forii(3) forij(3)
    if (m[i][j] != t[i][j]) return false;
  return true;
}


//---------------------------------------------------------------------------
// Set the 3x3 part of the matrix from ang and scale.  The implementation of
// this function is a bit goofy because it is (sort of) optimized for speed.

void tmtrx::SetMtrx(const ang3d& ang, const pt3d& scale) {
  double sinxy= sinTbl[ang.xy&(2*MA_PI-1)];
  double cosxy= cosTbl[ang.xy&(2*MA_PI-1)];
  double sinxz= sinTbl[ang.xz&(2*MA_PI-1)];
  double cosxz= cosTbl[ang.xz&(2*MA_PI-1)];
  double sinyz= sinTbl[ang.yz&(2*MA_PI-1)];
  double cosyz= cosTbl[ang.yz&(2*MA_PI-1)];
  
  int index= (ang.xy != 0) | ((ang.xz != 0) <<1) | ((ang.yz != 0) <<2);
  switch (index)
  { case 0:
      m[0][0]= m[1][1]= m[2][2]= 1.0;
      m[1][0]= m[2][0]= m[0][1]= m[2][1]= m[0][2]= m[1][2]= 0.0;
      break;
    case 1:
      m[2][2]= 1.0;
      m[2][0]= m[2][1]= m[0][2]= m[1][2]= 0.0;
      m[0][0]= m[1][1]= cosxy;
      m[1][0]= -sinxy;
      m[0][1]= sinxy;
      break;
    case 2:
      m[1][1]= 1.0;
      m[1][0]= m[2][1]= m[0][1]= m[1][2]= 0.0;
      m[0][0]= m[2][2]= cosxz;
      m[2][0]= -sinxz;
      m[0][2]= sinxz;
      break;
    case 4:
      m[0][0]= 1.0;
      m[1][0]= m[2][0]= m[0][1]= m[0][2]= 0.0;
      m[1][1]= m[2][2]= cosyz;
      m[2][1]= -sinyz;
      m[1][2]= sinyz;
      break;
    case 3:
    case 5:
    case 6:
    case 7:
      m[0][0]= cosxy*cosxz;
      m[1][0]= -cosxy*sinxz*sinyz -sinxy*cosyz;
      m[2][0]= -cosxy*sinxz*cosyz +sinxy*sinyz;
      m[0][1]= sinxy*cosxz;
      m[1][1]= -sinxy*sinxz*sinyz +cosxy*cosyz;
      m[2][1]= -sinxy*sinxz*cosyz -cosxy*sinyz;
      m[0][2]= sinxz;
      m[1][2]= cosxz*sinyz;
      m[2][2]= cosxz*cosyz;
      break;
  }

  if (scale != pt3d(1,1,1)) {
    m[0][0] *= scale.x;
    m[1][0] *= scale.y;
    m[2][0] *= scale.z;
    m[0][1] *= scale.x;
    m[1][1] *= scale.y;
    m[2][1] *= scale.z;
    m[0][2] *= scale.x;
    m[1][2] *= scale.y;
    m[2][2] *= scale.z;
  }
}


//---------------------------------------------------------------------------
// Return the angular rotation part of the tmtrx as an and3d.
// Believe it or not, floating point round-off error is a real problem in
// this function, hence the use of the epsilon constant.
// WARNING:  assumes the tmtrx has unity scaling

ang3d tmtrx::Ang() const {
  const double eps= 1e-9;      // epsilon
  double xy,xz,yz;

  xz= asin(m[0][2]*(1-eps));   // returns a value in [-M_PI/2, +M_PI/2]
  double cosxz= cos(xz);       // should always be non-negative
  if (cosxz <=eps) {
    yz= 0;                     // can I do better than this? -PAH
    xy= acos(m[1][1]*(1-eps));
    if (m[0][0] <0)
      xy= M_PI-xy;
  }
  else {
    yz= asin ((m[1][2] / cosxz) * (1-eps));
    if (m[2][2] <0)
      yz= M_PI-yz;
    xy= asin ((m[0][1] / cosxz) * (1-eps));
    if (m[0][0] <0)
      xy= M_PI-xy;
  }
  return
    ang3d((ang2d)(floor(xy*MA_PI/M_PI +0.5)),
	  (ang2d)(floor(xz*MA_PI/M_PI +0.5)),
	  (ang2d)(floor(yz*MA_PI/M_PI +0.5)) );
}


//---------------------------------------------------------------------------
tmtrx tmtrx::operator*(const tmtrx& t) const {
  fourby3 dest;
  
  forii(3)    // row
    forij(3)  // column
      dest[i][j]= m[i][0]*t[0][j] +m[i][1]*t[1][j] +m[i][2]*t[2][j];
  forij(3)
    dest[3][j]= m[3][0]*t[0][j] +m[3][1]*t[1][j] +m[3][2]*t[2][j] +t[3][j];
  return tmtrx(dest);
}


//---------------------------------------------------------------------------
// Return the inverse of the tmtrx.
// WARNING:  assumes the tmtrx has unity scaling

tmtrx tmtrx::Inverse() const {
  //  double det=  m[0][0]*(m[1][1]*m[2][2] - m[1][2]*m[2][1])
  //            -m[0][1]*(m[1][0]*m[2][2] - m[1][2]*m[2][0])
  //	      +m[0][2]*(m[1][0]*m[2][1] - m[1][1]*m[2][0]);
  //if (det==0.0) {cerr << "determinant is 0.\n"; return *this;}

  fourby3 dest;
  forii(3)    // row
    forij(3)  // column
      dest[i][j]= m[j][i];
  pt3d temp= pt3d(0,0,0) << (*this);
  dest[3][0]= temp.x;
  dest[3][1]= temp.y;
  dest[3][2]= temp.z;
  return tmtrx(dest);
}


ostream& operator<<(ostream& o, const tmtrx& t) {
  o << setprecision(4);
  o << t.m[0][0] << " " << t.m[0][1] << " " << t.m[0][2] << "\n";
  o << t.m[1][0] << " " << t.m[1][1] << " " << t.m[1][2] << "\n";
  o << t.m[2][0] << " " << t.m[2][1] << " " << t.m[2][2] << "\n";
  o << t.m[3][0] << " " << t.m[3][1] << " " << t.m[3][2] << "\n";
  return o << "\n";
}


//===========================================================================
// WARNING:  Assumes the tmtrx t has unity scaling.

tcomp::tcomp(const tmtrx& t)
  : cart(t.Cart()),
    ang(t.Ang()),
    scale(pt3d(1,1,1)) {}


//---------------------------------------------------------------------------
tcomp& tcomp::operator+=(tcomp& t) {
  Cart()  += t.Cart();
  Ang()   += t.Ang();
  Scale() += t.Scale();
  return *this;
}


//---------------------------------------------------------------------------
tcomp& tcomp::operator-=(tcomp& t) {
  Cart()  -= t.Cart();
  Ang()   -= t.Ang();
  Scale() -= t.Scale();
  return *this;
}


#if 0

//---------------------------------------------------------------------------
// Assumes t2.Cart() is in world coords,
//         t2.Ang()  is in world coords
tcomp tcomp::Plus(tcomp& t2) {
  fourby3 dest;
  Mtrx();    // update m
  t2.Mtrx(); // update t2.m
  
  forii(3)    // row
    forij(3)  // column
      dest[i][j]= m[i][0]*t2.m[0][j] +m[i][1]*t2.m[1][j] +m[i][2]*t2.m[2][j];
  forij(3)
    dest[3][j]= m[3][j] +t2.m[3][j];

  return tcomp(dest);
}


//---------------------------------------------------------------------------
// Assumes t2.Cart() is in world coords,
//         t2.Ang()  is in model coords
tcomp tcomp::Plus2(tcomp& t2) {
  fourby3 dest;
  Mtrx();    // update m
  t2.Mtrx(); // update t2.m
  
  forii(3)    // row
    forij(3)  // column
      dest[i][j]= t2.m[i][0]*m[0][j] +t2.m[i][1]*m[1][j] +t2.m[i][2]*m[2][j];
  forij(3)
    dest[3][j]= m[3][j] +t2.m[3][j];

  return tcomp(dest);
}


//---------------------------------------------------------------------------
// Clamp near-zero elements to zero...necessary b/c of fp roundoff error
tcomp tcomp::Clamped() {
  const double eps= 1e-9; // epsilon
  Mtrx();    // update m
  fourby3 dest;
  forii(4)    // row;  notice: 4, not 3
    forij(3)  // column
      dest[i][j]= (ABS(m[i][j]) < eps) ? 0 : m[i][j];
  return tcomp(dest);
}


    
//---------------------------------------------------------------------------
// For each row?column? in the 3x3 part of the mtrx, force sum of squares of
// elements to be 1; necessary b/c of fp roundoff error
 tcomp tcomp::Clamped2() {
  Mtrx();    // update m
  fourby3 dest;

  forij(3) { // column
    double squareSum= m[0][j]*m[0][j] +m[1][j]*m[1][j] +m[2][j]*m[2][j];
    forii(3)  // row
      dest[i][j]= m[i][j] / sqrt(squareSum);
  }
  forii(3) { // row
    double squareSum= dest[i][0]*dest[i][0] +dest[i][1]*dest[i][1] +dest[i][2]*dest[i][2];
    forij(3)  // row
      dest[i][j]= dest[i][j] / sqrt(squareSum);
  }
  dest[3][0]= m[3][0];
  dest[3][1]= m[3][1];
  dest[3][2]= m[3][2];
  return tcomp(dest);
}

    
//---------------------------------------------------------------------------
// Clamp elements to a resolution of 10e-9in 10^9 ...necessary b/c of fp 
// roundoff error
tcomp tcomp::Clamped1() {
  const double eps= 1e-9; // epsilon
  Mtrx();    // update m
  fourby3 dest;
  forii(4)    // row;  notice: 4, not 3
    forij(3)  // column
      dest[i][j]= floor(m[i][j]/eps+0.5)*eps;
  return tcomp(dest);
}

#endif

    
//---------------------------------------------------------------------------
ostream& operator<<(ostream& o, const tcomp& t) {
  return o << setprecision(4)
	   << "cart: " << t.cart
	   << "  ang: " << t.ang
	   << "  scale: " << t.scale << "\n";
}
