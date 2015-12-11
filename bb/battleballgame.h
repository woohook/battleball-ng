// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef BATTLEBALLGAME_h
#define BATTLEBALLGAME_h

//=========================================================================
class BattleBallGame
{
 public:
  virtual gob* createExplosion(const tcomp& np, bool sprk=true, const tcomp& nv=ang3d(MA_PI/8,MA_PI/8,0)) = 0;
  virtual gob* createShell(const tcomp& np, const tcomp& nv, int tn, int *am) = 0;
  virtual gob* createBang(const tcomp& np, const tcomp& nv) = 0;
  virtual gob* createTank(const tcomp& np, const tcomp& nv, int tn) = 0;
  virtual gob* morphToHelicopter(const tankGob& tank) = 0;
  virtual gob* morphToTank(const heliGob& heli) = 0;
  virtual gob* morphToHelicopter(const tank_heliGob& tankheli) = 0;
  virtual gob* morphToTank(const tank_heliGob& tankheli) = 0;
  virtual gob* morphToTurret(const turr_bladGob& turrblad, gob *par) = 0;
  virtual gob* morphToBlade(const turr_bladGob& turrblad, gob *par) = 0;
};

extern BattleBallGame* g_BattleBallGame;

#endif
