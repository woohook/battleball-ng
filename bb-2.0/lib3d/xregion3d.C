// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include "xregion3d.h"


/************************************************************************/
const xregion3d::coord
  lowx=  -32768.0,
  highx= 32767.0,
  lowy=  -32768.0;
//  highy= 32767.0; -PAH


/*----------------------------------------------------------------------*/
/*Out:  (xcode&1) = 1:  some x < lowx
        (xcode&2) = 1:  some x < 0 <=        < clip.x
        (xcode&4) = 1:           0 <= some x < clip.x
        (xcode&8) = 1:           0 <=        < clip.x <= some x
        (xcode&16)= 1:           0 <=        < clip.x < highx <= some x
        (xcode&32)= 1:  some pt3ds were behind viewplane

        (ycode&1) = 1:  some y < lowy
        (ycode&2) = 1:  some y < 0 <=        < clip.y
        (ycode&4) = 1:           0 <= some y < clip.y
        (ycode&8) = 1:           0 <=        < clip.y <= some y
        (ycode&16)= 1:           0 <=        < clip.y < highy <= some y
*/
void xregion3d::Project(const pt3d viewPts[], const pt2d& clip, pt2d prjPts[],
                        int& xcode, int& ycode) {
  pt2d  *prjPt;
  coord highy= clip.y;        // boy is this a kludge or what? -PAH
  pt2d  windowCenter= clip/2;

  xcode= ycode= 0;
  forii(pts.Num()) {
    prjPt= &prjPts[i];
    if (viewPts[i].x <MINDIST)
      xcode |= 32;
    else {
      *prjPt= viewPts[i].Project() +windowCenter;
      xcode |= 1 << ((prjPt->x >=lowx) +
                     (prjPt->x >=0) +
                     (prjPt->x >=clip.x) +
                     (prjPt->x >=highx));
      ycode |= 1 << ((prjPt->y >=lowy) +
                     (prjPt->y >=0) +
                     (prjPt->y >=clip.y) +
                     (prjPt->y >=highy));
    }
  }
}


