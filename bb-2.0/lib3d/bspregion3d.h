// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef BSPREGION3D_h
#define BSPREGION3D_h


#include "bsp.h"
#include "general.h"
#include "table.h"
#include "xpanel3d.h"
#include "region3d.h"



/************************************************************************/
// 3D shape class with a BSP tree representation.


typedef table<Polygon3f> polyTable;
typedef table<Polygon3f*> polyPtrTable;


struct bspRegion3d : region3d {
  int			numOrigPts;     /* number of pts in region before
					   extra points were introduced due
					   to BSP polygon splitting.  pts[0]
					   through pts[numOrigPts-1] should
					   correspond to the points with
					   which the user defined the
					   region (except that duplicate
					   points are collapsed).  (If the
					   region was defined from a BSP
					   tree, then numOrigPts==
					   pts.Num().) */
  BSP			*bspTree;	/* BSP tree representation of the
					   region; after this bspRegion3d is
					   created, the material_index field
					   of bspTree's Polygon3f's will
					   point to the xpanel3d's in
					   bspRegion3d::treePanels */
  BSP_TREE_TYPE		treeType;	/* BSP tree type, as defined by the
					   BSP library */

 private:
  panelList		treePanels;	/* xpanel3d's which correspond to the
					   Polygon3f's in bspTree */
  bool			bspTreeIsMyCopy;/* true ->  bspTree is a private
					   to this object, so destroy it
					   when this object's destructor
					   is called;
					   false -> don't destroy bspTree
					   when this object's destructor
					   is called */
  

 public:
        bspRegion3d(BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY);
  	bspRegion3d(coord *srcPts, int *srcPanelData, coord sc=1.0,
		    BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY);
        bspRegion3d(coord *srcPts, int *srcPanelData, const pt3d& sc,
		    BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY);
        bspRegion3d(pt3d  *srcPts, int *srcPanelData,
		    const pt3d& sc=pt3d(1,1,1),
		    BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY);
        bspRegion3d(BSP *tree, BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY,
		    bool wantCopy=true, bool wantEdges=true);
        bspRegion3d(const bspRegion3d&);  // copy ctor
  bspRegion3d&
        operator=(const bspRegion3d&); // assignment op

        ~bspRegion3d() {if (bspTree and bspTreeIsMyCopy)
			bsp_destroy(bspTree);}

  int	Add(const xpanel3d&);
  void	Add(const bspRegion3d& r, const pt3d& offset=pt3d(0,0,0),
	    const ang3d& ang=ang3d(0), const pt3d& scale=pt3d(1,1,1));
  bool	Contains(const pt3d& p, bool surfaceIsIn=true);

 private:
  const list<xpanel3d>& Init(BSP *tree, BSP_TREE_TYPE tt, bool wantCopy);
  void	InitBSP();
  void	MakeBspTreePanels(BSP*);
};


/* WARNING:  multiple bspRegion3d's can't share a common BSP
   tree, because each bspRegion3d insists that the material_index fields in
   the BSP tree point to that bspRegion3d's own treePanels.

   In other words, this sort of client code could get you into trouble:

   { BSP *tree;
     .
     .
     bspRegion3d rgn1(tree,BSP_TREE_SOLID,false);    // ok
     bspRegion3d rgn2(tree,BSP_TREE_SOLID,false);    // not ok
     bspRegion3d rgn3(tree,BSP_TREE_SOLID,false);    // not ok
   }

   The solution for now is to demand private copies of the BSP tree for
   bspRegion3d's after the first bspRegion3d, or just always to demand
   private copies.
*/


#endif
