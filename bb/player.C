// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include <ctype.h>       // to get tolower()
#include <cstring>       // to get strncpy()
#include <climits>       // to get INT_MAX, LONG_MAX
#include <algorithm>     // ugh, this baby's big
#include "player.h"
#include "battleballgame.h"
#include "train.h"

using namespace std;

#ifdef __GNUC__
char __stl_temp_buffer[16384]; //__stl_buffer_size does not exist in egcs.
#else
#include "../stl/tempbuf.cpp"    // quasi-hack necessary to use STL's stable_sort()
#endif


const coord
  COORD_MAX= LONG_MAX,
  HOMESPACING= 8;               // spacing 'twixt home positions


bool player::autoPilotAllowed= true;
bool player::autoGunnerAllowed= true;
bool player::pauseAllowed= true;
bool player::paused= false;
bool player::shadowsOn= true;
bool player::mayShootBallDefault= true;
bool player::mayShootVhclDefault= true;
int  player::defaultAccuracy= 4;
int  player::defaultFrequency= 3;
bool player::refitWindow= true;
int  player::shellsPerPlayer= 3;

player* g_trainPlayer = NULL;

/*-------------------------------------------------------------------------*/
// In:  newDispName = X display name for a human player, or
//                    "comp" or "comp..." for a computer player

player::player(const char *newDispName, int newTeamNum, int newMemberNum) 
  : teamNum(newTeamNum),
    memberNum(newMemberNum),
    gunnerAccuracy(defaultAccuracy),
    gunnerFrequency(defaultFrequency),
    mayShootBall(mayShootBallDefault),
    mayShootVhcl(mayShootVhclDefault)
{
  int len= strlen(newDispName);

  if (len >MAXDISPCHARS) len= MAXDISPCHARS;
  strncpy(dispName,newDispName,len);
  dispName[len]= '\0';

  if (strncmp(dispName,"comp",4)==0 and len <=7) {
    computerPlayer= true;
    autoPilot= autoGunner= true;
    active= false;

    char *s= dispName +4;
    if (*s >='0' and *s <='9')
      gunnerAccuracy= *(s++) - '0';
    if (*s >='0' and *s <='9')
      gunnerFrequency= *(s++) - '0';
    
    if (*s) {
      char c= tolower(*s);
      mayShootBall= (c=='b' or c=='a');
      mayShootVhcl= (c=='v' or c=='a');
      if (c != 'b' and c != 'v' and c != 'a' and c != 'n')
	cerr << "bad auto gunner option '" << c << "':  use (a)ll, (b)all, (v)ehicle, or (n)one" << endl;
    }

  } else {

    computerPlayer= false;
    autoPilot= autoGunner= false;
    active= true;
  }
}


/*------------------------------------------------------------------------*/
// Set the defaults for the auto-gunner.  When players are created, their
// auto-gunners get whatever the current defaults are.

void player::SetGunnerDefaults(const char* s) {
  if (*s >='0' and *s <='9')
    defaultAccuracy= *(s++) - '0';
  if (*s >='0' and *s <='9')
    defaultFrequency= *(s++) - '0';

  if (*s) {
    char c= tolower(*s);
    mayShootBallDefault= (c=='b' or c=='a');
    mayShootVhclDefault= (c=='v' or c=='a');
    if (c != 'b' and c != 'v' and c != 'a' and c != 'n')
      cerr << "bad auto gunner option '" << c << "':  use (a)ll, (b)all, (v)ehicle, or (n)one" << endl;
  }
}


/*-------------------------------------------------------------------------*/
// Init this player for the start of a new game round.

void player::InitForRound(gobList& gobs, coord hqDist, int numTeams)
{
  if (teamNum != -1) {
    homePos.Ang()= -MA_PI/2 + (teamNum * 2*MA_PI)/numTeams;
    homePos.Cart()= pt3d(hqDist-2*HOMESPACING, (1+memberNum)*HOMESPACING);
    if (memberNum%2 == 0)
      homePos.Cart().y= -homePos.Cart().y -HOMESPACING;
    homePos.Cart() >>= homePos.Ang();
    homePos.Ang() += MA_PI;

    vhcl= gobs.insert(gobs.end(), g_BattleBallGame->createTank(homePos,pt3d(0,0),teamNum));

  }

  usingHqView= false;
  ammo= shellsPerPlayer;
  textAreaNeedsRedrawing= true;
  textAreaShowingStatus= true;
}


/*-------------------------------------------------------------------------*/
void player::CloseXStuff()
{
  gt.CloseDisplay();
  active= false;
}


/*-------------------------------------------------------------------------*/
// Handle a key press event.

