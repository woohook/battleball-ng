// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef BBGOBS_h
#define BBGOBS_h


#include "gob.h"
#include "shapes.h"


// Notes:
//   gob is an abbreviation for Game OBject
//   STL is an abbreviation for the Standard Template Library


const int EXPLSIZES= 10;


//===========================================================================
// Vehicle gob class.  This is a base class for vehicle gobs like the tank,
// helicopter, and plane, as well as for vehicle "parts", like the tank
// turret or the helicopter blade.

struct vhclGob : public gob {
  bool          newlyCreated;  // set to true when created; used by client
  tcomp         ctrl;          // how this vhcl is trying to move itself
  int           teamNum;       // team number; -1 if no team association
  int           iAmHit;        // >0 if vhcl was recently hit by a shell
  double        rangeError;    // factor used to (mis-)estimate target range

                vhclGob(const tcomp& p=tcomp(0),
			const tcomp& v=tcomp(0), gob *par=NULL, int tn=-1);
                vhclGob(const vhclGob& g);
  gob*          Act();
  virtual bool  CanMakeGoal(const gob& hq, const gob& ball) const;
  void          ComputeMiss(const gob* tgt,int acc,pt3d& miss,double& time);
  virtual gob*  Control(KB_Key,bool pressed,int*);
  virtual void  Control(const pt3d& d,const pt3d& dd,tcomp& std);
  virtual void  Control(int *ammo,const gob *target, int acc, int frq) {};
  virtual void  DrawControls(bbGfxTarget&) {}
  virtual void  DrawStatus(bbGfxTarget&,int c1,int c2,bool all=true);
  virtual void  Fire(int* ammo) {};
  void          FireIfReady(int* am,const gob* tgt,pt3d& mv,int acc,int frq);
  virtual void  GetShellPos(tmtrx& p, pt3d& v) {p= v= pt3d(0,0,0);}
  twoGobs       HitByVhcl(vhclGob *h);
  gob*          Interact();
  bool          IsVhcl() const {return true;}
  virtual coord MaxRange() const {return 0;}
  gob*          PreferredVhclTarget(const pt3d& bp) const;
  void          SetVelocity();
  int		TeamNum() const {return teamNum;}

  static coord muzzleVel;  // initial shell velocity
  static vhclGob* testVhcl;
  static pt3d testVel;
  static bool useBangs;
};


//===========================================================================
struct barrGob : vhclGob {
  ang2d prevAng;
  rgn3*   shape;

                barrGob(const tcomp& np, const tcomp& nv, gob *par, int tn,
			rgn3* ns=NULL);
  int           Color() const {return iAmHit ? c_red : c_navy;}
  gob*          Control(KB_Key,bool pressed,int*);
  void          DrawStatus(bbGfxTarget&,int c1,int c2,bool all=true);
  void          Fire(int*);
  void          GetShellPos(tmtrx& p, pt3d& v);
  gob*          Interact();
  coord         MaxRange() const;
  tcomp         MaxVel() const {return tcomp(pt3d(0,0,0),ang3d(0,MA_PI/128));}
  rgn3*         Shape() const;
};


//===========================================================================
struct turrGob : vhclGob {
  barrGob barr;
  ang2d   prevAng;
  rgn3*   shape;

                turrGob(const tcomp& np, const tcomp& nv, gob *par, int tn,
			rgn3* ns=NULL);
                turrGob(const turr_bladGob&, gob *par);
  gob*          Act();
  int           Color() const {return iAmHit ? c_red : c_navy;}
  gob*          Control(KB_Key,bool pressed,int*);
  void          Control(int *ammo,const gob *target, int acc, int frq);
  void          DrawControls(bbGfxTarget&);
  void          DrawStatus(bbGfxTarget&,int c1,int c2,bool all=true);
  void          EyeIsInside(bool in);
  void          Fire(int* ammo) {barr.Fire(ammo);}
  void          GetShellPos(tmtrx& p, pt3d& v) {barr.GetShellPos(p,v);}
  void          GetViewOrder(const pt3d& eyePos,gobList& shapes);
  gob*          Interact();
  coord         MaxRange() const {return barr.MaxRange();}
  tcomp         MaxVel() const {return tcomp(pt3d(0,0,0),MA_PI/128);}
  rgn3*         Shape() const;
};


//===========================================================================
// A gob immobily attached to a parent gob
struct atchGob : gob {
  rgn3 *shape;

                atchGob(rgn3 *ns, gob *par);
  int           Color() const {return c_saddleBrown;}
  rgn3*         Shape() const {return shape;}
  tmtrx&	WorldPos() {return parent->WorldPos();}
  const tmtrx&	WorldPos() const {return parent->WorldPos();}
};


