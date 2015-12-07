// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


// BattleBall
// Developed 10/16/93 to 09/09/97
// by Philip A. Hardin



#include <stdio.h>       // to get sprintf()
#include <string.h>      // to get strncpy()
#include <time.h>        // to get time(time_t *)
#include "bb.h"
#include "battleball.h"
#include "terminal.h"
#include "terminals.h"
#include "display.h"
#include "surface.h"
#include "perspectiverenderer.h"
#include "hudrenderer.h"

Terminals terminals;

/*-------------------------------------------------------------------------*/
battleBall::battleBall(int argc, char *argv[])
  : progName(argv[0]),
    numPlayers(0),
    numTeams(0),
    pointsToWin(3),
    train(NULL),
    intrinsicDelay(30),
    startupDelay(0),
    testIterations(0),
    hqDist(HQDIST),
    doubleBuffer(true),
    keepInBounds(true),
    wantTrain(false),
    flybys(1),
    numTrees(TREES),
    numMtns(MTNS)
{
  srand(time(0));
  team::insigniaRandomizer= rand()%NUMINSIGNIA;
  ReadCmdLine(argc,argv);
  InitFixed();  // init sine & cosine lookup tables
  InitRegions(numTeams, gob::fancy, hqDist, team::insigniaRandomizer);
  OpenAllDisplays(argc,argv);
  InitScenery(sceneryGobs);
}


/*-------------------------------------------------------------------------*/
battleBall::~battleBall() {
  forii(numPlayers)
    if (players[i].active)
      players[i].CloseXStuff();
}


/*-------------------------------------------------------------------------*/
void battleBall::Play() {
  bool          done= false;

  // The following loop seems to be necessary for X windows
  // to not lose initial drawing commands.

  forii(numPlayers)
    if (players[i].active)
      if (players[i].gt.Pending() >0)
        players[i].gt.NextEvent();

  while (not done) {
    PlayOneRound(sceneryGobs,startupDelay,done);
    startupDelay= 0;
  }
}


/*-------------------------------------------------------------------------*/
/*                           private methods                               */
/*-------------------------------------------------------------------------*/

// Parse BattleBall's command line, setting game options and creating
// players and teams

