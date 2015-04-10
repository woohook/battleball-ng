// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.

#ifndef BATTLEBALL_h
#define BATTLEBALL_h

#include "player.h"

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
  counting,             // timer is counting down to begin playing
                        // (teams shown in text area)
  playing,              // playing the round; no goal scored yet
                        // (game status shown in text area)
  toppling,             // the (first) hq is now toppling over
                        // (teams & scores shown in text area)
  bouncing,             // the hq has fallen & bounced at least once
                        // (teams & updated scores shown in text area)
  roundEnding,          // the round is over (but _not_ the game)
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


class battleBall {
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


 public:
  battleBall(int argc, char *argv[]);
  ~battleBall();

  void  Play();

 private:
  void  ReadCmdLine(int argc, char *argv[]);
  void  ShowHelp();
  void  ShowBriefHelp();
  void  ActGobs(gobList& gobs);
  void  AddTeam(char *list);
  void  OpenAllDisplays(int argc, char *argv[]);
  void  PlayOneRound(const gobList& sceneryGobs, int startTime, bool& done);
  void  InitForRound(gobList& gobs, int startTime, roundInfo& ri);
  void  FreeRound(gobList& gobs);
  void  InitTrack(gobList& gobs);
  void  InitScenery(gobList& gobs);
  void  ShowTeams(bbGfxTarget& gt);
  void  ShowStatus(bool);
  void  DrawTextArea(const roundInfo& ri, player& dude);
  void  DrawStartingMsg(bbGfxTarget& gt, int startTime);
  void  GetNextState(gobList& gobs, roundInfo& ri);
  void  DoFlyby(gobList& gobs);
};

#endif // #ifndef BATTLEBALL_h
