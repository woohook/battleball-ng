// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef SHAPES_h
#define SHAPES_h

#include <stdio.h>   // gets definition of NULL
#include <limits.h>  // to get INT_MAX, LONG_MAX
#include "table.h"
#include "bb.h"
#include "bbcolor.h"


#define NUMINSIGNIA 6

extern const int EXPLSPIKES, TANKHELIRGNS, BANGRGNS;

extern rgn3
  tankRgn, turrRgn, barrRgn, shellRgn,
  ballRgn, hqRgns[], heliRgn, bladeRgn,
  treeRgn, mtnRgn, mtn2Rgn, mtn3Rgn, mtn4Rgn,
  tankHeliRgns[], turrBladRgns[], planeRgn, saucerRgn,
  canopyRgn, engineRgn, tailRgn, lwingRgn, rwingRgn,
  tankBRgn, tankBFinRgn, turrBRgn, barrBRgn, bangRgns[], horizonRgn,
  boundRgn, explRgn,
  trainRgn, turrCRgn, barrCRgn,
  railRgn40, railRgn100, railRgn260,
  pillarRgn;

extern rgn3 trainRgn, railRgn;
#ifdef NONRELEASE
extern rgn3
  tunnelRgn, destroyerRgn, wheelRgn, mntn2Rgn, vtolRgn, rpelRgn, rperRgn,
  prp3Rgn;
#endif

extern rgn2
  turr2dRgn, heli2dRgn, capitalBRgn, decorRgn, plne2dRgn,
  sauc2dRgn, arrowRgn, bound2dRgn,
  insigniaRgns[],insignia2Rgns[];

void InitRegions(int numTeams, bool fancy, double hqDist, int ir);

#endif
