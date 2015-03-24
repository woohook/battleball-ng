// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef REGION3D_h
#define REGION3D_h


#include <iostream.h>
#include <list.h>   // STL
#include "general.h"
#include "xform.h"
#include "region2d.h"
#include "fastpts.h"
#include "rect3d.h"
#include "table.h"
#include "edgetable.h"
#include "xpanel3d.h"


extern const pt3d::coord COORD_MAX;


#ifdef __GNUC__
inline void region3dDummyInstantiator() {
//  fastPts<pt3d,pt3d> fp1,fp2;
//  fp1.fastPts<pt3d,pt3d>::operator=(fp2);
  table<edge> edges;
}
#endif


/*=========================================================================*/
// 3D shape "boundary-rep" class.
// Allows point containment testing, bounding box / bounding sphere
// computation, and (most importantly) fast geometric transformation.

struct region3d {
  typedef pt3d::coord coord;
  typedef list<xpanel3d> panelList;

  friend ostream& operator<<(ostream&, const region3d&);

  fastPts       pts;                    // a collection of 3d points which
                                        // can be transformed very quickly
 protected:
  table<xpanel3d> panels;               // 3d panels
  coord         farthestDist;           // center-to-farthest-pt dist
  edgeTable     edges;                  // edges

 private:
  rect3d        box;                    // bounding box wholly enclosing rgn


 public:
        region3d() {};
        region3d(coord *srcPts, int *srcPanels, coord sc=1.0);
        region3d(coord *srcPts, int *srcPanels, const pt3d& sc);
        region3d(pt3d  *srcPts, int *srcPanels, const pt3d& sc=pt3d(1,1,1));
        region3d(const panelList& otherPanels, bool wantEdges=true);

  // default copy ctor and default assignment op are adequate

  const
  rect3d& Box() const {return box;}
  const
  table<xpanel3d>& Panels() const {return panels;}
  coord FarthestDist() const {return farthestDist;}

  bool  InFrustrum(const pt3d&,const pt3d[],const tmtrx&) const;
  void  TransformTo(const tmtrx& m, pt3d destPts[])
         {pts.TransformTo(m,destPts);}
  int   Add(const xpanel3d&);
  void  Add(const region3d& rgn, const pt3d& offset=pt3d(0,0,0),
            const ang3d& ang=ang3d(0), const pt3d& sc=pt3d(1,1,1));

  bool  Contains(const pt3d& p) const
          {return box.Contains(p);}
  bool  ContainsExclusively(const pt3d& p) const
          {return box.ContainsExclusively(p);}
  bool  ContainsInclusively(const pt3d& p) const
          {return box.ContainsInclusively(p);}

 private:
  void  InitPanelsEtc(int *srcPanelData, const pt3d& sc);
};


pt3d* RegularPolygon(int numSides, pt3d::coord dist, pt3d *pts);

#endif