//===========================================================================
struct tankGob : vhclGob {
  turrGob turr;
  atchGob fin;

                tankGob(const tcomp& np, const tcomp& nv, int tn);
                tankGob(const vhclGob&);
                tankGob(const tank_heliGob&);
  gob*          Act();
  gob*          Interact();
  bool          CanMakeGoal(const gob& hq, const gob& ball) const
                  {return turr.CanMakeGoal(hq,ball);}
  bool          Contains(const pt3d& worldPt,bool incl=true);
  int           Color() const {return iAmHit ? c_red : c_navy;}
  gob*          Control(KB_Key,bool pressed,int*);
  void          Control(const pt3d& d,const pt3d& dd,tcomp& std);
  void          Control(int *ammo,const gob *target, int acc, int frq);
  void          DrawControls(bbGfxTarget&);
  void          DrawStatus(bbGfxTarget&,int c1,int c2,bool all=true);
  void          GetShellPos(tmtrx& p, pt3d& v) {turr.GetShellPos(p,v);}
  void          GetViewOrder(const pt3d& eyePos,gobList& shapes);
  tcomp         MaxAcc() const {return tcomp(pt3d(1./8,0,0),MaxVel().Ang());}
  coord         MaxRange() const {return turr.MaxRange();}
  tcomp         MaxVel() const {return tcomp(pt3d(0.5,0,0),MA_PI/64);}
  tcomp         ModelViewPos() const;
  rgn3*         Shape() const {return fancy ? &tankBRgn : &tankRgn;}
  void          EyeIsInside(bool in) {turr.EyeIsInside(in);}
  twoGobs       HitByShll(shllGob *h);
  twoGobs       HitByBall(ballGob *h);
};


//===========================================================================
struct bladGob : vhclGob {
                bladGob(const tcomp& np, const tcomp& nv, gob *par, int tn);
                bladGob(const turr_bladGob&, gob *par);
  gob*          Act();
  int           Color() const {return c_navy;}
  gob*          Interact() {return gob::Interact();}
  rgn3*         Shape() const {return &bladeRgn;}
};


//===========================================================================
struct heliGob : vhclGob {
  ang2d prevAng;
  coord prevAltitude;
  bladGob blad;

                heliGob(const tcomp& np, const tcomp& nv, int tn);
                heliGob(const tank_heliGob&);
  gob*          Act();
  int           Color() const {return iAmHit ? c_red : c_navy;}
  gob*          Control(KB_Key,bool pressed,int*);
  void          Control(const pt3d& d,const pt3d& dd,tcomp& std);
  void          Control(int *ammo,const gob *target, int acc, int frq);
  void          DrawControls(bbGfxTarget&);
  void          DrawStatus(bbGfxTarget&,int c1,int c2,bool all=true);
  void          Fire(int*);
  void          GetShellPos(tmtrx& p, pt3d& v);
  void          GetViewOrder(const pt3d& eyePos,gobList& shapes);
  gob*          Interact();
  tcomp         ModelViewPos()      const {return pt3d(-2.3,0,2.2);} //-2,0,2.5
  tcomp         MaxAcc() const {return tcomp(pt3d(0.05,0.03,0.05),
                                             ang3d(MA_PI/512,MA_PI/512,MA_PI/512
));}
  coord         MaxRange() const;
  tcomp         MaxVel() const {return tcomp(pt3d(0.80,0.15,0.25),
                                             ang3d(MA_PI/64,MA_PI/128,MA_PI/64))
;}
  rgn3*         Shape() const {return &heliRgn;}
  twoGobs       HitByShll(shllGob *h);
  twoGobs       HitByBall(ballGob *h);
};


//===========================================================================
struct turr_bladGob : vhclGob {
  int animNum, animDir;
  ang2d angxyAcc;

                turr_bladGob(const turrGob&,gob *par);
                turr_bladGob(const bladGob&,gob *par);
  gob*          Act();
  int           Color() const {return c_navy;}
  gob*          Interact() {return gob::Interact();}
  rgn3*         Shape() const;
};


//===========================================================================
struct tank_heliGob : vhclGob {
  turr_bladGob  turrBlad;
  int animNum, animDir;

                tank_heliGob(const tankGob&);
                tank_heliGob(const heliGob&);
  gob*          Act();
  gob*          Interact();
  int           Color() const {return iAmHit ? c_red : c_navy;}
  void          EyeIsInside(bool in);
  void          GetViewOrder(const pt3d& eyePos,gobList& shapes);
  tcomp         ModelViewPos() const;
  twoGobs       HitByShll(shllGob *h);
  twoGobs       HitByBall(ballGob *h);
  rgn3*         Shape() const;
};


//===========================================================================
// Fixed-wing aircraft base class.

