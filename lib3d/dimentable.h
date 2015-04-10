// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef DIMENTABLE_h
#define DIMENTABLE_h


#include <list>  // STL
#include "dimension.h"

using namespace std;

/*=========================================================================*/
struct dimenTable {
  typedef dimension::coord coord;    // coordinate type from dimension class
 private:
  typedef list<dimension> dimenList;

  dimenList dimens;                  // collection of  dimensions

 public:
             dimenTable();

  dimension* Add(coord,dimenTable&);
  void       Empty();
  void       MakePartialProds(const double[4]);
  void       MakePartialProds();
  void       CountProps(int accums[]) const;
  dimenList::size_type size() const {return dimens.size()-1;};
};


/*-------------------------------------------------------------------------*/
/* The first dimension is always 0, even if there really is no dimension with
   value 0.  Always having a 0-valued dimension, whose partial product never
   need to be recomputed, often saves time (because _usually_ there is a
   dimension with value 0!)
*/
inline dimenTable::dimenTable() {
  double row[4]= {0,0,0,0};
  Add(0,*this)->MakePartialProds(row);
}


/*-------------------------------------------------------------------------*/
// The first dimension isn't (re)computed because its value is always zero.
inline void dimenTable::MakePartialProds(const double mtrxRow[4]) {
  dimenList::iterator dim=dimens.begin();

  dim++;
  for( ; dim != dimens.end(); dim++)
    (*dim).MakePartialProds(mtrxRow);
}


/*-------------------------------------------------------------------------*/
// Same as above, but adds an offset
inline void dimenTable::MakePartialProds() {
  dimenList::iterator dim=dimens.begin();

  dim++;
  for( ; dim != dimens.end(); dim++)
    (*dim).MakePartialProds();
}


#endif