void battleBall::ReadCmdLine(int argc, char *argv[]) {
  bool  helpRequested= false;

  for (int i=1; i <argc; i++) {
    char *s= argv[i];
    if (!strcmp(s,"-ff")) {
      if (i+1 <argc) {
	char c= *argv[++i];
        if (c=='b')                  gob::ffire= blocked; 
	else if (c=='d')             gob::ffire= dangerous; 
	else if (c=='t')             gob::ffire= transparent;
	else cout << "bad friendly fire option '" << c
		  << "':  use (b)locked, (d)angerous, or (t)ransparent"
		  << endl;
      }
    }
    else if (!strcmp(s,"-flyby"))   flybys= 2;
    else if (!strcmp(s,"-grav"))
    { if (i+1 <argc)                 gob::gravity= -atof(argv[++i]);
      if (gob::gravity >-0.01)       gob::gravity= -0.01;
      if (gob::gravity <-0.20)       gob::gravity= -0.20;
    }
    else if (!strcmp(s,"-ag")) {
      if (i+1 <argc) player::SetGunnerDefaults(argv[++i]);
    }
    else if (!strcmp(s,"-h"))         helpRequested= true;
    else if (!strcmp(s,"-help"))      helpRequested= true;
    else if (!strcmp(s,"-id"))
         { if (i+1 <argc)             intrinsicDelay= atoi(argv[++i]);}
    else if (!strcmp(s,"-mtns")) {
      if (i+1 <argc) numMtns= atoi(argv[++i]);
    }
    else if (!strcmp(s,"-noag"))      player::autoGunnerAllowed= false;
    else if (!strcmp(s,"-noap"))      player::autoPilotAllowed= false;
    else if (!strcmp(s,"-nobang"))    vhclGob::useBangs= false;
    else if (!strcmp(s,"-noflyby"))   flybys= 0;
    else if (!strcmp(s,"-nopause"))   player::pauseAllowed= false;
    else if (!strcmp(s,"-noresize"))  player::refitWindow= false;
    else if (!strcmp(s,"-noshade"))   player::shadowsOn= false;
    else if (!strcmp(s,"-out"))       keepInBounds= false;
    else if (!strcmp(s,"-pts"))
         { if (i+1 <argc)             pointsToWin= atoi(argv[++i]);}
    else if (!strcmp(s,"-rad"))
    { if (i+1 <argc)                  hqDist= atoi(argv[++i]);
      if (hqDist <MINHQDIST)          hqDist= MINHQDIST;
      if (hqDist >MAXHQDIST)          hqDist= MAXHQDIST;
    }
    else if (!strcmp(s,"-sb"))        doubleBuffer= false;
    else if (!strcmp(s,"-sd"))
        {if (i+1 <argc)               startupDelay= atoi(argv[++i]);}
    else if (!strcmp(s,"-simple"))    gob::fancy= false;
    else if (!strcmp(s,"-snum")) {
      if (i+1 <argc)                  player::shellsPerPlayer= atoi(argv[++i]);
    }
    else if (!strcmp(s,"-spow")) {
      if (i+1 <argc)                  shllGob::power= atof(argv[++i]);
    }
    else if (!strcmp(s,"-svel"))
    { if (i+1 <argc)                  vhclGob::muzzleVel= atof(argv[++i]);
      if (vhclGob::muzzleVel <0.5)    vhclGob::muzzleVel= 0.5;
      if (vhclGob::muzzleVel >5.0)    vhclGob::muzzleVel= 5.0;
    }
    else if (!strcmp(s,"-insig")) {
      if (i+1 <argc)
	team::insigniaRandomizer= atoi(argv[++i]);
    }
    else if (!strcmp(s,"-train"))     wantTrain= true;
    else if (!strcmp(s,"-trees")) {
      if (i+1 <argc) numTrees= atoi(argv[++i]);
    }
    else if (!strcmp(s,"-wf"))        gob::classSolid= false;
    else if (!strcmp(s,"-z")) {
      if (i+1 <argc) {               testIterations= atoi(argv[++i]);
                                     srand(testIterations);
      }
    }
    else if (s[0]=='-') cout << "Unrecognized flag: " << s << "\n";
    else if (numTeams <MAXTEAMS)
      AddTeam(argv[i]);
  }

  if (wantTrain)
    players[numPlayers++]= player("comp",-1,0); // teamNum==-1 -> train dude

  if (helpRequested) {
    ShowHelp();
    exit(1);
  }

  else if (numTeams==0) {
    ShowBriefHelp();
    exit(1);
  }
}


/*-------------------------------------------------------------------------*/
void battleBall::ShowBriefHelp() {
  cout << "Usage:  battleball [options...] XDisplay[,XDisplay] [XDisplay]...\n";
  cout << "Type battleball -help for more info.\n";
}


