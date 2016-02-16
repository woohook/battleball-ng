// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef PLAYER_h
#define PLAYER_h


#include "bb.h"
#include "shapes.h"
#include "bbgfxtarget.h"
#include "gobs.h"
#include "team.h"


#define MAXDISPCHARS 80


/*=========================================================================*/
struct player {
  char          dispName[MAXDISPCHARS]; // X display name
  int           memberNum;              // team member number
  bool          active;                 // true -> player's X display is open
  bool          computerPlayer;
  bool          usingHqView;            // use view from hq building
  int           ammo;                   // remaining ammo which player has
  tcomp         homePos;                // starting position for vhcl
  bool          textAreaNeedsRedrawing;
  bool          textAreaShowingStatus;  // true -> status, false -> team list

  bbGfxTarget   gt;                     // graphics target
  bool          autoPilot;              // auto-pilot is on
  bool          autoGunner;             // auto-gunner is on
  bool          doubleBuffer;           // double buffer game window
  int           gunnerAccuracy;         // accuracy of autogunner
  int           gunnerFrequency;        // "trigger-happines" of autogunner
  bool          mayShootBall;           // auto gunner may shoot at the ball
  bool          mayShootVhcl;           // auto gunner may shoot at vehicles

 private:
  int           teamNum;          // team number; may be -1 if this player
                                  // isn't on any team, as is the case with
                                  // the dummy "train" player
  //vector<gobRef> gobRefs;       // make this static? -PAH
  gobList::iterator vhcl;         // points to player's vehicle

 public:
                player() {};
                player(const char *disp, int team, int member);
  void          InitForRound(gobList& gobs, coord hqd, int nt);
  void          CloseXStuff();
  void          AutoPlay(gobList&,int,team[],gob&);
  void          HandleKeyPress(KB_Key k, bool p, gobList& gobs);
  void          HandleEvents(gobList& gobs);
  vhclGob*      Vhcl() const;
  void          DrawStatus(const team[]);
  void          ClearTextArea();
  int           TeamNum() const {return teamNum;}

 private:
  team*         TeamToAttack(int,team[],const gob&);
  void          GetDest(const team&,const gob&,pt2d& d,pt2d& toHq);

  // static class members...
 public:
  static bool autoPilotAllowed;    // autopilot is allowed for human players
  static bool autoGunnerAllowed;   // autogunner is allowed for human players
  static bool pauseAllowed;        // pausing game is allowed
  static bool paused;              // game is currently paused
  static bool shadowsOn;           // draw shadows
  static int  defaultAccuracy;
  static int  defaultFrequency;
  static bool mayShootBallDefault;
  static bool mayShootVhclDefault;
  static bool refitWindow;         // true -> window resizes itself to fit
  static int  shellsPerPlayer;     // max # of simultaneous shells per player

  static void SetGunnerDefaults(const char *);
};


#endif