/*----------------------------------------------------------------------*/
void xregion3d::DrawEdges(const pt3d viewPts[], const pt2d& clip,
			  const pt2d prjPts[], int xcode, int ycode,
			  const gfxTarget& gt) {
  int i;
  int numSegs= 0;
  static XSegment segs[1000]; //-PAH

  if ((xcode&49)==0 and (ycode&17)==0) { // if all pts were w/in drawable,
    for(i=0; i <edges.Num(); i++)        // don't bother to clip, just draw
    { segs[i].x1= (short) prjPts[edges[i].beginPtNum].x;
      segs[i].y1= (short) prjPts[edges[i].beginPtNum].y;
      segs[i].x2= (short) prjPts[edges[i].endPtNum  ].x;
      segs[i].y2= (short) prjPts[edges[i].endPtNum  ].y;
    }
    XDrawSegments(gt.disp,gt.win,gt.gc,segs,edges.Num());
    return;
  }

  coord highy= clip.y;        // boy is this a kludge or what? -PAH
  pt2d  beginPt2d;
  pt2d  endPt2d;
  pt2d  windowCenter= clip/2;

  if ((ycode&17)==0 and (xcode&32)==0) { // if all y coords were in drawable,
    for(i=0; i <edges.Num(); i++) {
      beginPt2d= prjPts[edges[i].beginPtNum];
      endPt2d=   prjPts[edges[i].endPtNum];

      // Clip X coords
      if (beginPt2d.x <0) {
	if (endPt2d.x <0) continue;
	if (beginPt2d.x <lowx)
	  Get2dXCrossing(beginPt2d,endPt2d,0);
	if (endPt2d.x >=highx)
	  Get2dXCrossing(endPt2d,beginPt2d,clip.x-1);
      }
      else if (beginPt2d.x <clip.x) {
	if (endPt2d.x <lowx)
	  Get2dXCrossing(endPt2d,beginPt2d,0);
	else if (endPt2d.x >=highx)
	  Get2dXCrossing(endPt2d,beginPt2d,clip.x-1);
      }
      else {        // beginPt2d.x is >= clip.x
	if (endPt2d.x >=clip.x) continue;
	if (beginPt2d.x >highx)
	  Get2dXCrossing(beginPt2d,endPt2d,clip.x-1);
	if (endPt2d.x <lowx)
	  Get2dXCrossing(endPt2d,beginPt2d,0);
      }
      
      segs[numSegs].x1= (short) beginPt2d.x;
      segs[numSegs].y1= (short) beginPt2d.y;
      segs[numSegs].x2= (short) endPt2d.x;
      segs[numSegs].y2= (short) endPt2d.y;
      numSegs++;
    }
    XDrawSegments(gt.disp,gt.win,gt.gc,segs,numSegs);
    return;
  }


  for(i=0; i <edges.Num(); i++) {
    int beginNum= edges[i].beginPtNum;
    int endNum=   edges[i].endPtNum;
    pt3d beginPt= viewPts[beginNum];
    pt3d endPt=   viewPts[endNum];


    // Check if in front of view plane
    if (beginPt.x >= MINDIST)
      if (endPt.x >= MINDIST)
      { beginPt2d= prjPts[beginNum];
        endPt2d=   prjPts[endNum];
      }
      else
      { beginPt2d= prjPts[beginNum];
        endPt.GetXCrossing(beginPt,MINDIST);
        endPt2d= endPt.Project() +windowCenter;
      }
    else
      if (endPt.x >= MINDIST)
      { beginPt.GetXCrossing(endPt,MINDIST);
        beginPt2d= beginPt.Project() +windowCenter;
        endPt2d= prjPts[endNum];
      }
      else
        continue;       // both pts behind view plane


    // Clip X coords
    if (beginPt2d.x <0)
    { if (endPt2d.x <0) continue;
      if (beginPt2d.x <lowx)
        Get2dXCrossing(beginPt2d,endPt2d,0);
      if (endPt2d.x >=highx)
        Get2dXCrossing(endPt2d,beginPt2d,clip.x-1);
    }
    else if (beginPt2d.x <clip.x)
    { if (endPt2d.x <lowx)
        Get2dXCrossing(endPt2d,beginPt2d,0);
      else if (endPt2d.x >=highx)
        Get2dXCrossing(endPt2d,beginPt2d,clip.x-1);
    }
    else        // beginPt2d.x is >= clip.x
    { if (endPt2d.x >=clip.x) continue;
      if (beginPt2d.x >highx)
        Get2dXCrossing(beginPt2d,endPt2d,clip.x-1);
      if (endPt2d.x <lowx)
        Get2dXCrossing(endPt2d,beginPt2d,0);
    }

    // Clip Y coords
    if (beginPt2d.y <0)
    { if (endPt2d.y <0) continue;
      if (beginPt2d.y <lowy)
        Get2dYCrossing(beginPt2d,endPt2d,0);
      if (endPt2d.y >=highy)
        Get2dYCrossing(endPt2d,beginPt2d,clip.y-1);
    }
    else if (beginPt2d.y <clip.y-1)
    { if (endPt2d.y <lowy)
        Get2dYCrossing(endPt2d,beginPt2d,0);
      else if (endPt2d.y >=highy)
        Get2dYCrossing(endPt2d,beginPt2d,clip.y-1);
    }
    else        // beginPt2d.y is >= clip.y-1
    { if (endPt2d.y >=clip.y-1) continue;
      if (beginPt2d.y >=highy)
        Get2dYCrossing(beginPt2d,endPt2d,clip.y-1);
      if (endPt2d.y <lowy)
        Get2dYCrossing(endPt2d,beginPt2d,0);
    }

    segs[numSegs].x1= (short) beginPt2d.x;
    segs[numSegs].y1= (short) beginPt2d.y;
    segs[numSegs].x2= (short) endPt2d.x;
    segs[numSegs].y2= (short) endPt2d.y;
    numSegs++;
  }
  XDrawSegments(gt.disp,gt.win,gt.gc,segs,numSegs);
}


/*----------------------------------------------------------------------*/
struct bspStack {
  BSP* node;
  BSP* next;
  bool facesViewer;
};

