// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.

#ifndef BATTLEBALL_h
#define BATTLEBALL_h

#include "player.h"
#include "battleballgame.h"

const int
  MAXTEAMS= NUMINSIGNIA,        // max # of teams is the # of team insignias
  MAXMEMBERS= 4,                // max # of team members per team
  MAXPLAYERS= MAXTEAMS*MAXMEMBERS, // max # of total players
  TREES= 12,                    // default # of trees
  MTNS= 8;                      // default # of mountains

const coord
  HQDIST= 100,                  // default dist from hq to center of playfield
  MINHQDIST= 50,                // min     dist from hq to center of playfield
  MAXHQDIST= 450;               // max     dist from hq to center of playfield



/*=========================================================================*/
// The states of a game round
enum stateOfRound {
  initializing,         // a new round is about to start
  counting,             // timer is counting down to begin playing
                        // (teams shown in text area)
  playing,              // playing the round; no goal scored yet
                        // (game status shown in text area)
  toppling,             // the (first) hq is now toppling over
                        // (teams & scores shown in text area)
  bouncing,             // the hq has fallen & bounced at least once
                        // (teams & updated scores shown in text area)
  gameEnding            // a team has won, game is over
};


struct roundInfo
{ int   loserTeamNum,   // team whose hq was hit; -1 if no hq hit
        winnerTeamNum;  // team which hit ball before a goal was scored;
                        // -1 if no hq hit yet
  stateOfRound  state,  // state of game round
                prevState; // state of game round from last iteration
  long  cycles;         // # of iterations this round has been going
  int   timer;          // in seconds
};


/*=========================================================================*/
// BattleBall class.
// This is the main class for the game.  A single object of this class exists
// for the duration of the game.


class battleBall : public BattleBallGame
{
  char*         progName;
  int           numPlayers;             // starts w/ 0
  player        players[MAXPLAYERS];
  int           numTeams;               // starts w/ 0
  team          teams[MAXTEAMS];
  int           pointsToWin;            // points required to win game
  gobList       sceneryGobs;            // the "scenery" game objects, such
                                        // as trees, mntns, playfield, etc.
  coord         hqDist;                 // HQ-to-playfield-center distance
  bounGob       *bounds;                // "boundary" (playfield) game object
  ballGob       *ball;                  // ball game object
  hrznGob       *horizon;               // horizon game object
  tranGob       *train;                 // train game object; NULL if none
  int           intrinsicDelay;         // delay between each iteration (ms)
  int           startupDelay;           // delay before game starts (seconds)
  int           testIterations;         // =0: normal game play
                                        // >0: number of iterations to play
                                        //     before game automatically
                                        //     ends (used for testing)
  bool          doubleBuffer;           // double buffer game windows
  bool          keepInBounds;           // keep vhcls, etc. in  playfield
  bool          wantTrain;              // user wants the train in the game
  int           flybys;                 // 0->none, 1->few, 2->many
  gobList       track;                  // train track
  int           numTrees;               // number of tree game objects
  int           numMtns;                // number of mountain game objects
  roundInfo     roundinfo;              // state of the game

 public:
  battleBall(int argc, char *argv[]);
  ~battleBall();

  void  Play();

  void  DrawTextArea(const roundInfo& ri, player& dude);
  void  DrawStartingMsg(bbGfxTarget& gt, int startTime);

  virtual gob* createExplosion(const tcomp& np, bool sprk=true, const tcomp& nv=ang3d(MA_PI/8,MA_PI/8,0));
  virtual gob* createShell(const tcomp& np, const tcomp& nv, int tn, int *am);
  virtual gob* createBang(const tcomp& np, const tcomp& nv);
  virtual gob* createTank(const tcomp& np, const tcomp& nv, int tn);
  virtual wingGob* createPlane(const tcomp& np, const tcomp& nv, int tn);
  virtual wingGob* createSaucer(const tcomp& np, const tcomp& nv, int tn);
  virtual gob* createTrain(const tcomp& np, const tcomp& nv, int tn, gobList* gobs);
  virtual gob* createRail(const tcomp& np,coord nl);
  virtual bldgGob* createBuilding(const tcomp& np, const tcomp& nv, int tn);
  virtual ballGob* createBall(const tcomp& np, const tcomp& nv, int tn);
  virtual gob* transformToTank(const vhclGob& vhcl);
  virtual gob* morphToHelicopter(const tankGob& tank);
  virtual gob* morphToTank(const heliGob& heli);
  virtual gob* morphToHelicopter(const tank_heliGob& tankheli);
  virtual gob* morphToTank(const tank_heliGob& tankheli);
  virtual gob* morphToTurret(const turr_bladGob& turrblad, gob *par);
  virtual gob* morphToBlade(const turr_bladGob& turrblad, gob *par);

 private:
  void  ReadCmdLine(int argc, char *argv[]);
  void  ShowHelp();
  void  ShowBriefHelp();
  void  ActGobs(gobList& gobs);
  void  AddTeam(char *list);
  void  OpenAllDisplays(int argc, char *argv[]);
  void  InitForRound(gobList& gobs, int startTime, roundInfo& ri);
  void  InitTrack(gobList& gobs);
  void  InitScenery(gobList& gobs);
  void  ShowTeams(bbGfxTarget& gt);
  void  ShowStatus(bool);
  void  GetNextState(gobList& gobs, roundInfo& ri);
  void  DoFlyby(gobList& gobs);
  void  AutoPlay(gobList& gobs);
  void  HandleTestIterations();
  void  HandleGameTermination(bool& done, int& numActivePlayers);
  void  FrameDelay();
};

#endif // #ifndef BATTLEBALL_h

