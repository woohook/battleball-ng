// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.



#ifndef BBGOB_h
#define BBGOB_h


#include <list.h>
#include <vector.h>

#include "view.h"
#include "bb.h"
#include "bbgfxtarget.h"


// Notes:
//   gob is an abbreviation for Game OBject
//   STL is an abbreviation for the Standard Template Library


struct gob;  // forward reference

/*=========================================================================*/
// gob "Reference" class.  gobRef's can be sorted by their distance from the
// view position, which is their main purpose.

struct gobRef {
  gob*	gobPtr;
  coord		distFromViewer; // for depth sorting

  gobRef() {};
  gobRef(gob *g, coord d=0) : gobPtr(g), distFromViewer(d) {};
  ~gobRef() {};   // so STL will shut up its warnings

  bool operator==(const gobRef& mr) const {return distFromViewer==mr.distFromViewer;}
  bool operator< (const gobRef& mr) const {return distFromViewer <mr.distFromViewer;}
  gob& operator*() {return *gobPtr;}
};



/*=========================================================================*/
// gob list class.  Derived from the STL list<> template

#ifdef __GNUC__
struct gobList : public list<gob*> {    // bug in g++, yes?
#else
struct gobList : list<gob*> {
#endif

  // no additional data members

  gobList() {};  // default ctor, automatically calls list<gob*>()
  gobList(const list<gob*>& gl) : list<gob*>(gl) {};  // copy constructor
  void operator=(const gobList& gl) {list<gob*>::operator=(gl);}
  gob* Update(gob*,iterator); //handles when a gob changes itself or dies
};



/*=========================================================================*/
typedef pair<gob*,gob*> twoGobs;

// More fwd references
struct vhclGob;
struct tankGob; struct turrGob; struct barrGob; struct shllGob;
struct bldgGob; struct ballGob; struct bounGob; struct heliGob;
struct bladGob; struct tank_heliGob; struct turr_bladGob; struct explGob;
struct plneGob; struct saucGob; struct tranGob;
struct miscGob; struct treeGob; struct mntnGob; struct pllrGob;
struct bangGob; struct hrznGob; struct whelGob; struct atchGob;


/*=========================================================================*/
/* Game OBject base class.  gob is the base class for all the game object
   classes.  The gob class itself is not meant to be instantiated; its
   derived classes are meant to be instantiated.

   Examples of game objects are the tank, helicopter, ball, hq building,
   tree, shell, the tank's turret, the tank's gun barrel, etc., etc.

   Member functions are listed in alphabetical order.  They are documented
   in gob.C.
*/


enum FriendlyFire {transparent, blocked, dangerous};

struct gob {
  tcomp		pos,vel;	// position, velocity tcomps
  gob           *parent;        // parent gob; NULL if none
 protected:
  tmtrx		worldPos;	// transform from model to world coords
 private:
  vector<view>	views;		// one view of this gob for EACH player
  bool		solid;		// true -> draw gob as a solid
  ang3d         lastPosAng;     // last angular pos; used to update worldPos


 public:
  		        gob(const tcomp& p=tcomp(0),const tcomp& v=tcomp(0),
			    gob *par=NULL);
  virtual	        ~gob() {};

  virtual gob*		Act();
  coord			Altitude() const {return pos.Cart().z+Shape()->Box().low.z;}
  pt3d			Center() const;
  virtual int		Color() const {return c_black;}
  virtual bool          Contains(const pt3d&,bool incl=true);
  virtual coord		DistFromViewer(const pt3d&) const;
  void			Draw(bbGfxTarget&,tmtrx&,int,const pt3d&,
			     const ulong colors[]=NULL);
  virtual void          EyeIsInside(bool in) {solid=classSolid and (not in);}
  virtual void          GetViewOrder(const pt3d& eyePos, gobList& shapes)
                          {shapes.push_back(this);}

  virtual twoGobs	HitByBall(ballGob *h) {return twoGobs((gob*)this,(gob*)h);}
  virtual twoGobs	HitByShll(shllGob *h) {return twoGobs((gob*)this,(gob*)h);}
  virtual twoGobs	HitByVhcl(vhclGob *h) {return twoGobs((gob*)this,(gob*)h);}

  virtual gob*		Interact();
  virtual bool		IsVhcl() const {return false;}
  bool			KickIfHit(gob*,coord mv=0.5,coord zv=1,coord a=0.5,
				  bool kb=false);
  virtual tcomp		MaxAcc() const {return tcomp(0);}
  virtual tcomp		MaxVel() const {return tcomp(0);}
  virtual tcomp		ModelViewPos() const {return tcomp(0);}
  void			PushFrom(const gob*,double pm=1,double sm=1,
				 coord gb=0);
  virtual rgn3*         Shadow() const {return (rgn3*)(Shape()->Shadow());}
  virtual rgn3*         Shape() const {return NULL;}
  virtual bool&		Solid() {return solid;}
  virtual bool		Solid() const {return solid;}
  virtual int		TeamNum() const {return -1;}
  void                  UpdateWorldPos();
  const view&		View(int n) const {return views[n];}
  tmtrx			ViewPos() const;
  bool			WillBeHitBy(const pt3d& pos, const pt3d& dir) const;
  virtual tmtrx&	WorldPos() {return worldPos;}
  virtual const tmtrx&	WorldPos() const {return worldPos;}
  
 protected:
  void			TestForGroundBounce(const pt3d& bf=pt3d(1,1,-0.5),
					    bool wantUpdate=false);


  //static class stuff: -----
 public:
  static bool		classSolid;	// default for gob .solid field
  static bool		fancy;		// true -> use fancier graphics
  static FriendlyFire	ffire;		// what happens when shot by a teammate
  static coord		gravity;	// gravitational acceleration
  static gobList*       gobs;
};


//---------------------------------------------------------------------------
/* This function doesn't belong in gob.h! :-) -PAH

   Returns the distance (i.e. max range) that a projectile will travel when
   fired at a given angle, from a given elevation, in the presence
   of gravity.

   In:  a = firing angle
        p = initial elevation
	v = firing velocity
	g = gravitational acceleration
*/
inline coord Range(ang2d a, coord p, coord v, coord g) {
  //assert(p >= 0);
  //if (p==0) return -sqr(vel)*sinTbl[ang*2]/grav;
  double timeInAir= -(v * sinTbl[a] + sqrt(sqr(v * sinTbl[a]) - g*p)) / g;
  return v * cosTbl[a] * timeInAir;
}



void Draw(rgn2 *rgn, gfxTarget& gt, const pt2d& off=pt2d(0,0), 
	  ang2d ang=0, const pt2d& sc=pt2d(1,1),bool solid=false);

void GetClosestApproach(const pt3d& shellInitPos, const pt3d& shelInitlVel,
			const pt3d& tgtInitPos,   const pt3d& tgtInitVel,
			coord grav, int maxShotTime,
			coord& closestDist, double& closestTime,
			pt3d& closestShellPos, pt3d& closestTgtPos);


#endif
