// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include <stdlib.h>
#include "bspregion3d.h"



/*=========================================================================*/
// Constructors galore:

bspRegion3d::bspRegion3d(BSP_TREE_TYPE tt) :
  numOrigPts(0),
  bspTree(NULL),
  treeType(tt),
  bspTreeIsMyCopy(true) {}

bspRegion3d::bspRegion3d(coord *srcPts, int *srcPanelData, coord scale,
			 BSP_TREE_TYPE tt) :
  region3d(srcPts,srcPanelData,scale), // base class ctors always called 1st
  numOrigPts(pts.Num()),
  treeType(tt)
{InitBSP();}

bspRegion3d::bspRegion3d(coord *srcPts, int *srcPanelData, const pt3d& scale,
			 BSP_TREE_TYPE tt) :
  region3d(srcPts,srcPanelData,scale), // base class ctors always called 1st
  numOrigPts(pts.Num()),
  treeType(tt)
{InitBSP();}

bspRegion3d::bspRegion3d(pt3d *srcPts, int *srcPanelData, const pt3d& scale,
			 BSP_TREE_TYPE tt) :
  region3d(srcPts,srcPanelData,scale), // base class ctors always called 1st
  numOrigPts(pts.Num()),
  treeType(tt)
{InitBSP();}

bspRegion3d::bspRegion3d(BSP *tree, BSP_TREE_TYPE tt, bool wantCopy,
                         bool wantEdges) :
  region3d(Init(tree,tt,wantCopy),wantEdges),
  numOrigPts(0) {}

// copy ctor always makes private copy of tree - see warning in bspregion3d.h
bspRegion3d::bspRegion3d(const bspRegion3d& r) :
  region3d(r),
  numOrigPts(r.numOrigPts) {
  Init(r.bspTree,r.treeType,true);
}

// assignment op always makes private copy of tree - see warning in
// bspregion3d.h
bspRegion3d& bspRegion3d::operator=(const bspRegion3d& r) {
  // free contents of this bspRegion3d
  if (bspTree and bspTreeIsMyCopy) bsp_destroy(bspTree);
  //(&treePanels)->~panelList();
  treePanels.erase(treePanels.begin(),treePanels.end());

  // copy in new bspRegion3d
  region3d::operator=(r);
  numOrigPts= r.numOrigPts;
  Init(r.bspTree,r.treeType,true);
  return *this;
}


/*-------------------------------------------------------------------------*/
/* In:  tree      = bsp tree
        tt        = bsp tree type
        wantCopy  = true -> make a private copy of tree to use with this
                            bspRegion3d, and set bspTreeIsMyCopy to true
                    false-> use the tree pointed to by 'tree'.  Assumes that
                            tree exists as long as this bspRegion3d exists.
*/
const list<xpanel3d>& bspRegion3d::Init(BSP *tree, BSP_TREE_TYPE tt,
					bool wantCopy) {
  treeType= tt;
  bspTreeIsMyCopy= wantCopy;
  if (bspTreeIsMyCopy) {
    bspTree= bsp_build(0,NULL,treeType);
    bsp_copy(tree,bspTree);
  }
  else
    bspTree= tree;
  MakeBspTreePanels(bspTree);
  return treePanels;
}


/*=========================================================================*/
void bspRegion3d::MakeBspTreePanels(BSP *bspt) {
  if (bspt)
  { forii(bspt->numpolys)
    { Polygon3f *poly= bspt->polys[i];
      xpanel3d& panel= * (xpanel3d *) poly->material_index;

      poly->material_index=
	(int) & *treePanels.insert(treePanels.end(),xpanel3d(*poly,pts,panel));
    }
    MakeBspTreePanels(bspt->negative);
    MakeBspTreePanels(bspt->positive);
  }
}


/*-------------------------------------------------------------------------*/
void bspRegion3d::InitBSP() {
  // create a BSP tree corresponding to this region3d

  // convert each panel3d to a polygon3f, and build an array of ptrs to the
  // polygon3f's.  Each polygon3f must be _individually_ allocated by
  // _malloc_
  polyPtrTable	polyPtrs;
  forii(panels.Num()) {
    Polygon3f *p= (Polygon3f*) malloc(sizeof(Polygon3f));
    *p= panels[i].asPolygon3f(pts.Array()); //make stupid xlC happy!
    //PolygonCopy3f(&panels[i].asPolygon3f(pts.Array()),p);
    polyPtrs.Add(p);
  }

  bspTree= bsp_build(polyPtrs.Num(),polyPtrs.Array(),treeType);
  bspTreeIsMyCopy= true;

  // create new panels corresponding to the bsp tree's split Polygon3fs
  MakeBspTreePanels(bspTree);
}


/*-------------------------------------------------------------------------*/
// Adds an xpanel3d to this region.
// Nothing is added if the xpanel3d is empty (i.e. xpanel3d.ptNums.Num()==0)
int bspRegion3d::Add(const xpanel3d& p) {
  int panelNum= region3d::Add(p);
  if (bspTree and bspTreeIsMyCopy)
    bsp_destroy(bspTree);
  bspTree= NULL;
  treePanels.erase(treePanels.begin(),treePanels.end());
  InitBSP();
  return panelNum;
}


/*-------------------------------------------------------------------------*/
// Adds a region to this region.
void bspRegion3d::Add(const bspRegion3d& rgn, const pt3d& offset,
		      const ang3d &ang, const pt3d& scale)
{ region3d::Add(rgn,offset,ang,scale);
  if (bspTree and bspTreeIsMyCopy)
    bsp_destroy(bspTree);
  bspTree= NULL;
  treePanels.erase(treePanels.begin(),treePanels.end());
  InitBSP();
}


/*-------------------------------------------------------------------------*/
bool bspRegion3d::Contains(const pt3d& p, bool surfaceIsIn) {
  Vec3f vec;
  pt3d2Vec3f(p,vec);
  return bsp_contains(bspTree,vec,surfaceIsIn);
}
