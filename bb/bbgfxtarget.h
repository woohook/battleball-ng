// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef BBGFXTARGET_h
#define BBGFXTARGET_h


#include "general.h"
#include "pt2d.h"
#include "bbcolor.h"
#ifdef WIN32
#include "gfxtargetwin.h"
#else
#include "gfxtargetx11.h"
#endif

/*======================================================================*/
/* Graphics Target class.
   The "graphics target" is all the OS-specific data needed to draw to the
   game window.  Encapsulating it here makes BattleBall easier to port to
   other OS's.

   The text-printing stuff here is ugly.  It could have a MUCH cleaner, nicer
   interface...
*/
#ifdef WIN32
struct bbGfxTarget : public gfxTargetWin
#else
struct bbGfxTarget : public gfxTargetX11
#endif
{
  pt2d		gfxSize;	// width, height of 3d gfx area in pixels
  pt2d		fontSize;	// font width, height+1 in pixels
  pt2d		textSize;	// width, height of text area, including
                                // borders, in pixels
  pt2d		cursorOrg;	// position of text cursor's (0,0) origin,
                                // in pixels
  pt2d		cursor;		// text cursor position (text coords)
  int		rightmost;	// rightmost text position printed to (text
                                // coords)

 public:
  bbGfxTarget();
  pt2d	PixelPos(const pt2d& pt);

  bbGfxTarget& operator<<(char*);
  bbGfxTarget& Box(int,int,int=1,int=1,bool hl=false);
  bbGfxTarget& Tab(int blanks=1,int newy=0);

  void  HandleResize(BBEvent *event, bool refit);
};


#endif
