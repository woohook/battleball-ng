// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include <cstring>      // to get strlen
#include <iostream>
#include "bbgfxtarget.h"

using namespace std;


/*======================================================================*/
bbGfxTarget::bbGfxTarget() :
  gfxSize(pt2d(DEFAULTWINWIDTH,DEFAULTWINWIDTH/2)),
  fontSize(pt2d(6,14)),
  cursor(pt2d(0,0)),
  rightmost(0)
{
  cursorOrg= pt2d(8,gfxSize.y);
  textSize= pt2d(gfxSize.x,fontSize.y*6+2);
}


/*----------------------------------------------------------------------*/
// pt = character position in text window
// Returns the pixel position in the text window corresponding to 'pt'
pt2d bbGfxTarget::PixelPos(const pt2d& pt) {
  return pt2d(pt.x*fontSize.x, pt.y*fontSize.y+gfxSize.y);
}


/*----------------------------------------------------------------------*/
bbGfxTarget& bbGfxTarget::operator<<(char *s) {
  int len= strlen(s);
  DrawString(cursorOrg +cursor*fontSize +pt2d(0,fontSize.y-2),s);
  if (cursor.x+len >rightmost)
    rightmost= (int) cursor.x+len;
  cursor.y++;
  return *this;
}


/*-------------------------------------------------------------------------*/
bbGfxTarget& bbGfxTarget::Box(int x, int y, int width, int height,
			      bool highlight) {
  DrawRectangle(cursorOrg +pt2d(x+cursor.x,y)*fontSize +pt2d(-3,1),
		pt2d(width,height)*fontSize +pt2d(4,-2));
  if (highlight) {
    DrawRectangle(cursorOrg +pt2d(x+cursor.x,y)*fontSize +pt2d(-4,0),
		  pt2d(width,height)*fontSize +pt2d(6,0));
  }
  return *this;
}


/*-------------------------------------------------------------------------*/
bbGfxTarget& bbGfxTarget::Tab(int spaces, int newy) {
  cursor.x= rightmost +spaces;
  cursor.y= newy;
  return *this;
}

void bbGfxTarget::HandleResize(BBEvent *event, bool refit)
{
  gfxSize.x= event->width;
  textSize.x= gfxSize.x;
  gfxSize.y= gfxSize.x/2;
  cursorOrg.y= gfxSize.y;
  if (refit)
    ResizeWindow(gfxSize +pt2d(0,textSize.y));

#ifdef WIN32
  gfxTargetWin::HandleResize(event, refit);
#else
  gfxTargetX11::HandleResize(event, refit);
#endif
}
