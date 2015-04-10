// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include <iostream>
#include "gfxtarget.h"

using namespace std;

/************************************************************************/
// In:  dispName = name of X display to open
// Out:	disp, screenNo, & rootWin member fields set if display was opened
//   	successfully
// Returns true if display was opened successfully, false otherwise
bool gfxTarget::OpenDisplay(char *dispName, int argc, char *argv[]) {
  disp= XOpenDisplay(dispName);
  if (disp==NULL) {
    cerr << "cannot connect to X server " << XDisplayName(dispName) << endl;
    return false;
  }
  screenNo= DefaultScreen(disp);
  rootWin= RootWindow(disp, screenNo);

  CreateWindow(argc, argv);

  return true;
}


/*----------------------------------------------------------------------*/
void gfxTarget::CreateWindow(int argc, char *argv[])
{
  static short	x=48, y=48;

  win= XCreateSimpleWindow(disp, rootWin, x, y,
			   DEFAULTWINWIDTH, DEFAULTWINHEIGHT, 4,
			   Black(), White());
  ConnectToWM(argv,argc,"BattleBall",x,y,MINWINWIDTH,MINWINHEIGHT);
  XSelectInput(disp,win,
	       ExposureMask|KeyPressMask|KeyReleaseMask|StructureNotifyMask);
  LoadFont("6x13");
  XGetWindowAttributes(disp,win,&winAttribs);
  pm= XCreatePixmap(disp,win,winAttribs.width,winAttribs.height,winAttribs.depth);

  GetGC();
  XMapWindow(disp,win);
}


void gfxTarget::ConnectToWM(char *argv[], int argc, char *progName,
			    short x, short y, short width, short height)
{ XSizeHints	size_hints;
  XWMHints	wm_hints;
  XClassHint	class_hints;
  XTextProperty	winNameProp,
  		iconNameProp;
  char		*winName= progName,
  		*iconName=   progName;
  
  size_hints.flags= PPosition|PSize|PMinSize;
  size_hints.min_width= width;
  size_hints.min_height= height;

  if (XStringListToTextProperty(&winName,1,&winNameProp)==0) {
    cerr << progName << ": structure allocation for windowName failed\n";
    exit(-1);
  }
  if (XStringListToTextProperty(&iconName,1,&iconNameProp)==0) {
    cerr << progName << ": structure allocation for iconName failed\n";
    exit(-1);
  }
  wm_hints.initial_state= NormalState;
  wm_hints.input= True;
  // wm_hints.icon_pixmap= icon_pixmap;
  wm_hints.flags= StateHint | InputHint; // |IconPixmapHint
  class_hints.res_name= progName;
  class_hints.res_class= progName;
  XSetWMProperties(disp,win,&winNameProp,&iconNameProp,argv,argc,
                   &size_hints,&wm_hints,&class_hints);
}

ulong gfxTarget::Black() const
{
  return BlackPixel(disp,screenNo);
}

ulong gfxTarget::White() const
{
  return WhitePixel(disp,screenNo);
}

void gfxTarget::SetForeground(ulong color) const  // should this really be const?
{
  XSetForeground(disp,gc,color);
  XSetForeground(disp,pmgc,color);
}

void gfxTarget::SetBackground(ulong color) const  // should this really be const?
{
  XSetBackground(disp,gc,color);
  XSetBackground(disp,pmgc,color);
}

void gfxTarget::AllocColors(char *colorNames[], int numColors) {
  forii(numColors)
    GetColor(colorNames[i],colors[i]);
}

ulong* gfxTarget::Colors()
{
  return colors;
};

void gfxTarget::GetColor(char *name, ulong& colorPixel)
{ Colormap	cmap;
  XColor	color1,color2;
  
  cmap= DefaultColormap(disp,screenNo);
  XAllocNamedColor(disp,cmap,name,&color1,&color2);
  colorPixel= color1.pixel;
}

void gfxTarget::DrawString(const pt2d& p,const char* s)
{
  XDrawImageString(disp,win,gc,(int)p.x,(int)p.y,s,strlen(s));
}

void gfxTarget::DrawRectangle(const pt2d& p,const pt2d& sz)
{
  XDrawRectangle(disp,win,gc,(int)p.x,(int)p.y,(int)sz.x,(int)sz.y);
}

