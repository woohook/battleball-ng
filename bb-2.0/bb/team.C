// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include "team.h"


int team::insigniaRandomizer= 0;


// The primary & secondary colors of the team insignia
int primaryColorNums[NUMINSIGNIA]= {
  c_white, c_darkGreen, c_yellow, c_firebrick, c_green3, c_orangeRed
};
int secondaryColorNums[NUMINSIGNIA]= {
  c_darkOliveGreen, c_forestGreen, c_blue, c_black, c_yellow, c_gray80
};


/*-------------------------------------------------------------------------*/
team::team(int num)
  : score(0),
    teamNum(num),
    hq(NULL)
{ 
  forii(fixedColors)
    colorNums[i]= i;
  int insigniaNum= (teamNum+insigniaRandomizer)%NUMINSIGNIA;
  colorNums[c_teamPrimary]= primaryColorNums[insigniaNum];
  colorNums[c_teamSecondary]= secondaryColorNums[insigniaNum];
}


/*-------------------------------------------------------------------------*/
// Initialize this team object for a new game round

void team::InitForRound(gobList& gobs, coord hqDist, int numTeams) {
  tcomp pos;
  pos.Ang()= -MA_PI/2 + (teamNum * 2*MA_PI)/numTeams;
  pos.Cart()= pt3d(hqDist,0) >> pos.Ang();
  pos.Ang() += MA_PI;
  hq= new bldgGob(pos,pt3d(0,0),teamNum);
  gobs.push_back(hq);
}