struct wingGob : vhclGob {
  ang2d prevAng;
  coord prevAltitude;
  int animNum,animDir;

                wingGob(const tcomp& np, const tcomp& nv, int tn)
		  : vhclGob(np,nv,NULL,tn), animNum(0), animDir(0) {}
  gob*          Act();
  void          Control(const pt3d& d,const pt3d& dd,tcomp& std);
  twoGobs       HitByShll(shllGob *h);
};


//===========================================================================
struct plneGob : wingGob {
                plneGob(const tcomp& np, const tcomp& nv, int tn);
  gob*          Act();
  int           Color() const {return c_navy;}
  tcomp         MaxAcc() const {return tcomp(pt3d(0.05,0,0.05),
                                             ang3d(MA_PI/512,
						   MA_PI/512,
						   MA_PI/512));}
  tcomp         MaxVel() const {return tcomp(pt3d(1.1,0,0),
                                             ang3d(0,MA_PI/128,MA_PI/64));}
  rgn3*         Shape() const {return &planeRgn;}
};


//===========================================================================
struct saucGob : wingGob {
                saucGob(const tcomp& np, const tcomp& nv, int tn);
  gob*          Act();
  int           Color() const {return c_forestGreen;}
  tcomp         MaxAcc() const {return tcomp(pt3d(0.05,0,0.05),
                                             ang3d(MA_PI/512,
						   MA_PI/512,
						   MA_PI/512));}
  tcomp         MaxVel() const {return tcomp(pt3d(1.5,0,0),
                                             ang3d(3*MA_PI/1024,MA_PI/64,MA_PI/64));}
  rgn3*         Shape() const {return &saucerRgn;}
};


#ifdef NONRELEASE

//===========================================================================
struct saucGob : wingGob {
  rpelGob legn, regn; // left & right engines

                saucGob(const tcomp& np, const tcomp& nv, int tn);
  gob*          Act();
  int           Color() const {return c_forestGreen;}
  gob*          Control(KB_Key,bool pressed,int*);
  void          DrawStatus(bbGfxTarget&,int c1,int c2,bool all=true);
  void          Fire(int*);
  tcomp         ModelViewPos()      const {return pt3d(0,0,0.6875);}
  void          SetVelocity();
  tcomp         MaxVel() const {return tcomp(pt3d(1.5,0,0),
                                             ang3d(3*MA_PI/1024,MA_PI/64,MA_PI/64));}
  rgn3*         Shape() const {return &vtolRgn;}
};


//===========================================================================
struct rpelGob : vhclGob {
  prp3Gob prop;
  rgn3     *shape;

                rpelGob(const tcomp& np, const tcomp& nv, gob *par, int tn,
			const rgn3* = &rpelRgn);
  int           Color() const {return c_steelBlue2;}
  gob*          Control(KB_Key,bool pressed,int*);
  void          LimitMotion();
  tcomp         MaxVel() const {return tcomp(pt3d(0,0,0),
                                             ang3d(0,MA_PI/128,0));}
  rgn3*         Shape() const {return shape;}
};


//===========================================================================
struct prp3Gob : vhclGob {
                prp3Gob(const tcomp& np, const tcomp& nv, gob *par, int tn);
  gob*          Act();
  int           Color() const {return c_blue;}
  tcomp         MaxVel() const {return tcomp(pt3d(0,0,0),
                                             ang3d(0,0,MA_PI/3));}
  rgn3*         Shape() const {return &prp3Rgn;}
};


//===========================================================================
struct whelGob : gob {
                whelGob(const tcomp& np, const tcomp& nv, gob *par);
  void          LimitMotion();
  tcomp         MaxVel() const {return ang3d(0,MA_PI/128,MA_PI/128);}
  rgn3*         Shape() const {return &wheelRgn;}
};
#endif


//===========================================================================
struct shllGob : gob {
  int           teamNum;        // team num of player that fired shell
  int*          ammo;           // remaining ammo of player

                shllGob(const tcomp& np, const tcomp& nv, int tn, int *am);
                ~shllGob() {(*ammo)++;}
  gob*          Act();
  gob*          Interact();
  tcomp         MaxVel() const {return tcomp(pt3d(1.5,0,0));}
  rgn3*         Shape() const {return &shellRgn;}

  static double power;
};


//===========================================================================
struct explGob : gob {
  int  animNum;
  bool sparkle;

                explGob(const tcomp& np, bool sprk=true,
                        const tcomp& nv=ang3d(MA_PI/8,MA_PI/8,0));
  gob*          Act();
  int           Color() const;
  rgn3*         Shape() const {return &explRgn;}
};


//===========================================================================
struct bldgGob : gob {
  int  teamNum;

