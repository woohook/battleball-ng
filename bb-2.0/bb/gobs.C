// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include <X11/keysym.h>
#include "gobs.h"


// Notes:
//   gob is an abbreviation for Game OBject
//   STL is an abbreviation for the Standard Template Library


const coord HITBOUNCE= 0.333;  // magnitude of the bounce caused by a hit

// If the auto gunner calculates that a shot will take longer than
// maxShotTime to reach an enemy vhcl, the auto gunner won't bother
// shooting.
const int maxShotTime= 55;


int explSizes[EXPLSIZES]= {4,7,8,9,9,8,7,5,4,3};


/*-------------------------------------------------------------------------*/
pt2d EraseStatus(bbGfxTarget& gt, char *title, const pt2d& pt) {
  gt << title;
  
  pt2d off= gt.PixelPos(gt.cursor+pt);
  gt.SetForeground(gt.White());
  gt.FillRectangle(off +pt2d(-26,-26),pt2d(53,53));
  gt.SetForeground(gt.Black());
  return off;
}



/*=========================================================================*/
/*                               Gob Subclasses                            */
/*=========================================================================*/


coord vhclGob::muzzleVel= 1.5;
vhclGob* vhclGob::testVhcl= NULL;
pt3d vhclGob::testVel= pt3d(0,0,0);
bool vhclGob::useBangs= true;


/*=========================================================================*/
vhclGob::vhclGob(const tcomp& p,const tcomp& v, gob *par, int tn)
  : gob(p,v,par),
    newlyCreated(true),
    ctrl(tcomp(0)),
    teamNum(tn),
    iAmHit(0),
    rangeError(0) {}

vhclGob::vhclGob(const vhclGob& g)
  : gob(g),
    newlyCreated(true),
    ctrl(g.ctrl),
    teamNum(g.teamNum),
    iAmHit(g.iAmHit),
    rangeError(g.rangeError) {}


/*-------------------------------------------------------------------------*/
gob* vhclGob::Act() {
  if (iAmHit) iAmHit--;

  if (testVhcl) {
    if (this==testVhcl)
      vel.Cart()= testVel;
    else
      vel.Cart()= pt3d(0,0,0);
  }

  return gob::Act();
}


/*-------------------------------------------------------------------------*/
// Returns true if this vhcl thinks that shooting the ball would score a
// goal, based on the ball's position and the hq's position.

bool vhclGob::CanMakeGoal(const gob& hq, const gob& ball) const {
  pt3d& ballPos= ball.WorldPos().Cart();
  pt3d& myPos= WorldPos().Cart();

  return
    (ballPos -myPos).Dist() <MaxRange() and
    hq.WillBeHitBy(ballPos,ballPos -myPos);
}


/*-------------------------------------------------------------------------*/
// Computes how close a fired shell would come to hitting a target gob,
// after factoring in imperfect accuracy.

void vhclGob::ComputeMiss(const gob* target, int firingAcc,
			  pt3d& missVector, double& closestTime) {
  tmtrx shellPos;
  pt3d  shellVel;
  GetShellPos(shellPos,shellVel);

  pt3d selfToTgt= target->Center() - Center();
  pt3d perceivedTgtPos= Center() + selfToTgt*(1+rangeError);
  
  coord closestDist;
  pt3d  closestShellPos,closestTgtPos;
  GetClosestApproach(shellPos.Cart(), shellVel,
		     perceivedTgtPos, target->vel.Cart(),
		     gravity, maxShotTime,
		     closestDist, closestTime,
		     closestShellPos, closestTgtPos);

  missVector= closestTgtPos -closestShellPos;
  missVector <<= shellPos.Ang().xy;
}


/*-------------------------------------------------------------------------*/
// Control this vhcl via keystrokes.

gob* vhclGob::Control(KeySym k, bool pressed, int *ammo) {
  if (k==XK_Home and pressed and testVhcl==NULL) testVhcl= this;
  if (k==XK_End  and pressed) testVhcl= NULL;

  if (testVhcl) {
    gobList::iterator gi;
    gob *g= testVhcl;

    switch(k) {
    case XK_Page_Up:
      gi= gobs->end();
      do {
	gi--;
	if ((**gi).IsVhcl() and *gi != testVhcl) g= *gi;
      }
      while (gi != gobs->begin() and *gi != testVhcl);
      testVhcl= (vhclGob*) g;
      break;
      
    case XK_Page_Down:
      gi= gobs->begin();
      while (gi != gobs->end() and *gi != testVhcl) {
	if ((**gi).IsVhcl()) g= *gi;
	gi++;
      }
      testVhcl= (vhclGob*) g;
      break;

    case XK_KP_Up:        testVel.x=  0.5*pressed; break;
    case XK_KP_Down:      testVel.x= -0.5*pressed; break;
    case XK_KP_Left:      testVel.y=  0.5*pressed; break;
    case XK_KP_Right:     testVel.y= -0.5*pressed; break;
    case XK_KP_Page_Up:   testVel.z=  0.5*pressed; break;
    case XK_KP_Page_Down: testVel.z= -0.5*pressed; break;
    }
  }
  
  return this;
}


/*-------------------------------------------------------------------------*/
// Control this vhcl via auto-pilot, by providing a destination.
// dest is in world coords
// destDir may be (0,0,0) if N/A

void vhclGob::Control(const pt3d& dest, const pt3d& destDir,
		      tcomp& selfToDest) {
  pt3d modelDest= dest << WorldPos();  // dest in model coords
  selfToDest= tcomp(modelDest,modelDest.Ang3d().Normalized());

  ctrl= selfToDest;
  tcomp maxVel= MaxVel();
  tcomp negMaxVel= -maxVel;
  ctrl.SetMax(negMaxVel);
  ctrl.SetMin(   maxVel);
}


/*-------------------------------------------------------------------------*/
// Draw status indicators for this vhcl in the text subwindow.

void vhclGob::DrawStatus(bbGfxTarget& gt, int color1, int color2,
			 bool drawAll) {
  if (drawAll) {
    gt << " Team";
    
    gt.SetForeground(gt.Colors()[color2]);
    ::Draw(&insignia2Rgns[teamNum],gt,gt.PixelPos(pt2d(4.5,3)),0,pt2d(4,-4),
	   true);
    gt.SetForeground(gt.Colors()[color1]);
    ::Draw(&insigniaRgns[teamNum], gt,gt.PixelPos(pt2d(4.5,3)),0,pt2d(4,-4),
	   true);
    gt.SetForeground(gt.Black());
    
    gt.cursor.y= 5;
    if (ffire==transparent)
      gt << "friendly fire: transparent";
    else if (ffire==blocked)
      gt << "friendly fire: blocked    ";
    else if (ffire==dangerous)
      gt << "friendly fire: dangerous  ";
  }
  gt.cursor.x += 9;
  gt.cursor.y= 0;
}


/*-------------------------------------------------------------------------*/
twoGobs vhclGob::HitByVhcl(vhclGob *v) {
  if (v != this)  // don't hit yourself!
    if (KickIfHit(v,0.3,1,0.3,true))
      gobs->push_back(new explGob((Center()+v->Center())/2,false,0));
  return twoGobs(this,v);
}


/*-------------------------------------------------------------------------*/
gob* vhclGob::Interact() {
  TestForGroundBounce();
  UpdateWorldPos();
  gobList::iterator gi;
  for_(gi,*gobs)
    (**gi).HitByVhcl(this);  // handles hitting other vhcls, bldgs etc.
  return this;
}


/*-------------------------------------------------------------------------*/
// Fire a shell, if the gob believes it can hit the target.

void vhclGob::FireIfReady(int* ammo,
			  const gob* target, pt3d& missVector,
			  int firingAcc, int firingFrq) {
  double closestTime;
  ComputeMiss(target,firingAcc,missVector,closestTime);
  if (missVector.Dist() < 0.7*target->Shape()->FarthestDist() and
      (rand()%1024) < (1-closestTime/maxShotTime) *220*firingFrq/9) {
    Fire(ammo);

    // re-calculate the range error
    double maxRangeError= 0.45*(1-firingAcc/9.0);
    rangeError= maxRangeError * (2*(rand()/(double)RAND_MAX) - 1);
  }
}


