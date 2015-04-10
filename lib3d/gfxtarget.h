// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef GFXTARGET_h
#define GFXTARGET_h


#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "general.h"
#include "pt2d.h"
#include "bbcolor.h"

#define DEFAULTWINWIDTH 512
#define DEFAULTWINHEIGHT (256+6*14+2)
#define MINWINWIDTH 400
#define MINWINHEIGHT (200+6*14+2)

//===========================================================================
/* Graphics Target class.
   This class contains all the OS-specific data needed to draw graphics.
   In this case, this data is X Windows-specific stuff.  Encapsulating the 
   X Windows stuff here makes the graphics library cleaner and easier to port
   to other OS's.

   (Not *all* the X stuff is encapsulated here, but it's a start)
*/
struct gfxTarget
{ Display       *disp;		// X Display
  Drawable      win;		// window on the display
  Window        rootWin;	// root window of the display
  int           screenNo;	// default screen of the display
  GC            gc;		// graphics context for window 'win'
  Pixmap        pm;		// Pixmap for double-buffering window 'win'
  GC            pmgc;		// graphics context for the pixmap
  XFontStruct   *font;
  Drawable	savedWin;             // X window; used for double-buffering
  GC		savedGc;              // X GC;     used for double-buffering
  ulong         colors[totalColors];  // X color lookup table
  XWindowAttributes winAttribs;       // X11 window attributes
  bool          doubleBuffer;         // double buffer game window

  bool	OpenDisplay(char *dispName, int argc, char *argv[]);
  void  CloseDisplay();
  void	ResizeWindow(const pt2d& sz);
  void  CreateWindow(int argc, char *argv[]);
  void	ConnectToWM(char*[],int,char*,short,short,short,short);
  void  HandleResize(XEvent *event, bool refit);

  ulong	Black() const;
  ulong	White() const;
  void	SetForeground(ulong color) const;  // should this really be const?
  void  SetBackground(ulong color) const;
  void  AllocColors(char *colorNames[], int numColors);
  ulong	*Colors();
  void  GetColor(char *name, ulong& colorPixel);

  void  DrawSegments(XSegment* segs, int numSegs) const;
  void  DrawLine(int x1, int y1, int x2, int y2);
  void  DrawLines(XPoint* points, int npoints, int mode);
  void	DrawRectangle(const pt2d& p,const pt2d& sz);
  void  DrawRectangle(int x, int y, unsigned int width, unsigned int height);
  void	DrawString(const pt2d& p,const char* s);
  void  DrawImageString(int x, int y, const char* string, int length);

  void	FillRectangle(const pt2d& p,const pt2d& sz);
  void  FillPolygon(XPoint* points, int npoints, int shape, int mode) const;

  void	DoubleBufferBegin();
  void	DoubleBufferEnd();
  void  Flush();

  void  LoadFont(const char* name);
  void  UnloadFont();
  void  SetFont();

  void  CreateGC(unsigned long valuemask, XGCValues* values);
  void  FreePixmap();
  void	GetGC();
  void  FreeGC();

  void  SetLineAttributes(unsigned int line_width, int line_style, int cap_style, int join_style);

  int   Pending();
  void  NextEvent(XEvent* event);
  KeySym  LookupKeysym(XKeyEvent* key_event);
};


#endif
