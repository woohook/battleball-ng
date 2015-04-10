// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include <ctype.h>       // to get tolower()
#include <cstring>       // to get strncpy()
#include <climits>       // to get INT_MAX, LONG_MAX
#include <X11/keysym.h>  // to get XK_*
#include <algorithm>     // ugh, this baby's big
#include "player.h"

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


/*-------------------------------------------------------------------------*/
// In:  newDispName = X display name for a human player, or
//                    "comp" or "comp..." for a computer player

player::player(const char *newDispName, int newTeamNum, int newMemberNum) 
  : teamNum(newTeamNum),
    memberNum(newMemberNum),
    gunnerAccuracy(defaultAccuracy),
    gunnerFrequency(defaultFrequency),
    mayShootBall(mayShootBallDefault),
    mayShootVhcl(mayShootVhclDefault),
    train(NULL)
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

void player::InitForRound(gobList& gobs, coord hqDist, int numTeams,
			  gobList* track) {
  if (teamNum != -1) {
    homePos.Ang()= -MA_PI/2 + (teamNum * 2*MA_PI)/numTeams;
    homePos.Cart()= pt3d(hqDist-2*HOMESPACING, (1+memberNum)*HOMESPACING);
    if (memberNum%2 == 0)
      homePos.Cart().y= -homePos.Cart().y -HOMESPACING;
    homePos.Cart() >>= homePos.Ang();
    homePos.Ang() += MA_PI;
    
    vhcl= gobs.insert(gobs.end(),new tankGob(homePos,pt3d(0,0),teamNum));

  } else {

    homePos= pt3d(0,0,0);
    vhcl= gobs.insert(gobs.end(),
		      new tranGob(homePos,pt3d(0,0,0),teamNum,track));
  }

  usingHqView= false;
  ammo= shellsPerPlayer;
  textAreaNeedsRedrawing= true;
  textAreaShowingStatus= true;
}


/*-------------------------------------------------------------------------*/
// Draw the 3-D view for this player.

void player::DrawView(gobList& gobs, team teams[], int playerNum,
                      bool doubleBuff, hrznGob *horizon) {

  tmtrx viewPos= Vhcl()->ViewPos(); // both cartesian & angular position
  pt3d eyePos= viewPos.Cart();      // just cartesian position

  if (usingHqView) {
    eyePos= teams[teamNum].hq->ViewPos().Cart();
    pt3d vhclCenter= pt3d(0,0,Vhcl()->Shape()->Box().Center().z) >> Vhcl()->WorldPos();
    viewPos= tcomp(eyePos,(vhclCenter -eyePos).Ang3d());
  }

  pt3d  frustrumNormals[4];
  frustrumNormals[0]= (pt3d(1, 2, 0) >> viewPos) -viewPos.Cart();
  frustrumNormals[1]= (pt3d(1,-2, 0) >> viewPos) -viewPos.Cart();
  frustrumNormals[2]= (pt3d(1, 0, 4) >> viewPos) -viewPos.Cart();
  frustrumNormals[3]= (pt3d(1, 0,-4) >> viewPos) -viewPos.Cart();
  frustrumNormals[0].Normalize();
  frustrumNormals[1].Normalize();
  frustrumNormals[2].Normalize();
  frustrumNormals[3].Normalize();

  // compute the world-to-view transformation
  tmtrx worldToView= viewPos.Inverse();
  // scale worldToView to do some of the 3D->2D screen projection
  forii(4)
    for (int j= 1; j <=2; j++)
      worldToView[i][j] *= (-gt.gfxSize.x);

  if (doubleBuff) gt.DoubleBufferBegin();

  if (not horizon->Solid()) {
    gt.SetForeground(gt.White());
    gt.FillRectangle(pt2d(0,0),gt.gfxSize);
    gt.SetForeground(gt.Black());
  }

  horizon->SetWorldPos(viewPos);

  // sort all the non-part gobs by distance from viewer
  gobList::iterator gi;
  vector<gobRef> refs;
  for_(gi,gobs)
    refs.push_back(gobRef(*gi,(**gi).DistFromViewer(eyePos)));
  stable_sort(refs.begin(),refs.end());

  // now insert gob parts into the list, in correct drawing order
  gobList allGobs;
  vector<gobRef>::iterator gr;
  for_(gr,refs)
    (**gr).GetViewOrder(eyePos,allGobs);

  // draw the horizon, then the playfield (they're the last two gobRefs)
  gi= allGobs.end();
  (**(--gi)).Draw(gt,worldToView,playerNum,eyePos); // horizon
  (**(--gi)).Draw(gt,worldToView,playerNum,eyePos); // playfield

  // Next, draw any shadows
  ulong tempColors[totalColors];

  if (shadowsOn) {
    fori(totalColors)
      tempColors[i]= gt.Colors()[c_gray20];
    
    gobList::iterator savedGi= gi;
    while (gi != allGobs.begin()) {
      gi--;
      rgn3 *shadow= (**gi).Shadow();
      if (shadow) {
	// project flattened shadow onto the ground!
	tmtrx pos= (**gi).WorldPos();
	pos[0][2] *= 1e-6;   // I'm worried about making a "singular" mtrx
	pos[1][2] *= 1e-6;   // here, so I don't quite zero this column...
	pos[2][2] *= 1e-6;
	pos[3][2]= 0;
	//pos.UnitScaling()= false;
	if (shadow->InFrustrum(eyePos,frustrumNormals,pos)) {
	  view v;
	  v.Update(shadow,pos*worldToView);
	  gt.SetForeground(gt.Colors()[c_gray20]);
	  shadow->Draw(v.pts, gt, eyePos<<pos, gt.gfxSize, //eyePos<<pos -PAH
		       (**gi).Solid(), tempColors);
	}
      }
    }
    gi= savedGi;
  }

  // Finally, draw the gobs themselves

  Vhcl()->EyeIsInside(not usingHqView);

  while (gi != allGobs.begin()) {
    gi--;
    if ((**gi).Shape()->InFrustrum(eyePos,frustrumNormals,(**gi).WorldPos())) {
      ulong *colors= gt.Colors();
      if ((**gi).TeamNum() != -1) {
	colors= tempColors;
	fori(totalColors)
	  tempColors[i]= gt.Colors()[teams[(**gi).TeamNum()].colorNums[i]];
      }

      (**gi).Draw(gt,worldToView,playerNum,eyePos,colors);
    }
  }
  
  Vhcl()->EyeIsInside(false);
}


