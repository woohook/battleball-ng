// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef GENERAL_h
#define GENERAL_h


#ifndef __GNUC__
#ifndef bool
  typedef int bool;  // apparently this type is already defined for g++
#endif
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

// versions of g++ before 3.0 did not support these standard aliases.
// version 3.0 refuses to redefine them.
#ifdef __GNUC__
#if __GNUC__ < 3
#define and &&
#define or  ||
#define not !
#endif
#endif

#define forii(limit) for (int i= 0; i <limit; i++)
#define forij(limit) for (int j= 0; j <limit; j++)

#ifndef __GNUC__

#define fori(limit) for (i= 0; i <limit; i++)
#define forj(limit) for (j= 0; j <limit; j++)
#else
#define fori(limit) for (int i= 0; i <limit; i++)
#define forj(limit) for (int j= 0; j <limit; j++)
#endif

#define for_(iter,contain) for (iter=(contain).begin(); iter != (contain).end(); iter++)

//template <class T>  // xlC can't handle this for some reason
//inline T sqr(T v) {return v*v;}
#define sqr(v) ((v)*(v))

template <class T1,class T2>
inline bool isBetween(T1 lim1,T2 x,T1 lim2)
{ return ((lim1 <x)==(x <lim2));}

template <class T1,class T2>
inline bool isBetweenInclusively(T1 lim1,T2 x,T1 lim2)
{ return ((lim1 <=x and x <=lim2) or (lim2 <=x and x <=lim1));}

template <class T1,class T2>
inline bool isBetweenExclusively(T1 lim1,T2 x,T1 lim2)
{ return ((lim1 <x and x <lim2) or (lim2 <x and x <lim1));}

#define ABS(x) (((x)>0)?(x):-(x))
#define roundToLong(x) ((long)(floor(x +0.5)))

/* DANGER:  Major kludges below! -PAH
   With some compilers, such as xlC, these next two func templates are
   necessary for STL to work with classes that don't define operator== and
   operator< .
   However, these templates can potentially CAUSE BUGS, depending on how
   different C++ compilers apply them.
*/
#ifndef __GNUC__
template <class T>
inline bool operator==(const T& a, const T& b) {
  return *((int*)&a) == *((int*)&b);
}

template <class T>
inline bool operator<(const T& a, const T& b) {
  return *((int*)&a) < *((int*)&b);
}
#endif

extern const int MA_PI;
extern double *sinTbl, *cosTbl;

void InitFixed();
void SleepFor(int val);

#endif