void player::HandleKeyPress(KB_Key k, bool pressed, gobList& gobs)
{
//  gobList::iterator gi;
  switch(k) {
    case KBK_a:                  // toggle player/autonomous pilot
      if (pressed and autoPilotAllowed) {
        autoPilot= not autoPilot;
	textAreaNeedsRedrawing= true;
        if (not autoPilot)
          Vhcl()->ctrl= pt3d(0,0);
      }
      break;
    case KBK_g:                  // toggle player/autonomous gunner
      if (pressed and autoGunnerAllowed) {
        autoGunner= not autoGunner;
	textAreaNeedsRedrawing= true;
      }
      break;
    case KBK_p:
      if (pressed and pauseAllowed) {
	paused= not paused;
	textAreaNeedsRedrawing= true;
      }
      break;
    case KBK_q:
    case KBK_Escape:
      if (!pressed) CloseXStuff();
      break;
    case KBK_v:
      if (pressed) {
	usingHqView= not usingHqView;
	textAreaNeedsRedrawing= true;
      }
      break;
    case KBK_Insert:
      if (pressed) {
	if (g_trainPlayer==this)
        {
          g_trainPlayer = NULL;
	}
        else
        {
	  if( (g_train != NULL) && (g_trainPlayer==NULL) )
          {
            g_trainPlayer = this;
	  }
	}
      }
      break;
    case KBK_Tab:
      if (pressed) {
	textAreaShowingStatus= not textAreaShowingStatus;
	textAreaNeedsRedrawing= true;
      }
      break;
    default:
      if (not paused) {
	if (g_trainPlayer != this)
	  gobs.Update(Vhcl()->Control(k,pressed,&ammo),vhcl);
	else
	  Vhcl()->Control(k,pressed,&ammo);
      }
      break;
  }
}


/*-------------------------------------------------------------------------*/
// Handle an operating system event for this player.

void player::HandleEvents(gobList& gobs)
{
  while (active and gt.Pending() >0)
  {
    BBEvent event = gt.NextEvent();
    switch (event.type) {
      case BBE_Expose:
        textAreaNeedsRedrawing= true;
        break;
      case BBE_Key:
        HandleKeyPress(event.key,event.pressed,gobs);
        break;
      case BBE_Resize:
        gt.HandleResize(&event,refitWindow);
        break;
    }
  }
}


/*-------------------------------------------------------------------------*/
// Return a ptr to the team that this player wants to attack.

team* player::TeamToAttack(int numTeams,team teams[],const gob& ball) {
  coord teamHqDist= COORD_MAX;

  team *teamToAttack= NULL;
  forii(numTeams)
    if (i != teamNum and
        (teamToAttack==NULL or
         teams[i].score > teamToAttack->score or
         (teams[i].score==teamToAttack->score and
          (ball.pos.Cart() -teams[i].hq->pos.Cart()).FastDist() <teamHqDist)))
    {
      teamToAttack= &teams[i];
      teamHqDist= (ball.pos.Cart() -teams[i].hq->pos.Cart()).FastDist();
    }

  return teamToAttack ? teamToAttack : &teams[0];
}


/*-------------------------------------------------------------------------*/
// Decide the player's current destination, depending on which team he's
// trying to attack and where the ball is.

void player::GetDest(const team& teamToGet, const gob& ball,
		     pt2d& dest, pt2d& ballToHq) {
  pt2d  ballPos,
        hqPos,
        vhclPos,
        vhclToBall;
  coord ballToDestDist;

  ballPos= (pt2d)(ball.pos.Cart());
  hqPos=   (pt2d)(teamToGet.hq->pos.Cart());
  vhclPos= (pt2d)(Vhcl()->pos.Cart());

  vhclToBall= ballPos -vhclPos;
  ballToDestDist= vhclToBall.FastDist()*0.68;
  if (ballToDestDist >16)
    ballToDestDist= 16;
  ballToHq= (hqPos -ballPos).Normalized();
  if (vhclToBall.Dot(ballToHq) >0)
    dest= ballPos -ballToHq*ballToDestDist;
  else {
    pt2d toLeft= vhclToBall.Normalized(ballToDestDist).Left90();
    if (toLeft.Dot(ballToHq) <0)
      dest= ballPos +toLeft;
    else
      dest= ballPos -toLeft;
  }
}


/*-------------------------------------------------------------------------*/
// Control or partially control the player's vehicle using the auto-pilot
// and/or the auto-gunner.

