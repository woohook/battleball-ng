// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#include <iostream>
#include "gfxtargetx11.h"
#include <stdlib.h>
#include <string.h>
#include "general.h"

using namespace std;

/************************************************************************/
// In:  dispName = name of X display to open
// Out:	disp, screenNo, & rootWin member fields set if display was opened
//   	successfully
// Returns true if display was opened successfully, false otherwise
bool gfxTargetX11::OpenDisplay(char *dispName, int argc, char *argv[]) {
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
void gfxTargetX11::CreateWindow(int argc, char *argv[])
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


void gfxTargetX11::ConnectToWM(char *argv[], int argc, char *progName,
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

ulong gfxTargetX11::Black() const
{
  return BlackPixel(disp,screenNo);
}

ulong gfxTargetX11::White() const
{
  return WhitePixel(disp,screenNo);
}

void gfxTargetX11::SetForeground(ulong color) const  // should this really be const?
{
  XSetForeground(disp,gc,color);
  XSetForeground(disp,pmgc,color);
}

void gfxTargetX11::SetBackground(ulong color) const  // should this really be const?
{
  XSetBackground(disp,gc,color);
  XSetBackground(disp,pmgc,color);
}

void gfxTargetX11::AllocColors(char *colorNames[], int numColors) {
  forii(numColors)
    GetColor(colorNames[i],colors[i]);
}

ulong* gfxTargetX11::Colors()
{
  return colors;
};

void gfxTargetX11::GetColor(char *name, ulong& colorPixel)
{ Colormap	cmap;
  XColor	color1,color2;
  
  cmap= DefaultColormap(disp,screenNo);
  XAllocNamedColor(disp,cmap,name,&color1,&color2);
  colorPixel= color1.pixel;
}

void gfxTargetX11::DrawString(const pt2d& p,const char* s)
{
  XDrawImageString(disp,win,gc,(int)p.x,(int)p.y,s,strlen(s));
}

void gfxTargetX11::DrawRectangle(const pt2d& p,const pt2d& sz)
{
  XDrawRectangle(disp,win,gc,(int)p.x,(int)p.y,(int)sz.x,(int)sz.y);
}

void gfxTargetX11::FillRectangle(const pt2d& p,const pt2d& sz)
{
  XFillRectangle(disp,win,gc,(int)p.x,(int)p.y,(int)sz.x,(int)sz.y);
}

void gfxTargetX11::DrawSegments(pt2d* segs, int numSegs) const
{
  int i;
  XSegment xsegs[1000];
  for(i=0; i < numSegs; i++)
  {
    xsegs[i].x1 = segs[i*2].x;
    xsegs[i].y1 = segs[i*2].y;
    xsegs[i].x2 = segs[i*2+1].x;
    xsegs[i].y2 = segs[i*2+1].y;
  }
  XDrawSegments(disp,win,gc,xsegs,numSegs);
}

void gfxTargetX11::DrawLine(int x1, int y1, int x2, int y2)
{
  XDrawLine(disp,win,gc, x1, y1, x2, y2);
}

void gfxTargetX11::DrawImageString(int x, int y, const char* string, int length)
{
  XDrawImageString(disp, win, gc, x, y, string, length);
}

void gfxTargetX11::DrawRectangle(int x, int y, unsigned int width, unsigned int height)
{
  XDrawRectangle(disp, win, gc, x, y, width, height);
}

void getXPoints(pt2d* pts, XPoint* xpts, int count)
{
  forii(count) {
    xpts[i].x= (short) pts[i].x;
    xpts[i].y= (short) pts[i].y;
  }
}

void gfxTargetX11::DrawLines(pt2d* points, int npoints)
{
#ifdef __GNUC__
  XPoint xpts[100];
#else
  XPoint *xpts= new XPoint[npoints];
#endif
  getXPoints(points, xpts, npoints);
  XDrawLines(disp, win, gc, xpts, npoints, CoordModeOrigin);
#ifndef __GNUC__
  delete [] xpts;
#endif
}

void gfxTargetX11::FillPolygon(pt2d* points, int npoints, bool convex) const
{
  int shape = Convex;
  if(!convex)
  {
    shape = Nonconvex;
  }

#ifdef __GNUC__
  XPoint xpts[100];
#else
  XPoint *xpts= new XPoint[npoints];
#endif
  getXPoints(points, xpts, npoints);
  XFillPolygon(disp, win, gc, xpts, npoints, shape, CoordModeOrigin);
#ifndef __GNUC__
  delete [] xpts;
#endif
}

void gfxTargetX11::LoadFont(const char *name)
{
  font= XLoadQueryFont(disp, name);
  if (font==NULL) {
    cerr << "cannot open font " << name << endl;
    exit(-1);
  }
}

void gfxTargetX11::UnloadFont()
{
  XUnloadFont(disp, font->fid);
}

void gfxTargetX11::SetFont()
{
  XSetFont(disp, gc, font->fid);
  XSetFont(disp, pmgc, font->fid);
}

void gfxTargetX11::FreePixmap()
{
  XFreePixmap(disp, pm);
}

void gfxTargetX11::GetGC()
{ unsigned long valuemask= 0;
  XGCValues values;
  
  gc = XCreateGC(disp, win, valuemask, &values);
  pmgc = XCreateGC(disp, pm, valuemask, &values);

  SetForeground(Black());
  SetBackground(White());
  SetLineAttributes(0,LineSolid,CapRound,JoinRound);
  SetFont();
}

void gfxTargetX11::FreeGC()
{
  XFreeGC(disp, gc);
  XFreeGC(disp, pmgc);
}

void gfxTargetX11::CloseDisplay()
{
  if(disp != NULL)
  {
    XCloseDisplay(disp);
  }
}

void gfxTargetX11::SetLineAttributes(unsigned int line_width, int line_style, int cap_style, int join_style)
{
  XSetLineAttributes(disp, gc, line_width, line_style, cap_style, join_style);
  XSetLineAttributes(disp, pmgc, line_width, line_style, cap_style, join_style);
}

void gfxTargetX11::DoubleBufferBegin() {
  savedWin= win;
  savedGc= gc;
  win= pm;
  gc= pmgc;
  doubleBuffer = true;
}

void gfxTargetX11::DoubleBufferEnd() {
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

void gfxTargetX11::Flush()
{
  if(doubleBuffer)
    DoubleBufferEnd();
  XFlush(disp);
}

void gfxTargetX11::ResizeWindow(const pt2d& sz)
{
  XResizeWindow(disp,win,(int)sz.x,(int)sz.y);
}

void gfxTargetX11::HandleResize(BBEvent *event, bool refit)
{
  XGetWindowAttributes(disp,win,&winAttribs);
  XFreePixmap(disp,pm);
  pm= XCreatePixmap(disp,win,winAttribs.width,winAttribs.height,winAttribs.depth);
}

int gfxTargetX11::Pending()
{
  return XPending(disp);
}

BBEvent gfxTargetX11::NextEvent()
{
  BBEvent event;

  XEvent xevent;
  XNextEvent(disp, &xevent);

  switch (xevent.type)
  {
    case Expose:
      event.type = BBE_Expose;
      break;
    case KeyPress:
    case KeyRelease:
      event.type    = BBE_Key;
      event.pressed = (xevent.type == KeyPress);
      event.key     = LookupKeysym((XKeyEvent *)(&xevent));
      break;
    case ConfigureNotify:
      event.type  = BBE_Resize;
      event.width = ((XConfigureEvent *)&xevent)->width;
      break;
    default:
      event.type = BBE_Unknown;
      break;
  }

  return event;
}

KB_Key gfxTargetX11::LookupKeysym(XKeyEvent* key_event)
{
  KB_Key  key  = KBK_None;
  KeySym xkey = XLookupKeysym(key_event, 0);

  switch(xkey)
  {
  case XK_Down: key = KBK_Down; break;
  case XK_End: key = KBK_End; break;
  case XK_Escape: key = KBK_Escape; break;
  case XK_Home: key = KBK_Home; break;
  case XK_Insert: key = KBK_Insert; break;
  case XK_KP_Down: key = KBK_KP_Down; break;
  case XK_KP_Left: key = KBK_KP_Left; break;
  case XK_KP_Page_Down: key = KBK_KP_Page_Down; break;
  case XK_KP_Page_Up: key = KBK_KP_Page_Up; break;
  case XK_KP_Right: key = KBK_KP_Right; break;
  case XK_KP_Up: key = KBK_KP_Up; break;
  case XK_Left: key = KBK_Left; break;
  case XK_Page_Down: key = KBK_Page_Down; break;
  case XK_Page_Up: key = KBK_Page_Up; break;
  case XK_Right: key = KBK_Right; break;
  case XK_Tab: key = KBK_Tab; break;
  case XK_Up: key = KBK_Up; break;
  case XK_a: key = KBK_a; break;
  case XK_d: key = KBK_d; break;
  case XK_e: key = KBK_e; break;
  case XK_f: key = KBK_f; break;
  case XK_g: key = KBK_g; break;
  case XK_h: key = KBK_h; break;
  case XK_i: key = KBK_i; break;
  case XK_j: key = KBK_j; break;
  case XK_k: key = KBK_k; break;
  case XK_l: key = KBK_l; break;
  case XK_p: key = KBK_p; break;
  case XK_q: key = KBK_q; break;
  case XK_s: key = KBK_s; break;
  case XK_space: key = KBK_space; break;
  case XK_t: key = KBK_t; break;
  case XK_v: key = KBK_v; break;
  case XK_y: key = KBK_y; break;
  case XK_z: key = KBK_z; break;
  default: key = KBK_None; break;
  }

  return key;
}

