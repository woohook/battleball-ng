// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include <math.h>
#include <limits.h>  // to get INT_MAX
#include "region3d.h"



/*=========================================================================*/
region3d::region3d(coord *srcPts, int *srcPanels, coord scale) {
#ifdef __GNUC__
  //  cout << "srcPts=" << srcPts << " *=" << *srcPts << " srcPanels=" << srcPanels << " *=" << *srcPanels << " scale=" << scale << endl;
#endif
  while (*srcPts != COORD_MAX)
    pts.Add(pt3d(srcPts)*scale);
  InitPanelsEtc(srcPanels,pt3d(scale,scale,scale));
}

region3d::region3d(coord *srcPts, int *srcPanels, const pt3d& scale) {
  while (*srcPts != COORD_MAX)
    pts.Add(pt3d(srcPts)*scale);
  InitPanelsEtc(srcPanels,scale);
}

region3d::region3d(pt3d *srcPts, int *srcPanels, const pt3d& scale) {
  while (srcPts->x != COORD_MAX)
    pts.Add(pt3d(srcPts)*scale);
  InitPanelsEtc(srcPanels,scale);
}

/* wantEdges = true -> fill the edgeTable with the edges from the
                       polygons in tree
               false-> don't add anything to the edgeTable
                       (which saves time if the edgeTable isn't needed)
   This funny little routine allows initialization of some of a region3d's
   fields, from an external list of panels.  Assumes that this->pts is
   already defined.
*/
region3d::region3d(const panelList& otherPanels, bool wantEdges) {
  panelList::const_iterator p;
  if (wantEdges)
    for(p= otherPanels.begin(); p != otherPanels.end(); p++)
      edges.AddPanel(*p);
  box.MakeBoundingBox(pts.getTable(),farthestDist);
}


/*-------------------------------------------------------------------------*/
// In:  viewPos = viewer's eye position, in world coords
//      frustrumNormals = four normals of planes bounding the view frustrum;
//                        where normals point _into_ the viewable space.
//                        Note that to get the plane equations,
//                        viewPos must be added to these normals.
//      worldPos = model-to-world transform for this region3d
// Returns true if any part of this region3d may be viewable in the
// view frustrum.
bool region3d::InFrustrum(const pt3d& viewPos,
			  const pt3d frustrumNormals[],
			  const tmtrx& worldPos) const {
  // vector from eye pos to center of this region3d, in world coords
  pt3d eyeToThis= (Box().Center() >> worldPos) - viewPos;

  forii(4) {
    coord distInsideFrustrum= eyeToThis.Dot(frustrumNormals[i]);
    if (distInsideFrustrum +FarthestDist() <0)
      return false;
    }
  return true;
}


/*-------------------------------------------------------------------------*/
/* Adds an xpanel3d to this region3d.
   Assumes that the panel's point numbers refer to points that already
   exist in this->pts.
   No panel is added if the xpanel3d is empty (i.e. xpanel3d.ptNums.Num()==0)
   Returns the panel number of the added panel, or -1 if the panel wasn't
   added.
*/
int region3d::Add(const xpanel3d& p) {
if (p.ptNums.Num() >0) {
    edges.AddPanel(p);
    box.MakeBoundingBox(pts.getTable(), farthestDist);
    return panels.Add(p);
  }
  else
    return -1;
}


/*-------------------------------------------------------------------------*/
/* Adds a src region3d to this region3d.
   Nothing is added if the src region3d is empty (i.e. rgn.panels.Num()==0).
   The src region3d is (1) scaled, (2) rotated, and (3) offset, in that
   order, before being added to the this region3d.
*/
void region3d::Add(const region3d &rgn, const pt3d& offset, const ang3d &ang,
                   const pt3d& scale) {
forii(rgn.panels.Num())
    Add(xpanel3d(rgn.panels[i],rgn.pts.Array(),pts,offset,ang,scale));
}


/*-------------------------------------------------------------------------*/
/* In:  srcPanelData = an array of integers which define the panels of this
                       region3d; terminated by INT_MAX
        scale = needed only to ensure that the panels' normals point in the
	        correct direction
*/
void region3d::InitPanelsEtc(int *srcPanelData, const pt3d& sc) {
  bool shouldReverse= false;  // reverse the directions that the panels face
  if (sc.x <0) shouldReverse= ! shouldReverse;
  if (sc.y <0) shouldReverse= ! shouldReverse;
  if (sc.z <0) shouldReverse= ! shouldReverse;

  // So... reverse the directions already!  how? -PAH

  while (*srcPanelData != INT_MAX) {
    panels.Add(xpanel3d(srcPanelData,&pts[0])); // updates srcPanelData ptr
    //panels[panels.Num()-1].normal *= normalDir;
    edges.AddPanel(panels[panels.Num()-1]);
  }
  box.MakeBoundingBox(pts.getTable(),farthestDist);
}


/*-------------------------------------------------------------------------*/
// Should this function go in pt3d.C?
// Writes into the array 'pts' a sequence of 'numSides' points which form
// a regular polygon; dist = distance from poly center to each side's center
// Returns a pointer just past the end of the last point written
pt3d* RegularPolygon(int numSides, pt3d::coord dist, pt3d *pts) {
  double innerAng= M_PI_2 - M_PI/numSides;
  pt3d::coord distToCorner= dist/sin(innerAng);

/* Using this code, instead of the code below, elicits a compiler bug
   in g++ 2.7.2.1 when optimizing with -O2 -funroll-loops on a Pentium.

  forii(numSides) {
    double ang= -M_PI_2
           +(2*M_PI/numSides)/2
           +(i * 2*M_PI)/numSides;
    *tempPts = pt3d(distToCorner,0,0) >> ang;
    tempPts++;
  }
*/

  pt3d *tempPts= pts;
  pt3d *ptsEnd= pts +numSides;
  while (tempPts != ptsEnd) {
    double ang= -M_PI_2
           +(2*M_PI/numSides)/2
           +((tempPts-pts) * 2*M_PI)/numSides;
    *tempPts = pt3d(distToCorner,0,0) >> ang;
    tempPts++;
  }

  return tempPts;
}

/*-------------------------------------------------------------------------*/
ostream& operator<< (ostream& out, const region3d& r) {
  return out << "panels:" << r.panels.Num() << "  edges:" << r.edges.Num() << "\n" << r.pts;
}
