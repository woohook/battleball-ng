// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.

#include "flyby.h"
#include <string.h>          // for strcmp()
#include "gobs.h"            // for wing gobs
#include "battleballgame.h"  // for gob spawning
#include "player.h"          // for player::paused

FlyBy::FlyBy()
: mode(1)
, cycles(0)
{
}

FlyBy::~FlyBy()
{
}

void FlyBy::initialize(int argc, char** argv)
{
  for (int i=1; i <argc; i++)
  {
    char *s= argv[i];
    if (!strcmp(s,"-flyby"))
    {
      mode = 2;
    }
    else if (!strcmp(s,"-noflyby"))
    {
      mode = 0;
    }
    else if (!strcmp(s,"-help"))
    {
      cout <<
        "FlyBy OPTIONS\n\n"
        "    -flyby      Many aircraft fly-bys.\n"
        "    -noflyby    Disable aircraft fly-bys.\n"
      ;
    }
  }
}

/*-------------------------------------------------------------------------*/
// Create an airplane or saucer ship game object to do a fly-by over the
// playfield.
void FlyBy::processFrame(gobList& gobs)
{
  if ( (not player::paused) and
       (cycles %512)==0 and
        (mode==2 or (mode==1 and (rand() & 0x700)==0x700)) )
  {
    bool isSaucer= rand()%2;
    tcomp pos(rand()%(2*MA_PI));
    tcomp vel= isSaucer ? pt3d(1.5,0) : pt3d(1,0);
    pos.Cart()= pt3d(-384*vel.Cart().x,0,24) >> pos.Ang();

    wingGob* g;
    if (isSaucer)
    {
      g = g_BattleBallGame->createSaucer(pos,vel,-1);  // dummy team number - does it matter? -PAH
    }
    else
    {
      g = g_BattleBallGame->createPlane(pos,vel,-1);
    }

    g->animDir= 1;
    g->ctrl= vel;
    gobs.push_back(g);
  }
  cycles++;
}