/*-------------------------------------------------------------------------*/
// Return a ptr to the gob at which this vhcl would like to shoot.

gob* vhclGob::PreferredVhclTarget(const pt3d& ballPos) const
{
  // direction that player's vhcl is facing, in world coords
  pt3d dir= pt3d(1,0,0) * WorldPos() -WorldPos().Cart();

  gob  *closestEnemyToBall= NULL;
  coord closestEnemyToBallDist= MaxRange()/2;
  gob  *closestEnemyToSelf= NULL;
  coord closestEnemyToSelfDist= MaxRange();
  
  gobList::iterator gi;
  for_(gi,*gobs) {
    gob& g= **gi;
    if (g.IsVhcl() and g.TeamNum() != teamNum) {
      coord enemyToBallDist=
	(g.WorldPos().Cart() -ballPos).Dist();
      pt3d enemyToSelf= WorldPos().Cart() -g.WorldPos().Cart();
      coord enemyToSelfDist= enemyToSelf.Dist();
      
      if (enemyToBallDist <closestEnemyToBallDist and
	  dir.Dot(enemyToSelf) <6) {
	closestEnemyToBall= &g;
	closestEnemyToBallDist= enemyToBallDist;
      }
      
      if (enemyToSelfDist <closestEnemyToSelfDist and
	  dir.Dot(enemyToSelf) <6) {
	closestEnemyToSelf= &g;
	closestEnemyToSelfDist= enemyToSelfDist;
      }
    }
  }
  
  return closestEnemyToBall ? closestEnemyToBall : closestEnemyToSelf;
}


/*-------------------------------------------------------------------------*/
// Set this vhcl's velocity.

void vhclGob::SetVelocity() {
  tcomp desiredVel= ctrl;

  desiredVel.Cart() >>= pos.Ang();  // put desiredVel.Cart() in world coords
  tcomp delta= desiredVel -vel;


  // Do translational velocity ---
  coord cartAcc= MaxAcc().Cart().x;
  coord dist= delta.Cart().Dist();  // "distance" from present velocity to
                                    // desired velocity
  if (dist <=cartAcc)
    vel.Cart()= desiredVel.Cart();
  else
    vel.Cart() += delta.Cart() * (cartAcc/dist);


  // Do angular velocity ---
  ang2d angAcc=  MaxAcc().Ang().xy;

  if      (delta.Ang().xy >angAcc)  vel.Ang().xy += angAcc;
  else if (delta.Ang().xy <-angAcc) vel.Ang().xy -= angAcc;
  else                     vel.Ang().xy = desiredVel.Ang().xy;

  if      (delta.Ang().xz >angAcc)  vel.Ang().xz += angAcc;
  else if (delta.Ang().xz <-angAcc) vel.Ang().xz -= angAcc;
  else                     vel.Ang().xz = desiredVel.Ang().xz;

  if      (delta.Ang().yz >angAcc)  vel.Ang().yz += angAcc;
  else if (delta.Ang().yz <-angAcc) vel.Ang().yz -= angAcc;
  else                     vel.Ang().yz = desiredVel.Ang().yz;
}


/*=========================================================================*/
tankGob::tankGob(const tcomp& newPos, const tcomp& newVel, int tn)
  : vhclGob(newPos,newVel,NULL,tn),
    turr(pt3d(fancy ? 1 : 0,0,3.75),0,this,tn),
    fin(&tankBFinRgn,this) {}

tankGob::tankGob(const vhclGob& g)
  : vhclGob(g),
    turr(pt3d(fancy ? 1 : 0,0,3.75),0,this,g.teamNum),
    fin(&tankBFinRgn,this) {}

tankGob::tankGob(const tank_heliGob& g)
  : vhclGob(g),
    turr(g.turrBlad,this),
    fin(&tankBFinRgn,this) {}

gob* tankGob::Act() {
  // Set velocity
  if (Altitude() >0)
    vel.Cart().z += gravity;
  else if (vel.Cart().z==0) {
    SetVelocity();
    // level tank when on ground
    ang3d a= pos.Ang().Normalized();
    vel.Ang().xz= -a.xz;
    vel.Ang().yz= -a.yz;
  }
  gob *result= vhclGob::Act();
  if (result==this) turr.Act();
  return result;
}

bool tankGob::Contains(const pt3d& globalPt, bool inclusive) {
  // adjust (fudge) the bounding 3D rectangles around the tanks
  // this provides more playable collision detection
  rect3d box= Shape()->Box();
  box.high.z += 1.5;
  if (inclusive)
    return box.ContainsInclusively(globalPt << WorldPos());
  else
    return box.ContainsExclusively(globalPt << WorldPos());
}

gob* tankGob::Control(KeySym k, bool pressed, int *ammo) {
  vhclGob::Control(k,pressed,ammo);
  turr.Control(k,pressed,ammo);
  switch(k) {
    case XK_Left:
    case XK_j:     ctrl.Ang().xy=  MaxVel().Ang().xy*pressed; break;
    case XK_Right:
    case XK_l:     ctrl.Ang().xy= -MaxVel().Ang().xy*pressed; break;
    case XK_Up:
    case XK_i:     ctrl.Cart().x=  MaxVel().Cart().x*pressed; break;
    case XK_Down:
    case XK_k:     ctrl.Cart().x= -MaxVel().Cart().x*pressed; break;
    case XK_t:     if (pressed) return new tank_heliGob(*this);
                   break;
#ifdef NONRELEASE
    case XK_z:     if (pressed) return new saucGob(pos,vel,teamNum);
                   break;
#endif
  }
  return this;
}

void tankGob::Control(int *ammo,
		      const gob *target, int firingAcc, int firingFrq) {
  turr.Control(ammo,target,firingAcc,firingFrq);
}

void tankGob::DrawControls(bbGfxTarget& gt) {
  turr.DrawControls(gt);
  gt.cursor= pt2d(gt.rightmost+1,2);
  gt << "  I  ";
  gt << "J K L   Spc   T";
  gt << " tank  fire  trans";
  gt << "             form";
  gt.Box(2,2);
  gt.Box(0,3).Box(2,3).Box(4,3).Box(8,3,3).Box(14,3);
}

void tankGob::DrawStatus(bbGfxTarget& gt, int color1, int color2,
			 bool drawAll) {
  vhclGob::DrawStatus(gt,color1,color2,drawAll);
  turr.DrawStatus(gt,color1,color2,drawAll);
}

tcomp tankGob::ModelViewPos() const {
  tcomp vp(pt3d(-3,0,0.15) >> turr.pos, turr.pos.Ang());
  vp.Ang().xz += turr.barr.pos.Ang().xz /3;
  return vp;
}

void tankGob::GetViewOrder(const pt3d& eyePos, gobList& shapes) {
  pt3d eyePosInModel= eyePos << WorldPos();  // eye pos in model coords
  // if the eye pos is above the plane along the top of the tank,
  // then the turret it closer, so add it to shapes first;
  // else, add the tank chassis to shapes first
  bool eyeIsAboveTank= (eyePosInModel.z >3);
  bool eyeIsBeforeFin= (eyePosInModel.x >-6.5);

  if (eyeIsAboveTank) {
    if (eyeIsBeforeFin) {
      turr.GetViewOrder(eyePos,shapes);
      if (fancy) shapes.push_back(&fin);
    } else {
      if (fancy) shapes.push_back(&fin);
      turr.GetViewOrder(eyePos,shapes);
    }
    shapes.push_back(this);
  } else {
    shapes.push_back(this);
    if (eyeIsBeforeFin) {
      turr.GetViewOrder(eyePos,shapes);
      if (fancy) shapes.push_back(&fin);
    } else {
      if (fancy) shapes.push_back(&fin);
      turr.GetViewOrder(eyePos,shapes);
    }
  }
}