                bldgGob(const tcomp& np, const tcomp& nv, int tn);
  gob*          Act();
  bool          Contains(const pt3d&,bool incl=true);
  twoGobs       HitByBall(ballGob *h);
  twoGobs       HitByShll(shllGob *h);
  twoGobs       HitByVhcl(vhclGob *h);
  gob*          Interact();
  tcomp         ModelViewPos() const {return pt3d(0,0,32);}
  rgn3*         Shape() const {return &hqRgns[TeamNum()];}
  int		TeamNum() const {return teamNum;}
  void          Topple();
};


//===========================================================================
struct ballGob : gob {
  int           teamNum;        // number of team that last moved the ball
                ballGob(const tcomp& np, const tcomp& nv, int tn);
  gob*          Act();
  twoGobs       HitByShll(shllGob *h);
  gob*          Interact();
  rgn3*         Shape() const {return &ballRgn;}
  int		TeamNum() const {return teamNum;}
};


//===========================================================================
struct bounGob : gob {
  bool          keepInBounds;

                bounGob(const tcomp& np, bool kib);
  bool          Contains(const pt3d& worldPt,bool incl=true);
  coord		DistFromViewer(const pt3d&) const {return LONG_MAX-1;}
  twoGobs       HitByBall(ballGob *h);
  twoGobs       HitByShll(shllGob *h);
  twoGobs       HitByVhcl(vhclGob *h);
  void          KeepWithin(gob*,const pt3d& =pt3d(0,0,0));
  rgn3*         Shape() const {return &boundRgn;}
};


//===========================================================================
struct miscGob : gob {
  rgn3* shape;
  int   color;
                miscGob(const tcomp& np, rgn3* ns, int nc=c_navy);
  int           Color() const {return color;}
  twoGobs       HitByBall(ballGob *h);
  twoGobs       HitByShll(shllGob *h);
  twoGobs       HitByVhcl(vhclGob *h);
  rgn3*         Shape() const {return shape;}
};

struct treeGob : miscGob {
  treeGob(const tcomp& np) : miscGob(np,&treeRgn,c_forestGreen) {};
};
  
struct mntnGob : miscGob {
  mntnGob(const tcomp& np);
};

struct pllrGob : miscGob {
  pllrGob(const tcomp& np) : miscGob(np,&pillarRgn,c_gray60) {};
};
  

//===========================================================================
struct bangGob : gob {
  int animNum;
                bangGob(const tcomp& np, const tcomp& nv);
  gob*          Act();
  int           Color() const {return c_orange;}
  rgn3*         Shape() const {return &bangRgns[animNum ? animNum-1 : 0];}
};


//===========================================================================
struct hrznGob : gob {
                hrznGob(const tcomp& np);
  coord		DistFromViewer(const pt3d&) const {return LONG_MAX;}
  int           Color() const {return c_lawnGreen;}
  void          SetWorldPos(const tmtrx& viewPos);
  rgn3*         Shape() const {return &horizonRgn;}
};


//===========================================================================
struct tranGob : vhclGob {
  turrGob turr;
  gobList *track;           // ptr to a list of railGobs
  gobList::iterator  rail;  // the rail that the train is currently on

                tranGob(const tcomp& np, const tcomp& nv, int tn,
			gobList*);
  gob*          Act();
  int           Color() const {return c_gray50;}
  void          Control(const pt3d& d,const pt3d& dd,tcomp& std)
                  {ctrl=MaxVel();}
  void          Control(int *ammo,const gob *target, int acc, int frq);
  gob*          Control(KB_Key,bool pressed,int*);
  void          DrawControls(bbGfxTarget&);
  void          DrawStatus(bbGfxTarget&,int c1,int c2,bool all=true);
  void          GetViewOrder(const pt3d& eyePos,gobList& shapes);
  gob*          Interact();
  void          EyeIsInside(bool in) {turr.EyeIsInside(in);}
  tcomp         MaxAcc() const {return tcomp(pt3d(0.05,0,0),0);}
  coord         MaxRange() const {return turr.MaxRange();}
  tcomp         MaxVel() const {return tcomp(pt3d(0.7,0,0),0);}
  tcomp         ModelViewPos() const;
  rgn3*         Shape() const {return &trainRgn;}
  twoGobs       HitByShll(shllGob *h);
  twoGobs       HitByVhcl(vhclGob *h) {return twoGobs(this,h);}
};


//===========================================================================
struct railGob : gob {
  rgn3 *shape;
  coord length;
                railGob(const tcomp& np,coord nl);
  int           Color() const {return c_saddleBrown;}
  coord		DistFromViewer(const pt3d&) const;
  coord         Length() const {return length;}
  rgn3*         Shadow() const;
  rgn3*         Shape() const {return shape;}
};


#endif
