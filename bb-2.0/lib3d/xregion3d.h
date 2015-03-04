// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef XREGION3D_h
#define XREGION3D_h


//#include <stdlib.h>
#include "general.h"
#include "gfxtarget.h"
#include "bspregion3d.h"


/************************************************************************/
// 3D shape class for X Windows.
// Includes code to project the 3D shape to 2D, and to draw it in X Windows.

struct xregion3d : bspRegion3d {
        xregion3d(BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY) :
          bspRegion3d(tt) {};

        xregion3d(coord *srcPts, int *srcPanelData, coord sc=1.0,
                  BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY) :
          bspRegion3d(srcPts,srcPanelData,sc,tt) {};

        xregion3d(coord *srcPts, int *srcPanelData, const pt3d& sc,
                  BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY) :
          bspRegion3d(srcPts,srcPanelData,sc,tt) {};

        xregion3d(pt3d  *srcPts, int *srcPanelData, const pt3d& sc=pt3d(1,1,1),
                  BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY) :
          bspRegion3d(srcPts,srcPanelData,sc,tt) {};

        xregion3d(BSP *tree, BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY,
                  bool wantCopy=true, bool wantEdges=true) :
          bspRegion3d(tree,tt,wantCopy,wantEdges) {};

  // default copy ctor and default assignment op are adequate

  void  Draw     (const pt3d viewPts[], const gfxTarget& gt,
		  const pt3d& viewPos, const pt2d& clip, bool solid,
		  const ulong colors[]);

 protected:
  void  Project  (const pt3d viewPts[], const pt2d& clip, pt2d prjPts[],
		int& xcode, int& ycode);
  void  DrawEdges(const pt3d viewPts[], const pt2d& clip,
		  const pt2d prjPts[], int xcode, int ycode,
		  const gfxTarget& gt);
  void  DrawPolys(const pt3d viewPts[], const pt2d& clip,
		  const pt2d prjPts[], int xcode, int ycode,
		  const ulong colors[], const gfxTarget& gt,
		  const pt3d& viewPos);
};

void InitMorphRegion3ds(xregion3d rgns[], int numRgns,
                        xregion3d::coord *firstPts,
			xregion3d::coord firstScale,
                        xregion3d::coord *lastPts,
			xregion3d::coord lastScale, int *panels,
                        bool exactFirst, bool exactLast);

#endif
