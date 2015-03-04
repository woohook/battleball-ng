// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include "gob.h"


// Notes:
//   gob is an abbreviation for Game OBject
//   STL is an abbreviation for the Standard Template Library


// static data members
FriendlyFire gob::ffire= dangerous;
bool         gob::fancy= true;
bool         gob::classSolid= true;
coord        gob::gravity= -1./32;   // gravitational acceleration
gobList*     gob::gobs= NULL;


/*=========================================================================*/
// gobList class

gob* gobList::Update(gob* gobPtr, gobList::iterator gi) {
  if (gobPtr != *gi)   // if the new gob ptr is different from the orig one,
    delete *gi;        // delete the orig gob
  if (gobPtr)          // if new gob ptr is non-NULL,
    *gi= gobPtr;       // replace orig gob ptr w/ new gob ptr
  else                 // else
    erase(gi);         // erase the orig gob ptr from the gobList
  return gobPtr;
}



/*=========================================================================*/
// the newVel.Scale() is overriden to be zero.

gob::gob(const tcomp& newPos, const tcomp& newVel, gob *newParent)
  : pos(newPos),
    vel(newVel),
    parent(newParent),
    solid(classSolid),
    lastPosAng(pos.Ang()+ang3d(1,1,1))
{
  vel.Scale()= pt3d(0,0,0);
  UpdateWorldPos();
}


//---------------------------------------------------------------------------
/* Make this gob "act" for one game iteration.  "Acting" typically includes
   one or more of the following:

   o  morphing the gob (changing its shape or class)
   o  setting the gob's velocity
   o  moving the gob
   o  interacting with other gobs

   Act() returns:

   o  NULL, if the gob is destroyed,
   o  a ptr to a new gob, if the gob becomes a different object (as in the
      tank to helicopter transformation), or
   o  the "this" ptr, otherwise
*/
gob* gob::Act() {
  vel.Scale()= pt3d(0,0,0);  // for right now... -PAH
  pos += vel;
  return Interact();
}


//---------------------------------------------------------------------------
// Returns the center of the bounding box around this gob, in world coords

pt3d gob::Center() const {
  return Shape()->Box().Center() * WorldPos();
}


//---------------------------------------------------------------------------
// Returns true if globalPt lies inside of this gob.
// Implemented to test against the gob's bounding box, but can be overridden
// in derived classes with a more accurate test if necessary.
// In:  inclusive = true  -> consider a pt on the surface contained
//                = false -> consider a pt on the surface not contained

bool gob::Contains(const pt3d& globalPt, bool inclusive) {
  if (inclusive)
    return Shape()->Box().ContainsInclusively(globalPt << WorldPos());
  else
    return Shape()->Box().ContainsExclusively(globalPt << WorldPos());
}


//---------------------------------------------------------------------------
// Returns the distance from a viewer's eye position to this gob.
// In:  eyePos = viewer's eye position, in world coords

coord gob::DistFromViewer(const pt3d& eyePos) const {
  pt3d center= Shape()->Box().Center() >> WorldPos(); // in world coords
  return (eyePos - center).Dist();  // or use FastDist()?
}


//---------------------------------------------------------------------------
// Draw this gob.
// In:  worldToView = a world-to-view coordinates transformation matrix
//      viewNum = which view cache to use (helps performance)
//      eyePos = viewer's eye  position, in world coords
//      colors = the color table to use when drawing

void gob::Draw(bbGfxTarget& gt, tmtrx& worldToView, int viewNum,
		 const pt3d& eyePos, const ulong colors[]) {
  while (views.size() <=viewNum)
    views.push_back(view());
  views[viewNum].Update(Shape(),WorldPos()*worldToView);
  gt.SetForeground(gt.Colors()[Color()]);
  Shape()->Draw(views[viewNum].pts,
		gt,
		eyePos << WorldPos(),
		//pt3d(0,0,0) << views[viewNum].modelToView,
		gt.gfxSize,
		Solid(),
		colors ? colors : gt.Colors());
  gt.SetForeground(gt.Black());
}


//---------------------------------------------------------------------------
/* GetViewOrder()

   This function adds this gob's ptr to a list of gobs to be drawn.  If this
   gob has any child gobs (i.e. if this is a multi-part gob), the childrens'
   ptrs are added to the list too.  The list of gobs will be drawn beginning
   with the last gob first.

   The purpose of this function is to allow multi-part gobs to be drawn
   correctly.  The eye position can be checked to decide whether this gob
   should be drawn first, or whether its part(s) should be drawn first.

   This essentially allows a BSP tree to implicitly embedded into multi-part
   gobs...
*/