void xregion3d::DrawPolys(const pt3d viewPts[], const pt2d& clip,
			  const pt2d prjPts[], int xcode, int ycode,
			  const ulong colors[], const gfxTarget& gt,
			  const pt3d& viewPos) {
  bool fast= (xcode&49)==0 and (ycode&17)==0; //if all pts are w/in drawable
  
  static XPoint xprjPts[1000]; //-PAH
  if (fast)
    forii(pts.Num()) {
    xprjPts[i].x= (short) prjPts[i].x;
    xprjPts[i].y= (short) prjPts[i].y;
  }
  
  Vec3f viewPos3f;
  pt3d2Vec3f(viewPos,viewPos3f);

  static bspStack stack[1000];  // handles tree up to 1000 nodes deep
  bspStack* sp= stack;  // stack pointer
  BSP *tree= bspTree;

  while (true) {
    if (tree==NULL) {
      if (sp==stack) return;
      sp--;
      for (int i=0; i < sp->node->numpolys; i++) {
	Polygon3f* poly= sp->node->polys[i];
	xpanel3d* panel= (xpanel3d*) poly->material_index;
	if (sp->facesViewer==poly->facesForward /*panel->Faces(pts.Array(),viewPos)*/ or panel->doubleSided)
	  if (fast)
	    panel->DrawFast(xprjPts,colors,gt);
	  else
	    panel->Draw(viewPts,clip,prjPts,colors,gt);
      }
      tree= sp->next;

    } else {

      sp->node= tree;
      sp->facesViewer= Vdot3f(viewPos3f, tree->coeffs) > -tree->coeffs[3];
      if (sp->facesViewer) {
	sp->next= tree->positive;
	tree= tree->negative;
      } else {
	sp->next= tree->negative;
	tree= tree->positive;
      }
      sp++;
    }
  }
}


/*----------------------------------------------------------------------*/
/* In:  pts = the points of this region, in view coordinates
        viewPos = view point cartesian position, in model coordinates
*/
void xregion3d::Draw(const pt3d viewPts[], const gfxTarget& gt,
		     const pt3d& viewPos,
                     const pt2d& clip, bool solid, const ulong colors[]) {
  static pt2d prjPts[1000]; //-PAH
  int xcode, ycode;
  Project(viewPts, clip, prjPts, xcode, ycode);
  if ((xcode & 60)==0 or                // if all pts were to left, or
      (xcode & 39)==0 or                // all pts were to right, or
      xcode==32 or                      // all pts were behind, or
      ((ycode & 60)==0 or               // all pts were above, or
       (ycode & 39)==0) and
      (xcode & 32)==0)                  // all pts were below, then
    return;                             // don't bother to draw anything
  
  if (solid)
    DrawPolys(viewPts,clip,prjPts,xcode,ycode,colors,gt,viewPos);
  else
    DrawEdges(viewPts,clip,prjPts,xcode,ycode,gt);
}


/*=========================================================================*/
/* In:  exactFirst = true -> init rgns[0] to correspond exactly to firstPts &
                             firstScale (no morphing of rgns[0])
                     false-> init rgns[0] to be the first morphed shape,
                             corresponding most nearly to firstPts &
                             firstScale
        exactLast  = similar to exactFirst, but instead with respect to
                     rgns[numRgns-1], lastPts, & lastScale
*/
void InitMorphRegion3ds(xregion3d rgns[], int numRgns,
                        xregion3d::coord *firstPts,
			xregion3d::coord firstScale,
                        xregion3d::coord *lastPts,
			xregion3d::coord lastScale, int *panels,
                        bool exactFirst, bool exactLast)
{ table<xregion3d::coord> ptsData;

  forii(numRgns) {
    xregion3d::coord *pts1= firstPts;
    xregion3d::coord *pts2= lastPts;
    ptsData.Empty();
    while (*pts1 != COORD_MAX) {
      pt3d pt;
      pt.Interpolate(pt3d(pts1)*firstScale,
                     pt3d(pts2)*lastScale,
                     i +(not exactFirst),
                     numRgns -1 +(not exactFirst) +(not exactLast));
      ptsData.Add(pt.x);
      ptsData.Add(pt.y);
      ptsData.Add(pt.z);
    }
    ptsData.Add(COORD_MAX);
    rgns[i]= xregion3d(ptsData.Array(),panels);
  }
}
