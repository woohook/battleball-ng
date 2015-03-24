// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef TABLE_h
#define TABLE_h


#include <stdlib.h>  // to get NULL
#include "general.h"



/************************************************************************/
template <class c>
struct table
{ int	num;			// number of objects which are in table
  double growth;		// factor by which to expand table size when
  				// an obj is added to a full table

  private:
  c	*objs;			// dynamically allocated table of objects
  int	size;			// max num of objects which can fit in table
  
  public:
  	table(int initialSize=32,double grow=1.5);
  	table(const table<c>& t) {Copy(t);};	// copy constructor
  	~table()		{if (objs!=NULL) delete [] objs;};

  table<c>& operator=(const table<c>& t) {FreeArray(); Copy(t); return *this;};
  int	Add(const c&);
  c&	operator[](int i) const	{return objs[i];};
  int	Num() const		{return num;};
  c	*Array() const		{return objs;};
  void	Empty(int newSize=32)	{FreeArray(); AllocArray(newSize);};
  int	AddIfAbsent(const c&,bool& wasAbsent);
  int	Find(const c&);

  private:
  void	Copy(const table<c>&);
  void	AllocArray(int newSize);
  void	FreeArray()		{if (objs!=NULL) delete [] objs;};
};



/*----------------------------------------------------------------------*/
// Attempts to allocate the array of objects, and sets num & size accordingly
// If the array is already allocated, doesn't free the old one
template <class c>
inline void table<c>::AllocArray (int newSize)
{ objs= new c[newSize];
  num= 0;
  size= newSize;
}


/*----------------------------------------------------------------------*/
template <class c>
table<c>::table (int initialSize, double grow)
{ growth= grow;
  AllocArray(initialSize);
}


/*----------------------------------------------------------------------*/
// IMPORTANT:  Assumes that this table's array is not already allocated
template <class c>
void table<c>::Copy (const table<c>& t) {
  growth= t.growth;
  AllocArray(t.size);
  if (objs != NULL)
  { num= t.num;
    forii(num) objs[i]= t[i];
  }
}


/*----------------------------------------------------------------------*/
// Returns the index of the object added
template <class c>
int table<c>::Add(const c& obj) {
  if (num==size) {			// if table is full
    int newSize= (int) (size*growth +0.5); // compute size of new, larger table
    c  *newObjs= new c[newSize];	// allocate new table
    
    forii(size)				// copy old table to new table
      newObjs[i]= objs[i];
    delete [] objs;			// delete old table
    objs= newObjs;
    size= newSize;
  }

  objs[num++]= obj;
  return num-1;
}


/*----------------------------------------------------------------------*/
// Returns the index of the object, if it was found in the table, or -1 if
// the object wasn't found in the table
template <class c>
int table<c>::Find(const c& obj) {
  forii(num)
    if (objs[i] == obj) return i;
  return -1;
}


/*----------------------------------------------------------------------*/
// Adds the object to the table if it is not already present.
// Returns the index of the object in the table.
template <class c>
int table<c>::AddIfAbsent(const c& obj,bool& wasAbsent)
{ int idx= Find(obj);
  wasAbsent= (idx <0);
  if (wasAbsent) return Add(obj);
  else return idx;
}


#endif
