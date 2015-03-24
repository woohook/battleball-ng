// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include "dimentable.h"



/*=========================================================================*/
dimension *dimenTable::Add(coord value, dimenTable& symmDims) {
  dimenList::iterator dim;
  dimension	*symmDim= NULL,
  		*halfDim= NULL,
  		*qrtrDim= NULL;

  for(dim= dimens.begin(); dim != dimens.end(); dim++) {
    if ((*dim).value== value)
      return &(*dim);
    if ((*dim).value== -value)
      symmDim= &(*dim);
  }

  dimension newdim;

  if (symmDim) {
    newdim= dimension(value,dimension::symmetric,symmDim);
    return & *symmDims.dimens.insert(dimens.end(),newdim);
  }

  newdim= dimension(value,dimension::unique);
  return & *dimens.insert(dimens.end(),newdim);
}


/*-------------------------------------------------------------------------*/
void dimenTable::Empty() {
  // notice that the first, zero-valued dimension should not be erased!
  dimens.erase(++(dimens.begin()),dimens.end());
}


/*-------------------------------------------------------------------------*/
// The first "always-0" dimension isn't counted
void dimenTable::CountProps(int accums[]) const {
  dimenList::const_iterator dim= dimens.begin();

  dim++;
  for( ; dim != dimens.end(); dim++)
    accums[(*dim).prop]++;
}
