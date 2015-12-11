// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef BATTLEBALLGAME_h
#define BATTLEBALLGAME_h

//=========================================================================
class BattleBallGame
{
 public:
  virtual gob* createExplosion(const tcomp& np, bool sprk=true, const tcomp& nv=ang3d(MA_PI/8,MA_PI/8,0)) = 0;
};

extern BattleBallGame* g_BattleBallGame;

#endif