twoGobs tankGob::HitByShll(shllGob *hitter) {
  if (Contains(hitter->pos.Cart())) {
    if (teamNum==hitter->teamNum) {
      if (ffire==transparent)
        return twoGobs(this,hitter);
      if (ffire==blocked)
        return twoGobs(this,(gob*)NULL);
    }
    
    iAmHit= turr.iAmHit= turr.barr.iAmHit= 4;
    PushFrom(hitter,hitter->power/3,1,HITBOUNCE);
    return twoGobs(this,new explGob(hitter->pos));
  }
  return twoGobs(this,hitter);
}

twoGobs tankGob::HitByBall(ballGob *hitter) {
  if (KickIfHit(hitter,5,0.1333)) {
    gobs->push_back(new explGob(hitter->Center(),false));
    hitter->teamNum= teamNum;
  }
  return twoGobs(this,hitter);
}

void tankGob::Control(const pt3d& dest, const pt3d& destDir,
		      tcomp& selfToDest) {
  vhclGob::Control(dest,destDir,selfToDest);
  if (isBetween(0.2*MA_PI,ABS(selfToDest.Ang().xy),0.8*MA_PI))
    ctrl.Cart().x= 0;
}


/*=========================================================================*/
turrGob::turrGob(const tcomp& newPos, const tcomp& newVel, gob *newParent,
		 int tn, rgn3* newShape)
  : vhclGob(newPos,newVel,newParent,tn),
    barr(pt3d(newShape ? 1.8 : (fancy ? 2.25 : 2),0,0), 0, this, tn),
    prevAng(0),
    shape(newShape) {};

turrGob::turrGob(const turr_bladGob& g, gob *newParent) 
  :  vhclGob(tcomp(pt3d(fancy ? 1 : 0,0,3.75),g.pos.Ang()),
	     0, newParent, g.teamNum),
     barr(pt3d(fancy ? 2.25 : 2,0,0), 0, this, g.teamNum),
     prevAng(0),
     shape(NULL)
{
  pos.Ang()= pos.Ang().Normalized();
};

gob* turrGob::Control(KeySym k, bool pressed, int *ammo) {
  vhclGob::Control(k,pressed,ammo);
  barr.Control(k,pressed,ammo);
  switch(k) {
    case XK_s:     vel.Ang().xy=  MaxVel().Ang().xy*pressed; break;
    case XK_f:     vel.Ang().xy= -MaxVel().Ang().xy*pressed; break;
  }
  return this;
}

void turrGob::Control(int *ammo,
		      const gob *target, int firingAcc, int firingFrq) {
  if (target==NULL) {
    // no target, so stop moving turret & barrel
    vel.Ang().xy= 0;
    barr.vel.Ang().xz= 0;
    return;
  }

  pt3d missVector;
  FireIfReady(ammo,target,missVector,firingAcc,firingFrq);

  // Swivel turret

  if (missVector.y < -1)
    vel.Ang().xy= -MaxVel().Ang().xy;
  else if (missVector.y > +1)
    vel.Ang().xy= +MaxVel().Ang().xy;
  else
    vel.Ang().xy= 0;

  // Adjust gun elevation

  // Assumes that raising gun angle causes the shell to have a higher
  // altitude at the closest point of approach
  if (missVector.z >+1)
    barr.vel.Ang().xz= +barr.MaxVel().Ang().xz;
  else if (missVector.z <-1)
    barr.vel.Ang().xz= -barr.MaxVel().Ang().xz;
  else
    barr.vel.Ang().xz= 0;
}

void turrGob::DrawStatus(bbGfxTarget& gt, int color1, int color2,
			 bool drawAll) {
  ang2d ang= pos.Ang().xy;
  if (ang != prevAng or drawAll) {
    pt2d off= EraseStatus(gt,"Turret",pt2d(4,2));
    ::Draw(&turr2dRgn,gt,off,-ang);
    prevAng= ang;
  }
  gt.cursor.x += 9;
  gt.cursor.y= 0;
  barr.DrawStatus(gt,color1,color2,drawAll);
}

void turrGob::DrawControls(bbGfxTarget& gt) {
  gt.cursor= pt2d(gt.rightmost+1,2);
  gt << "   E    ";
  gt << " S D F  ";
  gt << " barrel ";
  gt << "& turret";
  gt.Box(3,2);
  gt.Box(1,3).Box(3,3).Box(5,3);
}

void turrGob::EyeIsInside(bool inside) {
  vhclGob::EyeIsInside(inside);
  barr.EyeIsInside(inside);
}

gob *turrGob::Act() {
  vhclGob::Act();
  barr.Act();
  return this;
}

gob *turrGob::Interact() {
  if (pos.Ang().xy <-MA_PI/2) pos.Ang().xy= -MA_PI/2;
  if (pos.Ang().xy >MA_PI/2)  pos.Ang().xy= MA_PI/2;
  UpdateWorldPos();
  return this;
}

void turrGob::GetViewOrder(const pt3d& eyePos, gobList& shapes) {
  // if the eye pos is in front of the plane along the front of the turret,
  // then the barrel is closer, so add the barrel to shapes first;
  // else, add the turret to shapes first
  //if (fancy) the plane along the turret front is different... // -PAH
  pt3d& normal= Shape()->Panels()[0].normal;
  if ((eyePos << WorldPos()).Dot(normal) > (fancy ? 2.121 : 2.0)) {
    barr.GetViewOrder(eyePos,shapes);
    shapes.push_back(this);
  } else {
    shapes.push_back(this);
    barr.GetViewOrder(eyePos,shapes);
  }
}

rgn3* turrGob::Shape() const {
  return shape ? shape : (fancy ? &turrBRgn : &turrRgn);
}


/*=========================================================================*/
barrGob::barrGob(const tcomp& newPos, const tcomp& newVel, gob *newParent,
		 int tn, rgn3* newShape)
  : vhclGob(newPos,newVel,newParent,tn),
    prevAng(0),
    shape(newShape) {}

gob* barrGob::Control(KeySym k, bool pressed, int *ammo) {
  vhclGob::Control(k,pressed,ammo);
  switch(k) {
    case XK_e:     vel.Ang().xz=  MaxVel().Ang().xz*pressed; break;
    case XK_d:     vel.Ang().xz= -MaxVel().Ang().xz*pressed; break;
    case XK_space: if (pressed) Fire(ammo); break;
  }
  return this;
}

void barrGob::DrawStatus(bbGfxTarget& gt, int color1, int color2,
			 bool drawAll) {
  ang2d ang= pos.Ang().xz;
  if (ang != prevAng or drawAll) {
    pt2d off= EraseStatus(gt,"Barrel",pt2d(2,2));
    ::Draw(&arrowRgn,gt,off,-ang);
    prevAng= ang;
  }
}

void barrGob::Fire(int *ammo) {
  if (*ammo >0) {
    tmtrx shellPos;
    pt3d  shellVel;
    GetShellPos(shellPos,shellVel);
    gobs->push_back(new shllGob(shellPos,shellVel,teamNum,ammo));
    if (useBangs)
      gobs->push_back(new bangGob(shellPos,0));
  }
}

void barrGob::GetShellPos(tmtrx& shellPos, pt3d& shellVel) {
  shellPos= tcomp(pt3d(3.75,0,-0.25)) * WorldPos();
  shellVel= pt3d(muzzleVel,0,0) * WorldPos() -WorldPos().Cart();
}

gob *barrGob::Interact() {
  if (pos.Ang().xz <0)            pos.Ang().xz= 0;
  if (pos.Ang().xz >25*MA_PI/180) pos.Ang().xz= 25*MA_PI/180;
  UpdateWorldPos();
  return this;
}

coord barrGob::MaxRange() const {
  return Range(25*MA_PI/180,WorldPos().Cart().z+1,muzzleVel,gravity);
}

rgn3* barrGob::Shape() const {
  return shape ? shape : (fancy ? &barrBRgn : &barrRgn);
}


/*=========================================================================*/
heliGob::heliGob(const tcomp& newPos, const tcomp& newVel, int tn)
  : vhclGob(newPos,newVel,NULL,tn),
    blad(pt3d(0,0,3.75),tcomp(0),this,tn) {}

