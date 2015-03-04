// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef TEAM_h
#define TEAM_h


#include "general.h"
#include "gobs.h"


/*=========================================================================*/
struct team {
  int   score;                   // current score
  int   teamNum;                 // unique team number
  int   colorNums[totalColors];  // color indirection table
  bldgGob *hq;                   // headquarters game object for this team

  team() {};
  team(int num);
  void InitForRound(gobList& gobs,coord hqDist,int numTeams);

  static int insigniaRandomizer; // used to assign insignia randomly to
                                 // teams 
};



#endif