/*-------------------------------------------------------------------------*/
void battleBall::ShowHelp() {
  cout <<
"                                BattleBall 2.0\n"
"                      A 3-D multiplayer game for X11 Windows\n"
"                              by Philip A. Hardin    \n\n";

  cout <<
"SYNTAX\n\n"
"    battleball [options...] XDisplay[,XDisplay] [XDisplay]...\n\n"

"DESCRIPTION\n\n"
"    BattleBall is a 3-D multiplayer game for X11 Windows.  Teams of human\n"
"    and computer players use tanks and helicopters to play soccer on a\n"
"    large playfield.\n\n"

"OPTIONS\n\n"
"    -ag ##l     Set players' auto-gunner settings.  The argument of this\n"
"                option is three characters: a single-digit number\n"
"                specifying firing accuracy, a single-digit number\n"
"                specifying firing frequency, and one of the following\n"
"                letters specifying target selection:\n"
"                  a - fire at all targets\n"
"                  b - fire only at the ball\n"
"                  v - fire only at vehicles\n"
"                  n - no targets (i.e. do not fire)\n"
"                This option affects players which appear after it on the\n"
"                command line.  The default is 43a.\n"
"    -ff d|b|t   Make accidental 'friendly fire' from fellow teammates\n"
"                  d - dangerous (the default)\n"
"                  b - blocked\n"
"                  t - transparent\n"
"                If you dislike your teammates, use the 'dangerous' option.\n"
"    -grav #     Set gravity.  Defaults to 0.031 m/iteration^2.\n"
"    -help       Show this help screen\n"
"    -id #       Set the inter-frame delay.  If the game runs too slow, set\n"
"                this lower; if the game seems to lag behind your\n"
"                keystrokes, set this higher.  Defaults to 30 milliseconds.\n"
"    -mtns #     Set the number of mountains.  Defaults to 8.\n"
"    -noag       Disable human players' auto-gunner capability.\n"
"    -noap       Disable human players' auto-pilot capability.\n"
"    -nobang     Disable 'bangs' (the flashes at the end of a gun barrel)\n"
"    -noflyby    Disable aircraft fly-bys.\n"
"    -nopause    Disable players' use of the pause ('P') key.\n"
"    -noresize   Do not automatically resize the window to fit the graphics.\n"
"    -noshade    Disable shadows.  Uses less cpu time.\n"
"    -out        Allow vehicles to go outside of the playfield.\n"
"    -pts #      Set number of points required to win.  Defaults to 3.\n"
"    -rad #      Set the 'radius' of the playfield.  Defaults to 100 meters.\n"
"    -sb         Single-buffer the game windows.\n"
"    -sd #       Set the startup delay.  The game will wait for this many\n"
"                seconds for players to get ready.\n"
"    -simple     Use simpler graphics.  Uses less cpu time.\n"
"    -snum #     Set number of shells per player.  Defaults to 3.\n"
"    -spow #     Set shell power.  Defaults to 1.6.\n"
"    -svel #     Set shell muzzle velocity.  Defaults to 1.5 meters/frame.\n"
"                At higher velocities, some collisions may not be reliably\n"
"                detected.\n"
"    -train      Include a train running on a track around the playfield.\n"
"    -trees #    Set the number of trees.  Defaults to 12.\n"
"    -wf         Use wireframe rendering.  Uses less cpu time.\n\n"

"PLAYERS & TEAMS\n\n"
"    Human players are created by specifying X displays on the command line.\n"
"    Computer players are created by specifying 'comp' in place of the X\n"
"    displays.\n\n"

"    Commas between X displays put players on the same team; spaces between\n"
"    X displays separate teams.  Teams may have any mix of human and\n"
"    computer players.\n\n"

"    Computer players may be created with specific auto-gunner settings.\n"
"    Use 'comp', followed by (no space) the three characters used in the -ag\n"
"    option, e.g. 'comp67b'.  This overrides the -ag option.\n\n"

"EXAMPLES\n\n"
"    battleball mitre:0\n"
"                Creates one team which has one human player on the X display\n"
"                'mitre:0'.\n\n"

"    battleball flavio:0  chirp:0,roar:0\n"
"                Sets up a one-player team against a two-player team.\n\n"

"    battleball parrot:0,comp  raven:0  comp,comp\n"
"                Creates three teams with a mix of human and computer\n"
"                players.\n\n"
"    battleball dunce:0  comp75a  comp,comp\n"
"                Creates three teams.  The first computer player gets\n"
"                special auto-gunner settings; the other computer players\n"
"                get default settings.\n\n"

"WEB PAGE\n\n"
"    Source code and pre-compiled executables are available from the\n"
"    BattleBall web page at  http://www.cs.utexas.edu/users/pahardin/bb.html\n\n"
;
}


/*-------------------------------------------------------------------------*/
// Add a team of one or more players to the game
// (This includes adding the players to the game)

void battleBall::AddTeam(char *list) {
  int   numMembers= 0;

  do {
    int dispNameLen= strcspn(list,",");  // num chars before next comma
    char* newDispName= new char[dispNameLen+1];
    strncpy(newDispName,list,dispNameLen);
    newDispName[dispNameLen]= '\0';
    if (numPlayers <MAXPLAYERS) {
      players[numPlayers]= player(newDispName,numTeams,numMembers);
      if(players[numPlayers].computerPlayer == false)
      {
        player* aPlayer = &players[numPlayers];
        Terminal* aTerminal = terminals.create();
        aTerminal->thePlayer = aPlayer;
        aTerminal->m_Display->m_GfxTarget = &aPlayer->gt;
        aTerminal->train = &train;
        PerspectiveRenderer* aRenderer = new PerspectiveRenderer();
        aRenderer->doubleBuffer = &doubleBuffer;
        aRenderer->horizon = &horizon;
        aRenderer->thePlayer = aPlayer;
        aRenderer->playerIndex = numPlayers;
        aRenderer->teamIndex = numTeams;
        aRenderer->teams = teams;
        aTerminal->m_Display->m_Surface->m_ContentProvider = aRenderer;
        HudRenderer* aHud = new HudRenderer();
        aHud->thePlayer = aPlayer;
        aHud->ri = &roundinfo;
        aHud->bb = this;
        aTerminal->m_Display->m_Surface2->m_ContentProvider = aHud;
      }
      numMembers++;
      numPlayers++;
    }
    delete newDispName;

    list += dispNameLen +1;
  }
  while (list[-1] != ((char)('\0')));

  teams[numTeams]= team(numTeams);
  numTeams++;
}