heliGob::heliGob(const tank_heliGob& g)
  : vhclGob(g),
    blad(g.turrBlad,this) {}

void heliGob::Control(const pt3d& dest, const pt3d& destDir,
		      tcomp& selfToDest) {
  vhclGob::Control(dest,destDir,selfToDest);

  ang2d absAngxy= ABS(selfToDest.Ang().xy);
  if (isBetween(0.2*MA_PI,absAngxy,0.8*MA_PI) or
      (isBetween(0.1*MA_PI,absAngxy,0.9*MA_PI) and
       selfToDest.Cart().Dist() <8))
    ctrl.Cart().x *= 0.6;

  // vel.Cart().z is in model coords, not world coords, so this next
  // test is technically not correct...
  if (ctrl.Cart().z <0 and 
      (Altitude() +3*vel.Cart().z <0 or Altitude() <1))
    ctrl.Cart().z= 0;

  ang2d angxz= pos.Ang().xz;
  if (angxz > MA_PI/8 and ctrl.Ang().xz >0)
    ctrl.Ang().xz= 0;
  if (angxz <-MA_PI/8 and ctrl.Ang().xz <0)
    ctrl.Ang().xz= 0;
}

gob* heliGob::Control(KeySym k, bool pressed, int *ammo) {
  vhclGob::Control(k,pressed,ammo);
  blad.Control(k,pressed,ammo);
  switch(k) {
    case XK_e:     ctrl.Cart().z=  MaxVel().Cart().z*pressed; break;
    case XK_d:     ctrl.Cart().z= -MaxVel().Cart().z*pressed; break;
    case XK_Left:
    case XK_j:     ctrl.Ang().xy=  MaxVel().Ang().xy*pressed; break;
    case XK_Right:
    case XK_l:     ctrl.Ang().xy= -MaxVel().Ang().xy*pressed; break;
    case XK_Up:
    case XK_i:     ctrl.Cart().x=  MaxVel().Cart().x*pressed; break;
    case XK_Down:
    case XK_k:     ctrl.Cart().x= -MaxVel().Cart().x*pressed; break;
    case XK_s:     ctrl.Cart().y=  MaxVel().Cart().y*pressed; break;
    case XK_f:     ctrl.Cart().y= -MaxVel().Cart().y*pressed; break;
    case XK_y:     ctrl.Ang().xz= -MaxVel().Ang().xz*pressed; break;
    case XK_h:     ctrl.Ang().xz=  MaxVel().Ang().xz*pressed; break;

    case XK_space: if (pressed) Fire(ammo); break;
    case XK_t:     if (pressed) return new tank_heliGob(*this); break;
  }
  return this;
}

void heliGob::Control(int *ammo,
		      const gob *target, int firingAcc, int firingFrq) {
  if (target==NULL) return;

  // heli should fire more frequently since heli has a harder time
  // lining up a shot, so we mult. firingFrq by 1.5
  pt3d missVector;
  FireIfReady(ammo,target,missVector,firingAcc,2*firingFrq);

  // Adjust pitch to aim at target

  // Assumes that raising gun angle causes the shell to have a higher
  // altitude at the closest point of approach
  ang2d angxz= pos.Ang().xz;

  if (missVector.z >+0.8)
    ctrl.Ang().xz= (angxz <MA_PI/8) ? MaxVel().Ang().xz : 0;
  else if (missVector.z <-0.8)
    ctrl.Ang().xz= (angxz >-MA_PI/8) ? -MaxVel().Ang().xz : 0;
  else
    ctrl.Ang().xz= (ang2d) 0.9*ctrl.Ang().xz;  // taper off
}

void heliGob::DrawControls(bbGfxTarget& gt) {
  gt.cursor= pt2d(gt.rightmost+1,2);
  gt << "    E        Y      I";
  gt << "  S D F      H    J K L  Spc     T";
  gt << "altitude/  pitch  fwd/   fire  trans";
  gt << "sideways          turn         form";
  gt.Box(4,2).Box(2,3).Box(4,3).Box(6,3);
  gt.Box(13,2).Box(13,3);
  gt.Box(20,2).Box(18,3).Box(20,3).Box(22,3);
  gt.Box(25,3,3);
  gt.Box(33,3);
}

void heliGob::DrawStatus(bbGfxTarget& gt, int color1, int color2,
			 bool drawAll) {
  vhclGob::DrawStatus(gt,color1,color2,drawAll);
  ang2d ang= pos.Ang().xz;
  coord z= min(Altitude(),13.);
  if (ang != prevAng or z != prevAltitude or drawAll) {
    pt2d off= EraseStatus(gt,"Elevation & Pitch",pt2d(9,2));
    ::Draw(&heli2dRgn,gt,off,-ang);
    if (z <13)
      XDrawLine(gt.disp,gt.win,gt.gc,
		(int) off.x-26, (int) (off.y+9+z),
		(int) off.x+26, (int) (off.y+9+z));
    prevAng= ang;
    prevAltitude= z;
  }
}

void heliGob::Fire(int *ammo) {
  if (*ammo >0) {
    tmtrx shellPos;
    pt3d  shellVel;
    GetShellPos(shellPos,shellVel);
    gobs->push_back(new shllGob(shellPos,shellVel,teamNum,ammo));
    if (useBangs)
      gobs->push_back(new bangGob(shellPos,0));
  }
}

void heliGob::GetShellPos(tmtrx& shellPos, pt3d& shellVel) {
  shellPos=
    tcomp(pt3d(3.5,0,0.55),ang3d(0,(ang2d)(0.05*MA_PI),0)) * WorldPos();
  shellVel= pt3d(muzzleVel,0,0) * WorldPos() -WorldPos().Cart();
}

void heliGob::GetViewOrder(const pt3d& eyePos, gobList& shapes) {
  // if the eye pos is above the plane along the top of the heli,
  // then the blade it closer, so add it to shapes first;
  // else, add the heli to shapes first
  if ((eyePos << WorldPos()).Dot(pt3d(0,0,1)) > Shape()->Box().high.z) {
    blad.GetViewOrder(eyePos,shapes);
    shapes.push_back(this);
  } else {
    shapes.push_back(this);
    blad.GetViewOrder(eyePos,shapes);
  }
}

gob* heliGob::Interact() {
  if (pos.Ang().xz >MA_PI/4)  pos.Ang().xz= MA_PI/4;
  if (pos.Ang().xz <-MA_PI/4) pos.Ang().xz= -MA_PI/4;
  if (pos.Ang().yz >MA_PI/4)  pos.Ang().yz= MA_PI/4;
  if (pos.Ang().yz <-MA_PI/4) pos.Ang().yz= -MA_PI/4;
  return vhclGob::Interact();
}

gob *heliGob::Act() {
  // Heli can't slide along ground
  if (Altitude() <0.001) {
    ctrl.Cart().x= 0;
    ctrl.Cart().y= 0;
  }
  SetVelocity();
  // level heli when on ground
  if (Altitude() <0.001)
    pos.Ang().xz= pos.Ang().yz= 0;
  gob *result= vhclGob::Act();
  if (result==this) blad.Act();
  return result;
}

twoGobs heliGob::HitByShll(shllGob *hitter) {
  if (Contains(hitter->pos.Cart())) {
    if (teamNum==hitter->teamNum) {
      if (ffire==transparent)
        return twoGobs(this,hitter);
      if (ffire==blocked)
        return twoGobs(this,(gob*)NULL);
    }

    iAmHit= 4;
    PushFrom(hitter,hitter->power/2,3);
    return twoGobs(this,new explGob(hitter->pos));
  }
  return twoGobs(this,hitter);
}

twoGobs heliGob::HitByBall(ballGob *hitter) {
  pt3d ballPos= hitter->WorldPos().Cart() +hitter->Shape()->Box().Center();
  if (Contains(ballPos)) {
    pt3d kick= hitter->pos.Cart() - pt3d(-1.5,0,0)*WorldPos();
    hitter->vel.Cart() += kick.Normalized(0.5);
    gobs->push_back(new explGob(ballPos,false));
    
    if (hitter->Altitude() <0.001)
      hitter->vel.Cart().z= 0.1333;
    hitter->teamNum= teamNum;
  }
  return twoGobs(this,hitter);
}