void gfxTarget::FillRectangle(const pt2d& p,const pt2d& sz)
{
  XFillRectangle(disp,win,gc,(int)p.x,(int)p.y,(int)sz.x,(int)sz.y);
}

void gfxTarget::DrawSegments(XSegment* segs, int numSegs) const
{
  XDrawSegments(disp,win,gc,segs,numSegs);
}

void gfxTarget::DrawLine(int x1, int y1, int x2, int y2)
{
  XDrawLine(disp,win,gc, x1, y1, x2, y2);
}

void gfxTarget::DrawImageString(int x, int y, const char* string, int length)
{
  XDrawImageString(disp, win, gc, x, y, string, length);
}

void gfxTarget::DrawRectangle(int x, int y, unsigned int width, unsigned int height)
{
  XDrawRectangle(disp, win, gc, x, y, width, height);
}

void gfxTarget::DrawLines(XPoint* points, int npoints, int mode)
{
  XDrawLines(disp, win, gc, points, npoints, mode);
}

void gfxTarget::FillPolygon(XPoint* points, int npoints, int shape, int mode) const
{
  XFillPolygon(disp, win, gc, points, npoints, shape, mode);
}

void gfxTarget::LoadFont(const char *name)
{
  font= XLoadQueryFont(disp, name);
  if (font==NULL) {
    cerr << "cannot open font " << name << endl;
    exit(-1);
  }
}

void gfxTarget::UnloadFont()
{
  XUnloadFont(disp, font->fid);
}

void gfxTarget::SetFont()
{
  XSetFont(disp, gc, font->fid);
  XSetFont(disp, pmgc, font->fid);
}

void gfxTarget::FreePixmap()
{
  XFreePixmap(disp, pm);
}

void gfxTarget::GetGC()
{ unsigned long valuemask= 0;
  XGCValues values;
  
  gc = XCreateGC(disp, win, valuemask, &values);
  pmgc = XCreateGC(disp, pm, valuemask, &values);

  SetForeground(Black());
  SetBackground(White());
  SetLineAttributes(0,LineSolid,CapRound,JoinRound);
  SetFont();
}

void gfxTarget::FreeGC()
{
  XFreeGC(disp, gc);
  XFreeGC(disp, pmgc);
}

void gfxTarget::CloseDisplay()
{
  XCloseDisplay(disp);
}

void gfxTarget::SetLineAttributes(unsigned int line_width, int line_style, int cap_style, int join_style)
{
  XSetLineAttributes(disp, gc, line_width, line_style, cap_style, join_style);
  XSetLineAttributes(disp, pmgc, line_width, line_style, cap_style, join_style);
}

void gfxTarget::DoubleBufferBegin() {
  savedWin= win;
  savedGc= gc;
  win= pm;
  gc= pmgc;
  doubleBuffer = true;
}

void gfxTarget::DoubleBufferEnd() {
  win= savedWin;
  gc= savedGc;

  XCopyArea(disp,pm,win,gc,0,0,winAttribs.width,winAttribs.height,0,0);

  /* using XGetImage/XPutImage takes longer than XCopyArea
     XImage *ximg= XGetImage(disp,pm,0,0,gfxSize.x,gfxSize.y,
                             0x000000ff,ZPixmap);
     XPutImage(disp,win,gc,ximg,0,0,0,0,gfxSize.x,gfxSize.y);
     XDestroyImage(ximg);
  */

  doubleBuffer = false;
}

void gfxTarget::Flush()
{
  if(doubleBuffer)
    DoubleBufferEnd();
  XFlush(disp);
}

void gfxTarget::ResizeWindow(const pt2d& sz)
{
  XResizeWindow(disp,win,(int)sz.x,(int)sz.y);
}

void gfxTarget::HandleResize(XEvent *event, bool refit)
{
  XGetWindowAttributes(disp,win,&winAttribs);
  XFreePixmap(disp,pm);
  pm= XCreatePixmap(disp,win,winAttribs.width,winAttribs.height,winAttribs.depth);
}

int gfxTarget::Pending()
{
  return XPending(disp);
}

void gfxTarget::NextEvent(XEvent* event)
{
  XNextEvent(disp, event);
}

KeySym gfxTarget::LookupKeysym(XKeyEvent* key_event)
{
  return XLookupKeysym(key_event, 0);
}

