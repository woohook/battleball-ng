// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef SHDREGION3D_h
#define SHDREGION3D_h


#include "general.h"
#include "xregion3d.h"


/************************************************************************/
// Shadowed 3D shape class.
// Allows a 3D shape to have a shadow shape.  A pretty trivial extension of
// the xregion3d class.

struct shdRegion3d : xregion3d {
 private:
  xregion3d *shadow;   // NULL if none

 public:
        shdRegion3d(xregion3d *shdw=NULL,
		    BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY) :
          xregion3d(tt),
          shadow(shdw) {};

        shdRegion3d(coord *srcPts, int *srcPanelData, coord sc=1.0,
		    xregion3d *shdw=NULL,
		    BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY) :
          xregion3d(srcPts,srcPanelData,sc,tt),
          shadow(shdw) {};

        shdRegion3d(coord *srcPts, int *srcPanelData, const pt3d& sc,
		    xregion3d *shdw=NULL,
		    BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY) :
          xregion3d(srcPts,srcPanelData,sc,tt),
          shadow(shdw) {};

        shdRegion3d(pt3d  *srcPts, int *srcPanelData, const pt3d& sc=pt3d(1,1,1),
		    xregion3d *shdw=NULL,
		    BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY) :
          xregion3d(srcPts,srcPanelData,sc,tt),
          shadow(shdw) {};

        shdRegion3d(BSP *tree,
		    xregion3d *shdw=NULL,
		    BSP_TREE_TYPE tt=BSP_TREE_VISIBILITY,
		    bool wantCopy=true, bool wantEdges=true) :
          xregion3d(tree,tt,wantCopy,wantEdges),
          shadow(shdw) {};

  // default copy ctor and default assignment op are adequate

  xregion3d*& Shadow() {return shadow;}
};


void InitMorphRegion3ds(shdRegion3d rgns[], int numRgns,
                        shdRegion3d::coord *firstPts,
			shdRegion3d::coord firstScale,
                        shdRegion3d::coord *lastPts,
			shdRegion3d::coord lastScale, int *panels,
                        bool exactFirst, bool exactLast);

#endif