coord heliGob::MaxRange() const {
  return Range((ang2d)(0.129*MA_PI),WorldPos().Cart().z +0.5,muzzleVel,
	       gravity);
}


/*=========================================================================*/
bladGob::bladGob(const tcomp& newPos, const tcomp& newVel, gob *newParent,
		 int tn)
  : vhclGob(newPos,newVel,newParent,tn) {}

bladGob::bladGob(const turr_bladGob& g, gob *newParent)
  : vhclGob(tcomp(pt3d(0,0,3.75),g.pos.Ang()+MA_PI/2),
	    0, newParent, g.teamNum) {}

gob* bladGob::Act() {
  if (parent) {
    vhclGob *p= (vhclGob*) parent;  // Caution: assumes parent is a vhclGob
    if (p->Altitude()==0)
      vel.Ang().xy= 0;
    else {
      vel.Ang().xy= -MA_PI/5;
      if (p->ctrl.Cart().z >0) vel.Ang().xy= -MA_PI/4;
    }
  }
  return vhclGob::Act();
}


/*=========================================================================*/
tank_heliGob::tank_heliGob(const tankGob& g)
  : vhclGob(g.pos,g.vel,g.parent,g.teamNum),
    turrBlad(g.turr,this),
    animNum(-1),
    animDir(1) {}

tank_heliGob::tank_heliGob(const heliGob& g)
  : vhclGob(g.pos,g.vel,g.parent,g.teamNum),
    turrBlad(g.blad,this),
    animNum(TANKHELIRGNS),
    animDir(-1) {}

gob* tank_heliGob::Act() {
  animNum += animDir;
  if (animNum==TANKHELIRGNS) return new heliGob(*this);
  if (animNum==-1)           return new tankGob(*this);

  if (Altitude() >0)
    vel.Cart().z += gravity;
  gob *result= vhclGob::Act();
  if (result==this) turrBlad.Act();
  return result;
}

void tank_heliGob::EyeIsInside(bool inside) {
  vhclGob::EyeIsInside(inside);
  turrBlad.EyeIsInside(inside);
}

void tank_heliGob::GetViewOrder(const pt3d& eyePos, gobList& shapes) {
  // if the eye pos is above the plane along the top of the tank,
  // then the turret it closer, so add it to shapes first;
  // else, add the tank chassis to shapes first
  if ((eyePos << WorldPos()).Dot(pt3d(0,0,1)) > Shape()->Box().high.z) {
    turrBlad.GetViewOrder(eyePos,shapes);
    shapes.push_back(this);
  } else {
    shapes.push_back(this);
    turrBlad.GetViewOrder(eyePos,shapes);
  }
}

twoGobs tank_heliGob::HitByShll(shllGob *hitter) {
  if (Contains(hitter->pos.Cart())) {
    if (teamNum==hitter->teamNum) {
      if (ffire==transparent)
        return twoGobs(this,hitter);
      if (ffire==blocked)
        return twoGobs(this,(gob*)NULL);
    }

    iAmHit= 4;
    PushFrom(hitter,hitter->power/2,3);
    return twoGobs(this,new explGob(hitter->pos));
  }
  return twoGobs(this,hitter);
}

twoGobs tank_heliGob::HitByBall(ballGob *hitter) {
  if (KickIfHit(hitter,5,0.1333)) {
    gobs->push_back(new explGob(hitter->Center(),false));
    hitter->teamNum= teamNum;
  }
  return twoGobs(this,hitter);
}

tcomp tank_heliGob::ModelViewPos() const {
  pt3d pt;
  pt.Interpolate(pt3d(-3,0,0.25)+pt3d(0,0,3.75), //tank pos
		 pt3d(-2,0,2.5),                 //heli pos
		 animNum,
		 TANKHELIRGNS);
  return pt;
}

rgn3* tank_heliGob::Shape() const {
  // Have to check if animNum is out of range [0,TANKHELINUMS-1] !
  // AnimNum can be out of this range only if the player transforms then
  // hits the "pause" key in the same iteration
  if (animNum <0) return &tankHeliRgns[0];
  if (animNum >=TANKHELIRGNS) return &tankHeliRgns[TANKHELIRGNS-1];
  return &tankHeliRgns[animNum];
}


/*=========================================================================*/
turr_bladGob::turr_bladGob(const turrGob& g, gob *newParent)
  : vhclGob(g.pos.Ang(),g.vel,newParent,g.teamNum),
    animNum(-1),
    animDir(1),
    angxyAcc(0) {}

turr_bladGob::turr_bladGob(const bladGob& g, gob *newParent)
  : vhclGob(g.pos.Ang()-MA_PI/2,g.vel,newParent,g.teamNum),
    animNum(TANKHELIRGNS),
    animDir(-1),
    angxyAcc(0)
{
  pos.Ang().xy &= (2*MA_PI-1);  // force angxy to be positive
  if (vel.Ang().xy==0) return;

  // decelerate so that blade makes <2 full revolutions
  angxyAcc= -sqr(vel.Ang().xy) / (2 * (-2*MA_PI -pos.Ang().xy));

  // if <2 revolutions takes too long, decelerate harder to make <1 revolution
  int t= -vel.Ang().xy / angxyAcc;
  if (t >=TANKHELIRGNS)
    angxyAcc= -sqr(vel.Ang().xy) / (2 * (0 -pos.Ang().xy));
}

gob* turr_bladGob::Act() {
  animNum += animDir;
  if (animNum==TANKHELIRGNS)
    return new bladGob(*this,NULL);
  if (animNum==-1) 
    return new turrGob(*this,NULL);

  vel.Ang().xy += angxyAcc;
  if (vel.Ang().xy >0) {
    pos.Ang().xy= 0;
    vel.Ang().xy= 0;
    angxyAcc= 0;
  }

  return vhclGob::Act();
}

rgn3* turr_bladGob::Shape() const {
  // Have to check if animNum is out of range [0,TANKHELINUMS-1] !
  // AnimNum can be out of this range only if the player transforms then
  // hits the "pause" key in the same iteration
  if (animNum <0) return &turrBladRgns[0];
  if (animNum >=TANKHELIRGNS) return &turrBladRgns[TANKHELIRGNS-1];
  return &turrBladRgns[animNum];
}


/*=========================================================================*/
gob* wingGob::Act() {
  if (testVhcl==NULL)
    animNum += animDir;
  if (animNum==320 and (rand()%4)==0)
    ctrl.Ang().yz= MA_PI/64;
  else if (animNum==448)
    ctrl.Ang().yz= 0;
  else if (animNum==768)
    return NULL;
  return vhclGob::Act();
}

void wingGob::Control(const pt3d& dest, const pt3d& destDir,
		      tcomp& selfToDest) {
  vhclGob::Control(dest,destDir,selfToDest);

  ang3d a= pos.Ang().Normalized();
  if (selfToDest.Ang().xy >MA_PI/4)
    ctrl.Ang().yz= -MA_PI/2 -a.yz;  // full left turn
  else if (selfToDest.Ang().xy <-MA_PI/4)
    ctrl.Ang().yz=  MA_PI/2 -a.yz;  // full right turn
  else
    ctrl.Ang().yz= -2*selfToDest.Ang().xy -a.yz;

  if (ABS(a.yz) >3*MA_PI/8)
    if (selfToDest.Ang().xz <0)
      ctrl.Ang().xz= 0;
  if (selfToDest.Cart().x <0.5)
    ctrl.Cart().x= 0.5;  // minimum flying speed
}

twoGobs wingGob::HitByShll(shllGob *hitter) {
  if (Contains(hitter->pos.Cart())) {
    if (teamNum==hitter->teamNum) {
      if (ffire==transparent)
        return twoGobs(this,hitter);
      if (ffire==blocked)
        return twoGobs(this,(gob*)NULL);
    }

    iAmHit= 4;
    PushFrom(hitter,hitter->power/2,3);
    return twoGobs(this,new explGob(hitter->pos));
  }
  return twoGobs(this,hitter);
}


