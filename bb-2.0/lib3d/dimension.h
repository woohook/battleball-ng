// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef DIMENSION_h
#define DIMENSION_h


#include "general.h"
#include "pt3d.h"



/************************************************************************/
struct dimension {
  typedef double coord;			// coordinate type
  
  enum dimProperty {unique, symmetric, half, quarter, numDimProps};

  coord	value;				// value of dimension
  dimProperty	prop;			// property (description) of the
  					// dimension
  dimension	*refDim;		// ptr to reference dimension
					// (used if prop != unique)
  pt3d	prod;				// parital product

        dimension() {};
        dimension(coord v, dimProperty dp=unique, dimension *rd=NULL)
	  : value(v), prop(dp), refDim(rd) {};
        ~dimension() {};  // so STL won't produce irritating warmings...

  bool	operator< (const dimension& d) const {return value <d.value;};
  bool	operator==(const dimension& d) const {return value==d.value;};
  void	MakePartialProds(const double mtrxRow[4]);
  void	MakePartialProds();
  void	Print() const;
};


// compute the 3 partial products for this dimension
inline void dimension::MakePartialProds(const double mtrxRow[4]) {
  prod= pt3d(mtrxRow) * value;
}

// same as above, but add an offset
inline void dimension::MakePartialProds() {
  prod= -refDim->prod;
}


#endif