/*-------------------------------------------------------------------------*/
// Tries to open the X displays of all the players.  If any of the displays
// can't be opened, reports the unopened ones, closes the opened ones, and
// terminates the program.

void battleBall::OpenAllDisplays(int argc, char *argv[]) {
  bool success= true;

  forii(numPlayers)
    if (not players[i].computerPlayer)
      if (players[i].gt.OpenDisplay(players[i].dispName, argc, argv))
      {
        players[i].gt.AllocColors(bbColorNames,totalColors);
      }
      else
      {
        success= false;
      }

  if (not success)
  { fori(numPlayers)
      if (not players[i].computerPlayer)
      {
        cout << "Closing " << players[i].dispName << "\n";
        players[i].gt.CloseDisplay();
      }
    exit (-1);
  }
}


/*-------------------------------------------------------------------------*/
// Show the teams and their players in the text subwindow

void battleBall::ShowTeams(bbGfxTarget& gt) {
  char  temp[80];

  gt.cursor= pt2d(0,0);
  gt.rightmost= 0;
  forii(numTeams) {
    gt << "       Score";
    sprintf(temp, "       %d", teams[i].score);
    gt << temp;
    gt.Box(0,0,6,2);
    gt.SetForeground(gt.Colors()[teams[i].colorNums[c_teamSecondary]]);
    Draw(&insignia2Rgns[i],gt,
	 pt2d(8+(gt.cursor.x+3)*gt.fontSize.x, gt.gfxSize.y+gt.fontSize.y),
	 0,pt2d(4,-4),true);
    gt.SetForeground(gt.Colors()[teams[i].colorNums[c_teamPrimary]]);
    Draw(&insigniaRgns[i],gt,
	 pt2d(8+(gt.cursor.x+3)*gt.fontSize.x, gt.gfxSize.y+gt.fontSize.y),
	 0,pt2d(4,-4),true);
    gt.SetForeground(gt.Black());
    forij(numPlayers) {
      player& dude= players[j];
      if (dude.TeamNum()==i)
	gt << dude.dispName;
    }
    gt.Tab(2);
  }
}


/*-------------------------------------------------------------------------*/
// Show the status display in all players' text subwindows if
// showingStatus is true; otherwise show the team list display in all
// players' text subwindows.

void battleBall::ShowStatus(bool showingStatus) {
  forii(numPlayers)
    players[i].textAreaShowingStatus= showingStatus;
}


/*-------------------------------------------------------------------------*/
// Draw the contents of the text subwindow

void battleBall::DrawTextArea(const roundInfo& ri, player& dude) {
  if (dude.textAreaShowingStatus)
    dude.DrawStatus(teams);
  else {
    if (ri.state != ri.prevState or dude.textAreaNeedsRedrawing) {
      dude.ClearTextArea();
      ShowTeams(dude.gt);
      dude.textAreaNeedsRedrawing= false;
    }
  }
}


/*-------------------------------------------------------------------------*/
// Create the game objects for the train track

void battleBall::InitTrack(gobList& gobs) {
  double trackData[]= {
    40,0,0, 40,0,0.063, 100,0,0, 40,0,-0.063, 40,0,0,
    40,0.166,0, 40,0.333,0, 40,0.5,0, 40,0.666,0, 40,0.833,0,
    259.5,1,0,
    40,1.166,0, 40,1.333,0, 40,1.5,0, 40,1.666,0, 40,1.833,0,
    0
  };
    
  pt3d pos= pt3d(-129.2,0,0);
  double *data= trackData;
  while (*data != 0) {
    double len= *data++;
    ang3d ang= ang3d((int)(data[0]*MA_PI), (int)(data[1]*MA_PI), 0);
    data += 2;
    railGob *rail= new railGob(tcomp(pos,ang),len);
    track.push_back(rail);
    gobs.push_back(rail);
    pos += pt3d(len,0,0) >> ang;
    if (pos.z <0.001) pos.z= 0;
  }

  gobs.push_back(new pllrGob(pt3d(-46.4,0,0)));
  gobs.push_back(new pllrGob(pt3d( 46.4,0,0)));
}