//---------------------------------------------------------------------------
/* HitByBall(ballGob*)
   HitByShll(shllGob*)
   HitByVhcl(vhclGob*)

   These functions detect and handle collisions between this gob and the
   ball gob, a shell gob, and a vehicle gob.

   By default, the base gob class does not detect collisions with balls/
   shells/vehicles.  To make a derived gob class detect and handle these
   collisions, override the appropriate HitBy*() function(s).

   The HitBy*() functions return two gob ptrs, one for this gob and one for
   the "hitter" gob.  These returned ptrs have the same semantics as the
   ptr returned by Act().
*/


//---------------------------------------------------------------------------
/* Interact after moving.

   This function is called by Act() after a gob is moved, so that the gob
   may react appropriately to the results of its motion.

   Typically, Interact() will:

   o  Adjust the gob's position if it passes some intrinsic limit.  For
      example, the angular position of the tank's gun barrel is intrinsically
      limited to be between 0 and 25 degrees vertically.

   o  Update the gob's world position (i.e. its model-to-world transformation
      matrix)

   o  Test for interaction with other gobs.  This step requires that the
      gob's world position is up-to-date.

   Upon entrance to Interact(), the gob's world position (worldPos) is
   typically *not* up-to-date.  Upon exit, Interact() must ensure that
   worldPos consistent with pos and the worldPos of this gob's parent.

   Interact()'s return value has the same semantics as Act()'s return value.
*/

gob* gob::Interact() {
  UpdateWorldPos();
  return this;
}


//---------------------------------------------------------------------------
// Tests if this gob and another gob have collided, and applies a "kick" if
// they have.  The other gob is usually the ball gob.
// Returns true if a kick was applied.

bool gob::KickIfHit(gob *ball, coord maxVel, coord zvel, coord acc,
		    bool kickBoth) {
  pt3d ballPos= ball->WorldPos().Cart();
  ballPos.z += ball->Shape()->Box().Center().z;

  if (Contains(ballPos)) {
    pt3d selfToBall= ball->WorldPos().Cart() -WorldPos().Cart();
    pt3d kick= selfToBall;
    if (zvel==0) kick.z= zvel;
    kick.Normalize(acc);
    if (zvel != 1 and Altitude() <0.001) kick.z= zvel;

    if (kick.Dot(ball->vel.Cart()) <maxVel)
      ball->vel.Cart() += kick;
    if (kickBoth and (-kick).Dot(vel.Cart()) <maxVel)
      vel.Cart() -= kick;
			
    return true;
  }
  return false;
}


//---------------------------------------------------------------------------
// ModelViewPos() returns a gob's view position and direction in model
// coordinates.


//---------------------------------------------------------------------------
/* Push this gob because it was hit by a shell.

   The effects of the shell's hit are both cartesian pushing and angular
   spinning.  Hits in line with the center of the hit gob tend to induce
   only pushing; hits on the edge of the hit gob tend to induce only
   spinning.
*/
void gob::PushFrom(const gob *shell, double pushMult, double spinMult,
		    coord groundBounce) {
  coord R= Shape()->FarthestDist();  // approx. radius of self
  //assert(R >0);

  // compute distance from shell's (infinite) path to center of self
  pt2d shellToSelf= pos.Cart() -shell->pos.Cart();
  pt2d shellVelPerp= shell->vel.Cart().pt2d::Left90().Normalized();
  coord d= shellToSelf.Dot( shellVelPerp );
  if (d >R)  d=  R; // use assert here?
  if (d <-R) d= -R;
  
  double torque= d/R;
  pt3d push= shell->vel.Cart().Normalized((1 -ABS(torque)) *pushMult);
  if (groundBounce != 0 and Altitude() <0.001)
    push.z= groundBounce;
  ang2d xyspin= (ang2d) ((MA_PI/64) *torque *spinMult);

  vel.Ang().xy += xyspin;
  vel.Cart() += push;
}


//---------------------------------------------------------------------------
// TeamNum() returns the number of the team associated with this gob, or -1
// if this gob isn't associated with any team.


