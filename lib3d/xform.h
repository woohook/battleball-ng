// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef TRANSFORM_h
#define TRANSFORM_h


#include <iostream>
#include "general.h"
#include "pt3d.h"
#include "ang3d.h"

using namespace std;

//==========================================================================
/* Transformation classes tmtrx and tcomp.

   These classes each represent an affine transformation.  An affine
   transformation is a combination of geometric transformations like scaling,
   rotation, and translation.  Transformations can be applied to points and
   shapes to scale/rotate/move them.

   tcomp:
   a transformation implemented by discrete components:  a scaling factor, a
   rotation angle, and an offset.  Applying a tcomp to a point scales it,
   rotates it, and then translates it, in that order.

   tmtrx:
   a transformation implemented by a 4x4 matrix.  A 4x4 matrix can represent
   any number of scalings/rotations/translations, applied in any order.
   Applying a tmtrx to a point performs matrix multiplication between the
   point and the 4x4 matrix.

   WARNING:  several member functions of these classes do not support
   scaling; i.e. they assume unity scaling.  See the individual functions for
   warnings.

   For both tcomp and tmtrx,
   o  Cart() returns the cartesian translational component
   o  Ang()  returns the angular rotation component (has a limitation)

   Since tcomp and tmtrx represent the same thing, their constructors are
   designed to allow conversions between them.  This often makes tcomp's and
   tmtrx's interchangable in client code.  (tcomp::tcomp(tmtrx&) has a
   limitation)
*/


class tmtrx;  // forward reference

//----------------------------------------------------------------------------
class tcomp {
  pt3d  cart;
  ang3d ang;
  pt3d  scale;

 public:
  tcomp(ang2d a=0) : cart(pt3d(0,0,0)), ang(ang3d(a)), scale(pt3d(1,1,1)) {}
  tcomp(const ang3d& a) : cart(pt3d(0,0,0)), ang(a), scale(pt3d(1,1,1)) {}
  tcomp(const pt3d& c, const ang3d& a=ang3d(0),	const pt3d& s=pt3d(1,1,1))
    : cart(c), ang(a), scale(s) {}
  tcomp(const tmtrx& t);  // Warning: assumes t has unity scaling

  pt3d& Cart() {return cart;}
  ang3d& Ang() {return ang;}
  pt3d& Scale() {return scale;}
  const pt3d& Cart() const {return cart;}
  const ang3d& Ang() const {return ang;}
  const pt3d& Scale() const {return scale;}

  tcomp& operator+=(tcomp& t);
  tcomp& operator-=(tcomp& t);
  tcomp	operator- ()         {return tcomp(-Cart(),-Ang(),-Scale());}
  tcomp	operator+ (tcomp& t) {return tcomp(Cart()+t.Cart(),Ang()+t.Ang(),Scale()+t.Scale());}
  tcomp	operator- (tcomp& t) {return tcomp(Cart()-t.Cart(),Ang()-t.Ang(),Scale()-t.Scale());}
  //tcomp Plus(tcomp&);
  //tcomp Plus2(tcomp&);
  //tcomp Clamped1();
  //tcomp Clamped2();

// replace next two funcs by plain max() & min() someday -PAH
  void	SetMax(tcomp& t) {Cart().SetMax(t.Cart()); Ang().SetMax(t.Ang());}
  void	SetMin(tcomp& t) {Cart().SetMin(t.Cart()); Ang().SetMin(t.Ang());}

  friend ostream& operator<<(ostream&, const tcomp&);
};

//----------------------------------------------------------------------------
class tmtrx {
  typedef double fourby3[4][3];
  fourby3 m;

 public:
  tmtrx() {}
  tmtrx(const tmtrx& t) {Copy(t.m);}
  tmtrx(const fourby3 newm) {Copy(newm);}
  tmtrx(const tcomp& t) {SetMtrx(t.Ang(),t.Scale()); Cart()=t.Cart();}
  tmtrx(ang2d a)        {SetMtrx(ang3d(a,0,0)); Cart()= pt3d(0,0,0);}
  tmtrx(const ang3d& a) {SetMtrx(a); Cart()= pt3d(0,0,0);}
  tmtrx(const pt3d& c, const ang3d& a=ang3d(0), const pt3d& sc=pt3d(1,1,1)) {
    SetMtrx(a,sc);
    Cart()=c;
  }

  pt3d&  Cart() const {return *((pt3d*) m[3]);}
  ang3d  Ang() const;
  double *operator[](int i) {return m[i];}
  const double *operator[](int i) const {return m[i];}
  tmtrx& operator= (const tmtrx& t) {if (this != &t) Copy(t.m); return *this;}
  bool	 operator==(const tmtrx& t) const;
  tmtrx  operator* (const tmtrx&) const;
  tmtrx  Inverse() const;
  bool   RotationSame(const tmtrx& t) const;

 private:
  void  SetMtrx(const ang3d& a, const pt3d& s=pt3d(1,1,1));
  void  Copy(const fourby3);

  friend ostream& operator<<(ostream&,const tmtrx&);
};

inline
tmtrx operator*(const tcomp& tc, const tmtrx& tm) {return tmtrx(tc) * tm;}


//===========================================================================

// these could easily be templatized to work with both tmtrx or double[4][4]
pt3d operator*(const pt3d& p, const tmtrx& t);
pt3d operator/(const pt3d& p, const tmtrx& t);
inline pt3d operator>>(const pt3d& p, const tmtrx& t) {return p*t;}
inline pt3d operator<<(const pt3d& p, const tmtrx& t) {return p/t;}


#endif