/*-------------------------------------------------------------------------*/
// Create the "scenery" game objects which only need to be created once per
// game, as opposed to once per game round.

void battleBall::InitScenery(gobList& gobs) {
  tcomp pos;
  bool  blocked;
  gobList::iterator gi;

  gobs.push_back(bounds= new bounGob(tcomp(pt3d(0,0)),keepInBounds));
//  gobs.push_back(new dstrGob(pt3d(-3*hqDist/2,2*hqDist,0),
//                             pt3d(1./8,0)));

  if (wantTrain) InitTrack(gobs);
		 
  forii(numMtns +numTrees) {
    do
    { if (i <numMtns)
        pos.Cart()= pt3d((rand()%256)*(HQDIST/16) +4*HQDIST, 0, 0);
      else
      {
#ifdef WIN32
        int tmpRandomX = rand();
        double randomX = (tmpRandomX % 100) / 100.0;
        pos.Cart()= pt3d(randomX*2*hqDist, 0, 0);
#else
        pos.Cart()= pt3d(drand48()*2*hqDist, 0, 0);
#endif
      }
      pos.Cart() >>= rand()%(2*MA_PI);

      blocked= false;
      for_(gi,gobs)
        if ((**gi).Contains(pos.Cart()))
          blocked= true;
    }
    while (blocked);

    pos.Ang()= rand()%(MA_PI/2);
    if (i <numMtns)
      gobs.push_back(new mntnGob(pos));
    else
      gobs.push_back(new treeGob(pos));
  }

  horizon= new hrznGob(pt3d(0,0));
  gobs.push_back(horizon);
}


/*-------------------------------------------------------------------------*/
// Initialize the game for a new game round

void battleBall::InitForRound(gobList& gobs, int startTime, roundInfo& ri) {
  gob::gobs= &gobs;

  forii(numTeams)
    teams[i].InitForRound(gobs,hqDist,numTeams);

  fori(numPlayers) {
    players[i].InitForRound(gobs,hqDist,numTeams,&track);
    if (testIterations >0) {
      players[i].autoPilot=  player::autoPilotAllowed;
      players[i].autoGunner= player::autoGunnerAllowed;
    }
    if (players[i].TeamNum()==-1)
      train= (tranGob*) players[i].Vhcl();
  }

  ball= new ballGob(pt3d(0,0),pt3d(0,0),-1);
  gobs.push_back(ball);

  ri.loserTeamNum= -1;
  ri.cycles= 0;
  if (startTime) {
    ri.timer= startTime +1;
    ri.state= ri.prevState= counting;
    ShowStatus(false);
  } else {
    ri.timer= 0;
    ri.state= ri.prevState= playing;
  }
}


/*-------------------------------------------------------------------------*/
// Free things when a game round ends

void battleBall::FreeRound(gobList& gobs) {
//  gobList::iterator gi;

  // free the gobs which were created during the play of this round
  // (i.e. _don't_ free gobs which were created by InitScenery() )
  /* -PAH
  for_(gi,gobs)
    if ((**gi).type != boun and (**gi).type != tree and
        (**gi).type != mntn and (**gi).type != hrzn)
      delete *gi;
      */
}


/*-------------------------------------------------------------------------*/
// Draw a message while the startup delay counts down

void battleBall::DrawStartingMsg(bbGfxTarget& gt, int startTime)
{ char  msg[60];
  sprintf(msg," Game starts in %d seconds ",startTime);
  gt.DrawImageString((int)(29*gt.fontSize.x), (int)(2*gt.fontSize.y), msg,strlen(msg));
  gt.DrawRectangle((int)(29*gt.fontSize.x -1), (int)gt.fontSize.y +2, (int)(strlen(msg)*gt.fontSize.x+1), (int)gt.fontSize.y);
}


/*-------------------------------------------------------------------------*/
// Advance the game round to its next state when appropriate

