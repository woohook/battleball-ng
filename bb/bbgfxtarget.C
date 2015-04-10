// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include <cstring>      // to get strlen
#include <iostream>
#include <X11/Xlib.h>    // to get XEvent, XFontStruct
#include "bbgfxtarget.h"

using namespace std;

#define DEFAULTWINWIDTH 512
#define DEFAULTWINHEIGHT (256+6*14+2)


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
void bbGfxTarget::LoadFont(const char *name) {
  font= XLoadQueryFont(disp,name);
  if (font==NULL) {
    cerr << "cannot open font " << name << endl;
    exit(-1);
  }
}


/*----------------------------------------------------------------------*/
void bbGfxTarget::GetGC()
{ unsigned long valuemask= 0;
  XGCValues values;
  
  gc= XCreateGC(disp,win,valuemask,&values);
  XSetForeground(disp,gc,BlackPixel(disp,screenNo));
  XSetBackground(disp,gc,WhitePixel(disp,screenNo));
  XSetLineAttributes(disp,gc,0,LineSolid,CapRound,JoinRound);
  XSetFont(disp,gc,font->fid);

  pmgc= XCreateGC(disp,pm,valuemask,&values);
  XSetForeground(disp,pmgc,BlackPixel(disp,screenNo));
  XSetBackground(disp,pmgc,WhitePixel(disp,screenNo));
  XSetLineAttributes(disp,pmgc,0,LineSolid,CapRound,JoinRound);
  XSetFont(disp,pmgc,font->fid);
}


/*----------------------------------------------------------------------*/
void bbGfxTarget::CreateWindow(int argc, char *argv[]) {
  static short	x=0,
  		y=0;
  XWindowAttributes attr;
  
  x += 48;
  y += 48;
  win= XCreateSimpleWindow(disp, rootWin, x, y,
			   DEFAULTWINWIDTH, DEFAULTWINHEIGHT, 4,
			   Black(), White());
  ConnectToWM(argv,argc,"BattleBall",x,y,MINWINWIDTH,MINWINHEIGHT);
  XSelectInput(disp,win,
	       ExposureMask|KeyPressMask|KeyReleaseMask|StructureNotifyMask);
  LoadFont("6x13");
  XGetWindowAttributes(disp,win,&attr);
  pm= XCreatePixmap(disp,win,attr.width,attr.height,attr.depth);

  GetGC();
  XMapWindow(disp,win);
}


/*----------------------------------------------------------------------*/
void bbGfxTarget::AllocColors(char *colorNames[], int numColors) {
  forii(numColors)
    GetColor(colorNames[i],colors[i]);
}


/*----------------------------------------------------------------------*/
void bbGfxTarget::GetColor(char *name, ulong& colorPixel)
{ Colormap	cmap;
  XColor	color1,color2;
  
  cmap= DefaultColormap(disp,screenNo);
  XAllocNamedColor(disp,cmap,name,&color1,&color2);
  colorPixel= color1.pixel;
}


/*----------------------------------------------------------------------*/
void bbGfxTarget::HandleResize(XEvent *event, bool refit)
{ XWindowAttributes attr;
  
  gfxSize.x= ((XConfigureEvent *)event)->width;
  textSize.x= gfxSize.x;
  gfxSize.y= gfxSize.x/2;
  cursorOrg.y= gfxSize.y;
  if (refit)
    ResizeWindow(gfxSize +pt2d(0,textSize.y));
  
  XGetWindowAttributes(disp,win,&attr);
  XFreePixmap(disp,pm);
  pm= XCreatePixmap(disp,win,attr.width,attr.height,attr.depth);
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


/*-------------------------------------------------------------------------*/
void bbGfxTarget::DoubleBufferBegin() {
  savedWin= win;
  savedGc= gc;
  win= pm;
  gc= pmgc;
}


/*-------------------------------------------------------------------------*/
void bbGfxTarget::DoubleBufferEnd() {
  win= savedWin;
  gc= savedGc;

  XCopyArea(disp,pm,win,gc,0,0,(int)gfxSize.x,(int)gfxSize.y,0,0);

  /* using XGetImage/XPutImage takes longer than XCopyArea
     XImage *ximg= XGetImage(disp,pm,0,0,gfxSize.x,gfxSize.y,
                             0x000000ff,ZPixmap);
     XPutImage(disp,win,gc,ximg,0,0,0,0,gfxSize.x,gfxSize.y);
     XDestroyImage(ximg);
  */
}