//---------------------------------------------------------------------------
// Bounce this gob against the ground, if its altitude drops below zero.
void gob::TestForGroundBounce(const pt3d& bounceFactor, bool wantUpdate) {
  coord altitude= Altitude();
  if (altitude <0) {
    pos.Cart().z -= 2*altitude;  // bounce off ground
    vel.Cart().x *= bounceFactor.x;
    vel.Cart().y *= bounceFactor.y;
    vel.Cart().z *= bounceFactor.z;
    if (vel.Cart().z +2*gravity <= 0) {
      vel.Cart().z= 0;
      pos.Cart().z= -Shape()->Box().low.z;
    }
    if (wantUpdate)
      UpdateWorldPos();
  }
}


//---------------------------------------------------------------------------
// Update this gob's world position, i.e. its model-to-world transformation
// matrix.

void gob::UpdateWorldPos() {
  if (parent)
    worldPos= pos * parent->WorldPos();
  else {
    // this test saves computation time, but is a bit of a kludge... -PAH
    if (pos.Ang()==lastPosAng)
      worldPos.Cart()= pos.Cart();
    else {
      worldPos= pos;
      lastPosAng= pos.Ang();
    }
  }
}

//---------------------------------------------------------------------------
// Returns a gob's view position & direction in world coordinates

tmtrx gob::ViewPos() const {
  return ModelViewPos() * WorldPos();
}


//---------------------------------------------------------------------------
// Returns true if this gob is hit by a ray which starts at "startPos" and
// points in direction "dir".  Not too exact, and only considers 2-D.

// Used to determine when a vehicle has the ball lined up with a building.

bool gob::WillBeHitBy(const pt3d& startPos, const pt3d& dir) const {
  pt3d low=  Shape()->Box().low;
  pt3d high= Shape()->Box().high;
  pt2d p1= pt3d(high.x,low.y,0)  * WorldPos() -startPos;
  pt2d p2= pt3d(high.x,high.y,0) * WorldPos() -startPos;

  // This gob won't be hit if it isn't in "front" of the projectile
  if (dir.pt2d::Dot(p1) <0 and dir.pt2d::Dot(p2) <0)
    return false;

  // This gob will be hit if the projectile's path passes between the two
  // corners on this gob's front
  pt2d left= dir.Left90();
  return (left.Dot(p1) >0) != (left.Dot(p2) >0);
}


//===========================================================================
// Draw a region2d.
// off is in screen coords.

void Draw(rgn2 *rgn, gfxTarget& gt, const pt2d& off, ang2d ang,
	  const pt2d& sc, bool solid) {

#ifdef __GNUC__
  pt2d pts[100];
  XPoint xpts[100];
#else
  pt2d *pts= new pt2d[rgn->numSides];
  XPoint *xpts= new XPoint[rgn->numSides+1];
#endif
  rgn->TransformTo(off,ang,sc,pts);
  forii(rgn->numSides) {
    xpts[i].x= (short) pts[i].x;
    xpts[i].y= (short) pts[i].y;
  }
  xpts[rgn->numSides].x= (short) pts[0].x;
  xpts[rgn->numSides].y= (short) pts[0].y;
  if (solid)
    XFillPolygon(gt.disp,gt.win,gt.gc,xpts,rgn->numSides+1,Nonconvex,
		 CoordModeOrigin);
  else
    XDrawLines(gt.disp,gt.win,gt.gc,xpts,rgn->numSides+1,CoordModeOrigin);
#ifndef __GNUC__
  delete [] xpts;
  delete [] pts;
#endif
}


//---------------------------------------------------------------------------
// Simulates the motion of a shell and a target, and returns info about the
// closest point of approach.  The shell is influenced by gravity, but the
// target is not.

void GetClosestApproach(const pt3d& shellInitPos, const pt3d& shellInitVel,
			const pt3d& tgtInitPos,   const pt3d& tgtInitVel,
			coord grav, int maxShotTime,
			coord& closestDist, double& closestTime,
			pt3d& closestShellPos, pt3d& closestTgtPos) {

  closestDist= 1000;   // one kilometer!
  closestTime= 0;
  pt3d tgtPos= tgtInitPos;
  pt3d shellPos= shellInitPos;
  pt3d shellVel= shellInitVel;

  forii(maxShotTime) {
    coord d= (tgtPos-shellPos).Dist();
    if (d <closestDist) {
      closestDist= d;
      closestTime= i;
      closestShellPos= shellPos;
      closestTgtPos= tgtPos;
    }
    tgtPos += tgtInitVel;
    shellPos += shellVel;
    shellVel.z += grav;
  }
}