void battleBall::GetNextState(gobList& gobs, roundInfo& ri)
{ int   i;
  bldgGob *hq=NULL;

  if (not player::paused)
  {

  ri.prevState= ri.state;
  if (ri.loserTeamNum != -1)
    hq= teams[ri.loserTeamNum].hq;
  if (testIterations >0)
    testIterations--;
  (ri.cycles)++;

  switch (ri.state) {
    case counting:
      if (ri.timer >0) ri.timer--;
      if (ri.timer==0) {
	ri.state= playing;
	ShowStatus(true);
      }
      break;

    case playing:
      for(i= 0; i <numTeams; i++)
        if (teams[i].hq->vel.Ang().xz != 0) {
	  ri.loserTeamNum= i;
          ri.winnerTeamNum= ball->teamNum;
          ri.state= toppling;
	  ShowStatus(false);
          break;
        }
      break;
      
    case toppling:
      if (hq->pos.Ang().xz != -MA_PI/2)
        hq->Topple();
      else {
        for(i= 0; i <numTeams; i++)
          if (i != ri.loserTeamNum)
            teams[i].score++;
        if (numTeams >=3 and
            ri.winnerTeamNum != -1 and
            ri.winnerTeamNum != ri.loserTeamNum)
          teams[ri.winnerTeamNum].score++;
        ri.state= bouncing;
      }
      break;

    case bouncing:
      if (hq->Altitude()==0 and hq->vel.Cart().z==0)
      { ri.state= roundEnding;
        for(i= 0; i <numTeams; i++)
          if (teams[i].score >= pointsToWin)
            ri.state= gameEnding;
      }
      break;

    case roundEnding:
    case gameEnding:
      break;
  }

  }
}


/*-------------------------------------------------------------------------*/
// Make each game object "act", i.e. make it do whatever it does, for this
// iteration.  Depending on the game object and its state, "acting" can
// include morphing, setting its velocity, moving, colliding with other
// game objects, etc.

void battleBall::ActGobs(gobList& gobs) {
  gobList::iterator gi= gobs.begin();
  gobList::iterator nxtgob= gi;
  int numGobs= gobs.size();

  while (numGobs-- >0) {
    nxtgob++;
    gobs.Update((**gi).Act(),gi);
    gi=nxtgob;
  }
}


/*-------------------------------------------------------------------------*/
// Create an airplane or saucer ship game object to do a fly-by over the
// playfield.

void battleBall::DoFlyby(gobList& gobs)
{
  if ( (not player::paused) and
       (roundinfo.cycles %512)==0 and
        vhclGob::testVhcl==NULL and
        (flybys==2 or (flybys==1 and (rand() & 0x700)==0x700)) )
  {
    bool isSaucer= rand()%2;
    tcomp pos(rand()%(2*MA_PI));
    tcomp vel= isSaucer ? pt3d(1.5,0) : pt3d(1,0);
    pos.Cart()= pt3d(-384*vel.Cart().x,0,24) >> pos.Ang();

    wingGob* g;
    if (isSaucer)
      g= new saucGob(pos,vel,-1);  // dummy team number - does it matter? -PAH
    else
      g= new plneGob(pos,vel,-1);

    g->animDir= 1;
    g->ctrl= vel;
    gobs.push_back(g);
  }
}


/*-------------------------------------------------------------------------*/
// Process auto-pilots and auto-gunners

void battleBall::AutoPlay(gobList& gobs)
{
  forii(numPlayers)
  {
    if (not player::paused and roundinfo.state != counting)
    {
      players[i].AutoPlay(gobs,numTeams,teams,*ball);
    }
  }
}

/*-------------------------------------------------------------------------*/
// Play one full game round.
// Out: done = true if no human players are playing any longer

void battleBall::PlayOneRound(const gobList& sceneryGobs, int startTime,
                              bool& done) {
  gobList       gobs;
  int           numActivePlayers= numPlayers;

  gobs= sceneryGobs;
  InitForRound(gobs,startTime,roundinfo);

  while (roundinfo.state != roundEnding and numActivePlayers >0)
  {
    terminals.processInput();

    GetNextState(gobs,roundinfo);

    DoFlyby(gobs);

    AutoPlay(gobs);

    forii(numPlayers) {
      if (testIterations==1)
        if (players[i].active)
          players[i].CloseXStuff();     // also sets active= false
    }

    if (not player::paused) ActGobs(gobs);

    terminals.processOutput();

    numActivePlayers= 0;
    fori(numPlayers) {
      player& dude= players[i];
      if (dude.active) {
        numActivePlayers++;
      }
    }

    if (roundinfo.state==counting)
      SleepFor(990*1000);
    else if (intrinsicDelay >0)
      SleepFor((intrinsicDelay)*1000);
  }

  done= (numActivePlayers==0);
  FreeRound(gobs);
}

