// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include <math.h>
#include <sys/time.h>      // to get select()
//#include <sys/select.h>  // to get select() on some other systems
#include "general.h"
#include "ang2d.h"

const ang2d MA_PI= 1024;   // PI, in machine angles, min. 512


/************************************************************************/
double *sinTbl, *cosTbl;

void InitFixed() {
  // the following never gets freed, but who cares?
  sinTbl= new double[2*MA_PI +MA_PI/2];
  cosTbl= sinTbl +MA_PI/2;
  forii(2*MA_PI +MA_PI/2)
    sinTbl[i]= sin(i*M_PI/MA_PI);
}

void SleepFor(int val)
{ struct timeval tv2;
  tv2.tv_sec=0;
  tv2.tv_usec=val;
  select(0,NULL,NULL,NULL,&tv2);
}

//char *str_dup(char *s)
//{ return strcpy(malloc(strlen(s)+1),s);
//}