/*=========================================================================*/
plneGob::plneGob(const tcomp& newPos, const tcomp& newVel, int tn)
  : wingGob(newPos,newVel,tn) {}

gob* plneGob::Act() {
  SetVelocity();
  return wingGob::Act();
}


/*=========================================================================*/
saucGob::saucGob(const tcomp& newPos, const tcomp& newVel, int tn)
  : wingGob(newPos,newVel,tn) {}

gob* saucGob::Act() {
  SetVelocity();
  return wingGob::Act();
}


#ifdef NONRELEASE

/*=========================================================================*/
saucGob::saucGob(const tcomp& newPos, const tcomp& newVel, int tn) :
  wingGob(newPos,newVel,tn),
  regn(pt3d(0,-3.663,0.666),0,tn,&rperRgn),
  legn(pt3d(0, 3.663,0.666),0,tn) {}

gob* saucGob::Control(KeySym k, bool pressed, int *ammo) {
  vhclGob::Control(k,pressed,ammo);
  legn.Control(k,pressed,ammo);
  regn.Control(k,pressed,ammo);

  switch(k) {
    case XK_e:
      if (pressed)
        ctrl.Cart().x= min(ctrl.Cart().x+0.25,MaxVel().Cart().x);  break;
    case XK_d:
      if (pressed)
        ctrl.Cart().x= max(ctrl.Cart().x-0.25,(coord)0);  break;

    case XK_Left:
    case XK_j:     ctrl.Ang().yz= -MaxVel().Ang().yz*pressed; break;
    case XK_Right:
    case XK_l:     ctrl.Ang().yz=  MaxVel().Ang().yz*pressed; break;
    case XK_Up:
    case XK_i:     ctrl.Ang().xz= -MaxVel().Ang().xz*pressed; break;
    case XK_Down:
    case XK_k:     ctrl.Ang().xz=  MaxVel().Ang().xz*pressed; break;

    case XK_space: if (pressed) Fire(ammo); break;
    case XK_z:     if (pressed) return new tankGob(*this); break;

/*
    case XK_g:
      if (pressed) {
        if (parts.empty()) {
          Add(new whelGob(tcomp(pt3d(-2,1), ang3d(0,0,-MA_PI/2)),
                                ang3d(0,0, MaxVel().Ang().yz)));
          Add(new whelGob(tcomp(pt3d(-2,-1),ang3d(0,0, MA_PI/2)),
                                ang3d(0,0,-MaxVel().Ang().yz)));
          Add(new whelGob(tcomp(pt3d(2.5,0),ang3d(0,-MA_PI/2,0)),
                                ang3d(0,MaxVel().Ang().xz,0)));
        }
      }
*/
  }
  return this;
}

void saucGob::DrawStatus(bbGfxTarget& gt, int color1, int color2,
			 bool drawAll) {
  vhclGob::DrawStatus(gt,color1,color2,drawAll);
  ang2d ang= pos.Ang().xz;
  coord z= min(Altitude(),13.);
  if (ang != prevAng or z != prevAltitude or drawAll) {
    pt2d off= EraseStatus(gt,"Elevation & Pitch",pt2d(9,2));
    pt2d sc= (ABS(pos.Ang().yz) > MA_PI/2) ? pt2d(1,1) : pt2d(1,-1);
    ::Draw(&sauc2dRgn,gt,off,-ang,sc);
    if (z <13)
      XDrawLine(gt.disp,gt.win,gt.gc,
		off.x-26, off.y+7+z,
		off.x+26, off.y+7+z);
    prevAng= ang;
    prevAltitude= z;
  }
}

void saucGob::Fire(int *ammo) {
  if (*ammo >0) {
    tmtrx shellPos= tcomp(pt3d(3.5,0,0.5)) * WorldPos();
    pt3d  shellVel= pt3d(muzzleVel,0,muzzleVel/8) * WorldPos()
                    -WorldPos().Cart();
    gobs->push_back(new shllGob(shellPos,shellVel,teamNum,ammo));
    if (useBangs)
      gobs->push_back(new bangGob(shellPos,0));
  }
}

void saucGob::SetVelocity() {
  tcomp desiredVel= ctrl;

  vel.Scale()= pt3d(1,1,1); //-PAH
  tmtrx wap= WorldPos();    // world angular pos
  wap.Cart()= pt3d(0,0,0);
  pt3d up= pt3d(0,0,1) >> wap;

  // five forces act on plane (all in model coords):
  pt3d G= pt3d(0,0,gravity) << wap;
  pt3d R= pt3d(0,0,-(vel.Cart().Dot(up))*0.10);
  pt3d T= pt3d(desiredVel.Cart().x*0.05,0,0);
  T >>= legn.pos.Ang();  // VTOL engines!!!! -PAH
  pt3d D= (vel.Cart()*(-0.02)) << wap;
  pt3d L= pt3d(0,0,(vel.Cart() << wap).x*(-gravity/2));  // when flying 2 meters/iteration, lift equals gravity

  pt3d sum= G+R+T+D+L;
  pt3d worldCartVel= vel.Cart() +(sum >> wap);           // world coords
  vel.Cart()= worldCartVel;
  vel.Ang()= desiredVel.Ang();

  tcomp mv= MaxVel();  // experimental bank-turn stuff
  //vel.Ang().xy= roundToLong(-mv.Ang().xy *sinTbl[pos.Ang().yz&(2*MA_PI-1)]); this works ok..-PAH

  if (ABS(pos.Ang().yz) <MA_PI/2) {
//  vel.Ang().xy= roundToLong(-mv.Ang().xy *sinTbl[pos.Ang().yz&(2*MA_PI-1)]);
//  vel.Ang().xy= roundToLong(-mv.Ang().xy * (pos.Ang().yz /(MA_PI/2.0)) );
    double temp= sinTbl[pos.Ang().yz&(2*MA_PI-1)];  // 0 <= temp <= 1
    vel.Ang().xy= roundToLong(-mv.Ang().xy *temp*temp);
    double tanyz= sinTbl[pos.Ang().yz&(2*MA_PI-1)]/cosTbl[pos.Ang().yz&(2*MA_PI-1)];
    if (desiredVel.Ang().xz==0)
      vel.Ang().xz= roundToLong(min((double)mv.Ang().xz,ABS(tanyz*vel.Ang().xy))
);
  }

  /* All the following stuff is/was part of the attempt to put angular
     velocity into world coordinates

     tcomp modelAngVel= desiredVel.Ang();
     vel= (wap.Inverse() * modelAngVel * wap).Clamped1().Clamped2();
     vel.Cart()= worldCartVel;
     double det=vel[0][0]*(vel[1][1]*vel[2][2] - vel[1][2]*vel[2][1])
              -vel[0][1]*(vel[1][0]*vel[2][2] - vel[1][2]*vel[2][0])
              +vel[0][2]*(vel[1][0]*vel[2][1] - vel[1][1]*vel[2][0]);
     cout << "det=" << det << "\n";
     cout << vel;
     //vel= vel * wap;                       // put velocity into world coords
     //cout << vel << endl;
     //    cout.precision(3);
     //    cout << "R=" << R.Dist() << "  T=" << T.Dist() << "  D=" << D.Dist()
<< "  L=" << L.Dist() << "\t" << vel.Cart() << endl; //-PAH
     */
}

gob* saucGob::Act() {
  SetVelocity();
  //pos= (pos.Plus(vel)).Clamped1().Clamped2();
  //pos= pos.Plus2(vel);
  gob *result= wingGob::Act();
  if (result==this) {
    legn.Act();
    regn.Act();
  }
  return result;
}


/*=========================================================================*/
rpelGob::rpelGob(const tcomp& newPos, const tcomp& newVel, gob *newParent,
		 int tn)
  : vhclGob(newPos,newVel,newParent,tn),
    prop(pt3d(2.1,0,0),0,this,tn) {}