/*-------------------------------------------------------------------------*/
void player::CloseXStuff()
{ gt.UnloadFont();
  gt.FreePixmap();
  gt.FreeGC();
  gt.CloseDisplay();
  active= false;
}


/*-------------------------------------------------------------------------*/
// Handle a key press event.

void player::HandleKeyPress(KeySym k, bool pressed, gobList& gobs,
			    tranGob* bbTrain) {
//  gobList::iterator gi;
  switch(k) {
    case XK_a:                  // toggle player/autonomous pilot
      if (pressed and autoPilotAllowed) {
        autoPilot= not autoPilot;
	textAreaNeedsRedrawing= true;
        if (not autoPilot)
          Vhcl()->ctrl= pt3d(0,0);
      }
      break;
    case XK_g:                  // toggle player/autonomous gunner
      if (pressed and autoGunnerAllowed) {
        autoGunner= not autoGunner;
	textAreaNeedsRedrawing= true;
      }
      break;
    case XK_p:
      if (pressed and pauseAllowed) {
	paused= not paused;
	textAreaNeedsRedrawing= true;
      }
      break;
    case XK_q:
    case XK_Escape:
      if (!pressed) CloseXStuff();
      break;
    case XK_v:
      if (pressed) {
	usingHqView= not usingHqView;
	textAreaNeedsRedrawing= true;
      }
      break;
    case XK_Insert:
      if (pressed) {
	if (train) {
	  train->teamNum= -1;
	  train= NULL;
	} else {
	  if (bbTrain and bbTrain->TeamNum()==-1) {
	    train= bbTrain;
	    train->teamNum= teamNum;
	  }
	}
      }
      break;
    case XK_Tab:
      if (pressed) {
	textAreaShowingStatus= not textAreaShowingStatus;
	textAreaNeedsRedrawing= true;
      }
      break;
    default:
      if (not paused) {
	if (train==NULL)
	  gobs.Update(Vhcl()->Control(k,pressed,&ammo),vhcl);
	else
	  Vhcl()->Control(k,pressed,&ammo);
      }
      break;
  }
}


/*-------------------------------------------------------------------------*/
// Handle an operating system event for this player.

void player::HandleEvents(gobList& gobs, tranGob* bbTrain) {
  XEvent event;
  KeySym k;

  while (active and gt.Pending() >0)
  { gt.NextEvent(&event);
    switch (event.type) {
      case Expose:
        textAreaNeedsRedrawing= true;
        break;
      case KeyPress:
        k= gt.LookupKeysym((XKeyEvent *)(&event));
        HandleKeyPress(k,true,gobs,bbTrain);
        break;
      case KeyRelease:
        k= gt.LookupKeysym((XKeyEvent *)(&event));
        HandleKeyPress(k,false,gobs,bbTrain);
        break;
      case ConfigureNotify:
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

  // If this is the train player, but a real player currently has control of
  // the train, then don't do anything
  if (teamNum==-1 and Vhcl()->TeamNum() != teamNum)
    return;

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


/*-------------------------------------------------------------------------*/
// Clear the text subwindow.

void player::ClearTextArea() {
  gt.SetForeground(gt.White());
  gt.FillRectangle(pt2d(0,gt.gfxSize.y),gt.textSize);
  gt.SetForeground(gt.Black());
  gt.DrawRectangle(pt2d(0,gt.gfxSize.y),gt.textSize -pt2d(1,1));
}