void player::AutoPlay(gobList& gobs,int numTeams,team teams[],gob& ball) {
  team *teamToAttack= TeamToAttack(numTeams,teams,ball);

  gob *target= NULL;
  if (autoGunner) {
    if (mayShootBall and Vhcl()->CanMakeGoal(*(teamToAttack->hq),ball))
      target= &ball;
    else if (mayShootVhcl)
      target= Vhcl()->PreferredVhclTarget(ball.WorldPos().Cart());
  }

  if (autoPilot) {
    pt3d dest;
    pt3d ballToHq;
    GetDest(*teamToAttack,ball,dest,ballToHq);
    dest.z= ball.Center().z*0.7;
    ballToHq.z= 0;

    // if we have a tgt, and if this vhcl must turn itself in order to aim,
    if (target and Vhcl()->Shape()== &heliRgn) {

      // if the dest is far away and the target is almost in line with it,
      pt3d selfToTgt= target->pos.Cart() - Vhcl()->pos.Cart();
      pt3d selfToDest= dest - Vhcl()->pos.Cart();
      if (selfToDest.Dist() >25 and
	  selfToDest.Normalized().Dot(selfToTgt.Normalized()) >0.95) {
	
	// then the target becomes the destination
	dest= target->pos.Cart();
	ballToHq= pt3d(0,0,0);
      }
    }
    
    tcomp selfToDest;
    Vhcl()->Control(dest,ballToHq,selfToDest);
  }

  // The auto gunner may override some of the auto pilot's decisions,
  // so the auto gunner needs to be invoked after the auto pilot.
  if (autoGunner)
    Vhcl()->Control(&ammo,target,gunnerAccuracy,gunnerFrequency);
}


/*-------------------------------------------------------------------------*/
// Draw this player's status in the text subwindow.

void player::DrawStatus(const team teams[]) {
  const team& t= teams[TeamNum()];

  if (Vhcl()->newlyCreated) {
    textAreaNeedsRedrawing= true;
    Vhcl()->newlyCreated= false;
  }
  if (textAreaNeedsRedrawing)
    ClearTextArea();

  // Draw Status
  gt.cursor= pt2d(0,0);
  gt.rightmost= 0;
  Vhcl()->DrawStatus(gt,t.colorNums[c_teamPrimary],
		     t.colorNums[c_teamSecondary],textAreaNeedsRedrawing);

  if (textAreaNeedsRedrawing) {
    // Draw Keyboard Controls
    gt.DrawLine(182, (int)(gt.gfxSize.y), 182, (int)(gt.gfxSize.y+gt.textSize.y-1));
    pt2d temp(gt.rightmost+4,0);
    gt.cursor= temp;
    gt << "     Keyboard Controls";
    gt.rightmost= (int)temp.x;
    Vhcl()->DrawControls(gt);
    gt.Tab(1,3);
    gt << " Q     V   ";
    gt << "quit   HQ  ";
    gt << "game  view ";
    gt.Box(1,3).Box(7,3,1,1,usingHqView);
    if (autoPilotAllowed) {
      gt.Tab(1,3);
      gt << "  A";
      gt << "auto";
      gt << "pilot";
      gt.Box(2,3,1,1,autoPilot);
    }
    if (autoGunnerAllowed) {
      gt.Tab(1,3);
      gt << "  G";
      gt << " auto";
      gt << "gunner";
      gt.Box(2,3,1,1,autoGunner);
    }
    if (pauseAllowed) {
      gt.Tab(1,3);
      gt << "  P";
      gt << "pause";
      gt.Box(2,3,1,1,paused);
    }
    gt.Tab(1,3);
    gt << " Tab";
    gt << "teams";
    gt.Box(1,3,3);

    bool someIsHidden=
      gt.cursorOrg.x +gt.rightmost*gt.fontSize.x >gt.textSize.x;

    gt.Tab(3,1);
    gt << " BattleBall 2.0";
    gt << "      by";
    gt << "Philip A. Hardin";
    //gt << "www.cs.utexas.edu/users/pahardin/bb.html";

    if (someIsHidden) {
      gt.cursor= pt2d((gt.textSize.x -gt.cursorOrg.x)/gt.fontSize.x -6.5,0);
      gt << "more->";
    }
  }

  textAreaNeedsRedrawing= false;
}

vhclGob* player::Vhcl() const
{
  return (g_trainPlayer == this) ? g_train : (vhclGob*) *vhcl;
}

/*-------------------------------------------------------------------------*/
// Clear the text subwindow.

void player::ClearTextArea() {
  gt.SetForeground(gt.White());
  gt.FillRectangle(pt2d(0,gt.gfxSize.y),gt.textSize);
  gt.SetForeground(gt.Black());
  gt.DrawRectangle(pt2d(0,gt.gfxSize.y),gt.textSize -pt2d(1,1));
}
