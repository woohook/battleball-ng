// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef VIEW_h
#define VIEW_h


#include "xform.h"
#include "xregion3d.h"


/*=========================================================================*/
/* This is a 3d view of a region3d.  The view contains two types of data:
   1.  the 3d points of the region3d, which are kept in VIEW coordinates, and
       can be projected directly to the screen
   2.  angular/cartesian position tracking data, which allow the 3d points
       mentioned above to be updated, with the LEAST amount of computation
       possible

   This class could be templatized with little trouble to work with
   both 2d & 3d objects.  It would need to be parameterized by point, region,
   and transformation matrix types.
*/

struct view {
  pt3d		*pts;			// object's pts in view coords.
  int		numPts;			// number of valid points in 'pts'.
  tmtrx		modelToView;		// transforms points from the
					// object's shape definition
					// (region3d) into the view coord
  					// system.

 private:
  int		maxPts;			// size of array to which 'pts'
  					// points.
					// -- WARNING: --
					// NOT all elements of the 'pts' array
					// are necessarily valid! 'maxPts' is
					// simply the size of the array, used
					// to determine when a larger array
					// must be allocated.  The number of
					// VALID points in 'pts' is 'numPts'
  
					// The next four fields are used
					// to determine when 'pts' need to be
					// recomputed:
  xregion3d	*shape;		// ptr to the shape definition of
					// this object which was used the
					// last time 'pts' were computed 

 public:
        view()				{numPts= maxPts= 0; shape= NULL;}
  	view(const view& v)		{Copy(v);}
       ~view()				{if (maxPts >0) delete [] pts;};

  view&	operator=(const view& v)	{return Copy(v);}; // if &v != this... -PAH

  pt3d	*ViewPts()			{return pts;};
  int	NumPts() const			{return numPts;};
  void	Update(xregion3d*, const tmtrx& newMtv);

 private:
  view&	Copy(const view&);
};


#endif