gob* rpelGob::Control(KeySym k, bool pressed,int *ammo) {
  vhclGob::Control(k,pressed,ammo);
  prop.Control(k,pressed,ammo);
  switch(k) {
    case XK_h:  vel.Ang().xz=  MaxVel().Ang().xz * pressed; break;
    case XK_y:  vel.Ang().xz= -MaxVel().Ang().xz * pressed; break;
  }
  return this;
}

void rpelGob::LimitMotion() {
  if (pos.Ang().xz <0) pos.Ang().xz= 0;;
  if (pos.Ang().xz >MA_PI/2)  pos.Ang().xz= MA_PI/2;
}


/*=========================================================================*/
prp3Gob::prp3Gob(const tcomp& newPos, const tcomp& newVel, gob *newParent,
		 int tn)
  : vhclGob(newPos,newVel,newParent,tn) {};

gob *prp3Gob::Act() {
  if (parent) {
    // neither the Solaris CC or AIX xlC compilers will let me access
    // parent->parent, b/c parent is a protected member!?!  So here's
    // a misleading workaround:
    prp3Gob *par= (prp3Gob*) parent;
    // Caution: next line assumes parent's parent is a vhclGob
    vhclGob *parpar= (vhclGob*) par->parent;
    if (parpar)
      vel.Ang().yz= (MA_PI/3) * parpar->ctrl.Cart().x;
  }
  return vhclGob::Act();
}


/*=========================================================================*/
whelGob::whelGob(const tcomp& newPos, const tcomp& newVel)
  : gob(newPos,newVel) {};

void whelGob::LimitMotion() {
  if (pos.Ang()==ang3d(0,0,0)) vel.Ang()= ang3d(0,0,0);
}

#endif


/*=========================================================================*/
double shllGob::power= 1.6;

shllGob::shllGob(const tcomp& newPos, const tcomp& newVel, int tn, int *am)
  : gob(newPos,newVel),
    teamNum(tn),
    ammo(am) {(*ammo)--;}

gob *shllGob::Act() {
  if (Altitude() >0)
    vel.Cart().z += gravity;
  return gob::Act();
}

gob* shllGob::Interact() {
  if (Altitude() <=0) return NULL;

  UpdateWorldPos();

  gobList::iterator gi;
  for_(gi,*gobs) {
    twoGobs result= (**gi).HitByShll(this);
    // for now don't bother checking result.first -PAH
    if (result.second != this) return result.second;
  }
  return this;
}


/*=========================================================================*/
explGob::explGob(const tcomp& newPos, bool newSparkle, const tcomp& newVel)
  : gob(newPos,newVel),
    animNum(-1),
    sparkle(newSparkle)
{
  Solid()= false;
}

int explGob::Color() const {
  if (sparkle and (animNum%3)==2)
    return c_orange;
  else
    return c_red;
}

gob* explGob::Act() {
  animNum++;
  if (animNum==EXPLSIZES)
    return NULL;
  int size= explSizes[animNum];
  pos.Scale()= pt3d(size,size,size);
  gob::Act();
  worldPos= pos;  // because we're using scale here... -PAH
  return this;
}


/*=========================================================================*/
bldgGob::bldgGob(const tcomp& newPos, const tcomp& newVel, int tn) 
  : gob(newPos,newVel),
    teamNum(tn) {};

bool bldgGob::Contains(const pt3d& worldPt, bool inclusive) {
  return gob::Contains(worldPt,inclusive);
  /* this exact code won't work with type BSP_TREE_VISIBILITY
  pt3d modelPt= worldPt << WorldPos();
  return
    Shape()->Box().ContainsInclusively(modelPt) and  // bounding box test
    Shape()->Contains(modelPt,inclusive);      // exact bsp test
  */
}

twoGobs bldgGob::HitByBall(ballGob *hitter) {
  pt3d& ballPos= hitter->WorldPos().Cart();
  if (Contains(ballPos) or 
      Contains(ballPos +hitter->vel.Cart()
	       +hitter->Shape()->Box().Center())) {
    // knock bldg over, if it isn't already falling/fallen
    if (pos.Ang().xz==0)
      vel.Ang().xz= -MA_PI/120;

    // repel the ball out of the bldg
    pt3d kick= ballPos - pos.Cart();
    kick.FastNormalize(0.5);
    if (hitter->vel.Cart().Dot(kick) <0.7)
      hitter->vel.Cart() += kick;
  }
  return twoGobs(this,hitter);
}

twoGobs bldgGob::HitByShll(shllGob *hitter) {
  if (Contains(hitter->pos.Cart()))
    return twoGobs(this,(gob*)NULL);
  return twoGobs(this,hitter);
}

twoGobs bldgGob::HitByVhcl(vhclGob *v) {
  if(KickIfHit(v,0.5,0))
    gobs->push_back(new explGob(v->Center(),false,0));
  return twoGobs(this,v);
}

gob *bldgGob::Act() {
  if (Altitude() >0)
    vel.Cart().z += gravity;
  return gob::Act();
}

gob* bldgGob::Interact() {
  TestForGroundBounce();

  if (pos.Ang().xz <-MA_PI/2 and Altitude()==0) {
    pos.Ang().xz= -MA_PI/2;
    vel.Ang().xz= 0;
    vel.Cart().z= 3*HITBOUNCE;

    gobList::iterator gi;
    for_(gi,*gobs) {
      gob& g= **gi;
      if (g.Altitude()==0 and
	  (g.Shape()== &tankRgn or g.Shape()== &tankBRgn or
	   g.Shape()== &heliRgn or g.Shape()== &trainRgn or
	   g.Shape()== &ballRgn)) {
	coord dist= (g.WorldPos().Cart() -pos.Cart()).FastDist();
	double c= 2*HITBOUNCE*(1 -dist/200);
	if (c <HITBOUNCE/2) c= HITBOUNCE/2;
	g.vel.Cart().z += (coord) c;
      }
    }
  }
  
  UpdateWorldPos();
  return this;
}

void bldgGob::Topple() {
  ang3d ang(0,pos.Ang().xz,0);
  pt3d center= pt3d(-8,0,20) >> ang;
  //vel.Ang().xz += roundToLong((MA_PI/256 * center.x * GRAV) / 2);
  vel.Ang().xz += roundToLong((MA_PI/256 * center.x * (-1./32)) / 2);
}


/*=========================================================================*/
ballGob::ballGob(const tcomp& newPos, const tcomp& newVel, int tn)
  : gob(newPos,newVel),
    teamNum(tn) {};

gob *ballGob::Act() {
  if (Altitude() >0)
    vel.Cart().z += gravity;
  return gob::Act();
}

twoGobs ballGob::HitByShll(shllGob *hitter) {
  if (Contains(hitter->pos.Cart())) {
    teamNum= hitter->teamNum;
    pt2d savedVel= vel.Cart();  // careful: pt2d, not pt3d!
    PushFrom(hitter,1,1,0.25);
    //vel.Cart().pt2d::operator=(savedVel +hitter->vel.Cart()); //xlC unhappy
    vel.Cart().x= savedVel.x +hitter->vel.Cart().x;
    vel.Cart().y= savedVel.y +hitter->vel.Cart().y;
    return twoGobs(this,new explGob(hitter->pos,false));
  }
  return twoGobs(this,hitter);
}

gob* ballGob::Interact() {
  TestForGroundBounce(pt3d(0.66,0.66,-0.66));
  if (Altitude()==0 and vel.Cart().z==0) vel= tcomp(0);
  UpdateWorldPos();

  gobList::iterator gi;
  for_(gi,*gobs)
    (**gi).HitByBall(this);  // handles dribbling, hitting bldg

  return this;
}


/*=========================================================================*/
bounGob::bounGob(const tcomp& newPos, bool kib)
  : gob(newPos,0),
    keepInBounds(kib) {};

bool bounGob::Contains(const pt3d& worldPt, bool inclusive) {
  coord dist;
  side2d *s;
  bound2dRgn.CheckIfContains(worldPt,dist,s);
  return inclusive ? (dist <=0) : (dist <0);
}

twoGobs bounGob::HitByBall(ballGob *hitter) {
  KeepWithin(hitter,pt3d(0,0,2));
  return twoGobs(this,hitter);
}

