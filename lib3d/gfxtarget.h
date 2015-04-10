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

enum KB_Key
{
KBK_None,
KBK_Down,
KBK_End,
KBK_Escape,
KBK_Home,
KBK_Insert,
KBK_KP_Down,
KBK_KP_Left,
KBK_KP_Page_Down,
KBK_KP_Page_Up,
KBK_KP_Right,
KBK_KP_Up,
KBK_Left,
KBK_Page_Down,
KBK_Page_Up,
KBK_Right,
KBK_Tab,
KBK_Up,
KBK_a,
KBK_d,
KBK_e,
KBK_f,
KBK_g,
KBK_h,
KBK_i,
KBK_j,
KBK_k,
KBK_l,
KBK_p,
KBK_q,
KBK_s,
KBK_space,
KBK_t,
KBK_v,
KBK_y,
KBK_z,
KBK_MAX
};

enum BBEventType {BBE_Unknown, BBE_Expose, BBE_Key, BBE_Resize};

struct BBEvent
{
  BBEventType type;
  union
  {
    double width;
    KB_Key key;
  };
  bool pressed;
};

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
  void  HandleResize(BBEvent *event, bool refit);

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

  void  FreePixmap();
  void	GetGC();
  void  FreeGC();

  void  SetLineAttributes(unsigned int line_width, int line_style, int cap_style, int join_style);

  int   Pending();
  BBEvent NextEvent();
  KB_Key LookupKeysym(XKeyEvent* key_event);
};


#endif
