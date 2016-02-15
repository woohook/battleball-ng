// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef BATTLEBALLGAME_h
#define BATTLEBALLGAME_h

struct gobList;

//=========================================================================
class BattleBallGame
{
 public:
  virtual gob* createExplosion(const tcomp& np, bool sprk=true, const tcomp& nv=ang3d(MA_PI/8,MA_PI/8,0)) = 0;
  virtual gob* createShell(const tcomp& np, const tcomp& nv, int tn, int *am) = 0;
  virtual gob* createBang(const tcomp& np, const tcomp& nv) = 0;
  virtual gob* createTank(const tcomp& np, const tcomp& nv, int tn) = 0;
  virtual wingGob* createPlane(const tcomp& np, const tcomp& nv, int tn) = 0;
  virtual wingGob* createSaucer(const tcomp& np, const tcomp& nv, int tn) = 0;
  virtual gob* createTrain(const tcomp& np, const tcomp& nv, int tn) = 0;
  virtual gob* createRail(const tcomp& np,coord nl) = 0;
  virtual gob* createPillar(const tcomp& np) = 0;
  virtual bldgGob* createBuilding(const tcomp& np, const tcomp& nv, int tn) = 0;
  virtual ballGob* createBall(const tcomp& np, const tcomp& nv, int tn) = 0;
  virtual gob* createMountain(const tcomp& np) = 0;
  virtual gob* createTree(const tcomp& np) = 0;
  virtual bounGob* createBoundaries(const tcomp& np, bool kib) = 0;
  virtual hrznGob* createHorizon(const tcomp& np) = 0;
  virtual gob* transformToTank(const vhclGob& vhcl) = 0;
  virtual gob* morphToHelicopter(const tankGob& tank) = 0;
  virtual gob* morphToTank(const heliGob& heli) = 0;
  virtual gob* morphToHelicopter(const tank_heliGob& tankheli) = 0;
  virtual gob* morphToTank(const tank_heliGob& tankheli) = 0;
  virtual gob* morphToTurret(const turr_bladGob& turrblad, gob *par) = 0;
  virtual gob* morphToBlade(const turr_bladGob& turrblad, gob *par) = 0;
};

extern BattleBallGame* g_BattleBallGame;
extern gobList* g_scenery;

#endif