twoGobs bounGob::HitByShll(shllGob *hitter) {
  KeepWithin(hitter);
  return twoGobs(this,hitter);
}

twoGobs bounGob::HitByVhcl(vhclGob *hitter) {
  KeepWithin(hitter,pt3d(0,0,2));
  return twoGobs(this,hitter);
}

void bounGob::KeepWithin(gob *g, const pt3d& offset) {
  if (not keepInBounds or
      g->parent != NULL or
      g->Shape()== &trainRgn or   // all of these can go out of bounds
      g->Shape()== &planeRgn or
      g->Shape()== &saucerRgn)
    return;
  
  coord dist;
  side2d *s;

  bound2dRgn.CheckIfContains(g->pos.Cart(),dist,s);
  if (dist >0) {
    pt3d norm= pt3d(s->unitNormal.x,s->unitNormal.y,0)*dist;
    g->pos.Cart() -= norm;
    gobs->push_back(new explGob(g->pos.Cart()+offset,false,0));

    g->pos.Cart() -= norm;

    // reflect object's velocity using side's normal vector
    norm= pt3d(s->unitNormal.x,s->unitNormal.y,0);
    dist= g->vel.Cart().Dot(norm);
    if (dist >0) {
      norm *= dist;
      g->vel.Cart() -= norm;
      g->vel.Cart() -= norm;
    }
  }
}


/*=========================================================================*/
miscGob::miscGob(const tcomp& newPos, rgn3* newShape, int newColor)
  : gob(newPos,0),
    shape(newShape),
    color(newColor) {};

twoGobs miscGob::HitByBall(ballGob *hitter) {
  if(KickIfHit(hitter,0.7,0))
    gobs->push_back(new explGob(hitter->Center(),false,0));
  return twoGobs(this,hitter);
}

twoGobs miscGob::HitByShll(shllGob *hitter) {
  if (Contains(hitter->pos.Cart()))
    return twoGobs(this,(gob*)NULL);
  return twoGobs(this,hitter);
}

twoGobs miscGob::HitByVhcl(vhclGob *hitter) {
  KickIfHit(hitter,0.5,0);
  return twoGobs(this,hitter);
}


mntnGob::mntnGob(const tcomp& newPos)
  : miscGob(newPos,&mtnRgn,c_saddleBrown)
{
  if (fancy) {
    int n= (rand() & 0x1f00) >> 8;
    if (n >10) shape= &mtn2Rgn;
    if (n >18) shape= &mtn3Rgn;
    if (n >25) shape= &mtn4Rgn;
  }
}


/*=========================================================================*/
bangGob::bangGob(const tcomp& newPos, const tcomp& newVel)
  : gob(newPos,newVel), animNum(0)
{
  pos.Cart().z += 0.2;
  if ((rand() & 0xf00) == 0xf00)
    pos.Ang().yz= rand() % (2*MA_PI);
  if ((rand() & 0x1f00) == 0x1f00)
    vel.Ang().yz= MA_PI/64;

  UpdateWorldPos();
};

gob* bangGob::Act() {
  animNum++;
  if (animNum==BANGRGNS+1)
    return NULL;
  return gob::Act();
}


/*=========================================================================*/
hrznGob::hrznGob(const tcomp& newPos)
  : gob(newPos,0) {};

void hrznGob::SetWorldPos(const tmtrx& viewPos) {
  tcomp tempPos= viewPos;
  tempPos.Ang().xz= tempPos.Ang().yz= 0;
  worldPos= tempPos;
}


/*=========================================================================*/
tranGob::tranGob(const tcomp& np, const tcomp& nv, int tn,
                 gobList* nt)
  : vhclGob(np,nv,*(nt->begin()),tn),
    turr(pt3d(0,0,5.1),0,this,tn,&turrCRgn),
    track(nt),
    rail(nt->begin())
{
  turr.barr.shape= &barrCRgn;
}

gob* tranGob::Act() {
  if (Altitude() >0)
    vel.Cart().z += gravity;
  else if (vel.Cart().z==0)
    SetVelocity();

  gob *result= vhclGob::Act();
  turr.Act();
  return result;
}

void tranGob::Control(int *ammo,
		      const gob *target, int firingAcc, int firingFrq) {
  turr.Control(ammo,target,firingAcc,firingFrq);
}

gob* tranGob::Control(KeySym k, bool pressed, int *ammo) {
  vhclGob::Control(k,pressed,ammo);
  switch(k) {
    case XK_Up:
    case XK_i:  
      if (pressed and ctrl.Cart().x <=0)
	ctrl.Cart().x += MaxVel().Cart().x;
      break;
    case XK_Down:
    case XK_k:
      if (pressed and ctrl.Cart().x >= 0)
	ctrl.Cart().x -= MaxVel().Cart().x;
      break;
  }

  turr.Control(k,pressed,ammo);
  return this;
}

void tranGob::DrawControls(bbGfxTarget& gt) {
  turr.DrawControls(gt);
}

void tranGob::DrawStatus(bbGfxTarget& gt, int color1, int color2,
			 bool drawAll) {
  vhclGob::DrawStatus(gt,color1,color2,drawAll);
  turr.DrawStatus(gt,color1,color2,drawAll);
}

void tranGob::GetViewOrder(const pt3d& eyePos, gobList& shapes) {
  pt3d eyePosInModel= eyePos << WorldPos();  // eye pos in model coords
  // if the eye pos is above the plane along the top of the train,
  // then the turret it closer, so add it to shapes first;
  // else, add the tank chassis to shapes first
  bool eyeIsAboveTank= (eyePosInModel.z >Shape()->Box().high.z);

  if (eyeIsAboveTank) {
    turr.GetViewOrder(eyePos,shapes);
    shapes.push_back(this);
  } else {
    shapes.push_back(this);
    turr.GetViewOrder(eyePos,shapes);
  }
}

twoGobs tranGob::HitByShll(shllGob *hitter) {
  if (Contains(hitter->pos.Cart())) {
    if (teamNum==hitter->teamNum) {
      if (ffire==transparent)
        return twoGobs(this,hitter);
      if (ffire==blocked)
        return twoGobs(this,(gob*)NULL);
    }

    vel.Cart().z += HITBOUNCE;
    return twoGobs(this,new explGob(hitter->pos));
  }
  return twoGobs(this,hitter);
}

gob* tranGob::Interact() {
  coord railLen= ((railGob*)*rail)->Length();
  if (pos.Cart().x >= railLen) {
    pos.Cart().x -= railLen;
    rail++;                       // move to next rail
    if (rail==track->end())
      rail= track->begin();
  }
  else if (pos.Cart().x <0) {
    if (rail==track->begin())
      rail= track->end();
    rail--;                       // move to prev rail
    railLen= ((railGob*)*rail)->Length();
    pos.Cart().x += railLen;
  }
  parent= *rail;

  gob *result= vhclGob::Interact();
  vel.Cart().y= 0;
  return result;
}

tcomp tranGob::ModelViewPos() const {
  tcomp vp(pt3d(-3,0,0.25) >> turr.pos, turr.pos.Ang());
  vp.Ang().xz += turr.barr.pos.Ang().xz /3;
  return vp;
}


/*=========================================================================*/
railGob::railGob(const tcomp& newPos, coord newLength)
  : gob(newPos,0),
    length(newLength)
{
    shape= &railRgn40;
    if (length >70) shape= &railRgn100;
    if (length >200) shape= &railRgn260;
}

coord railGob::DistFromViewer(const pt3d& eyePos) const {
  if (pos.Cart().z <0.001 and pos.Ang().xz==0)
    return LONG_MAX-2;
  else
    return gob::DistFromViewer(eyePos);
}

rgn3* railGob::Shadow() const {
  if (pos.Cart().z==0 and pos.Ang().xz==0)
    return NULL;
  else
    return gob::Shadow();
}


/*=========================================================================*/
atchGob::atchGob(rgn3 *newShape, gob *newParent)
  : gob(0,0,newParent),
    shape(newShape) {}
