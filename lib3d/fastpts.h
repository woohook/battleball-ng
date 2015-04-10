// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef FASTPTS_h
#define FASTPTS_h


#include <iostream>
#include "general.h"
#include "table.h"
#include "xform.h"
#include "dimentable.h"

using namespace std;

/*=========================================================================*/
struct pt3dPtr {
  dimension *x, *y, *z;
};

typedef table<pt3dPtr> pt3dPtrTable;


/*=========================================================================*/
// OK, here's the mission:  if FASTPTS_TEMPLATE is defined, the fastPts class
// is a template, otherwise it's not.  g++ 2.7.2 and earlier can not handle
// fastpts being a template.


#ifdef FASTPTS_TEMPLATE
#define FASTPTS_TEMPL_DECL template<class pnt,class destPnt>
#define FASTPTS FastPts<pnt,destPnt>
#else
#define FASTPTS_TEMPL_DECL
#define FASTPTS FastPts
#endif

FASTPTS_TEMPL_DECL
struct FastPts {
#ifndef FASTPTS_TEMPLATE
  typedef pt3d pnt;
  typedef pt3d destPnt;
#endif
  typedef pnt::coord coord;
  //enum {numDims=pnt::numDims};
  friend ostream& operator<<(ostream&,const FASTPTS&);

  private:
  typedef table<pnt> pntTable;

  pntTable	pts;		// points
  pt3dPtrTable	ptPtrs;		// point ptrs; each pt ptr corresponds
				// to one point in 'pts', and points to that
				// point's dimensions in the dimenTable's
  dimenTable	xdims,		// all the X dimensions of the points in 'pts'
  		ydims,		// all the Y dimensions of the points in 'pts'
  		zdims;		// all the Z dimensions of the points in 'pts'
  dimenTable    sdims;          // symmetric dimensions

 public:
  FastPts() {};
  FastPts(const FASTPTS& fp) {Add(fp.pts);}  //copy ctor
  FASTPTS&
  operator=(const FASTPTS& fp);              //assignment op

  void	TransformTo(const tmtrx& m, destPnt destPts[]);
  int	Add(const pnt&);
  void  Add(const pntTable&);
  int	Num() const		{return pts.Num();}
  pnt	*Array() const		{return pts.Array();}
  pnt&	operator[](int i) const	{return pts[i];}
  pntTable& getTable()		{return pts;}
};

#ifdef FASTPTS_TEMPLATE
typedef FastPts<pt3d,pt3d> fastPts;
#else
typedef FastPts fastPts;
#endif

#endif
