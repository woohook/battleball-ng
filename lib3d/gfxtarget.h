// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef GFXTARGET_h
#define GFXTARGET_h


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
{
  virtual bool	OpenDisplay(char *dispName, int argc, char *argv[]) = 0;
  virtual void  CloseDisplay() = 0;
  virtual void	ResizeWindow(const pt2d& sz) = 0;
  virtual void  CreateWindow(int argc, char *argv[]) = 0;
  virtual void	ConnectToWM(char*[],int,char*,short,short,short,short) = 0;
  virtual void  HandleResize(BBEvent *event, bool refit) = 0;

  virtual ulong	Black() const = 0;
  virtual ulong	White() const = 0;
  virtual void	SetForeground(ulong color) const = 0;  // should this really be const?
  virtual void  SetBackground(ulong color) const = 0;
  virtual void  AllocColors(char *colorNames[], int numColors) = 0;
  virtual ulong	*Colors() = 0;
  virtual void  GetColor(char *name, ulong& colorPixel) = 0;

  virtual void  DrawSegments(pt2d* segs, int numSegs) const = 0;
  virtual void  DrawLine(int x1, int y1, int x2, int y2) = 0;
  virtual void  DrawLines(pt2d* points, int npoints) = 0;
  virtual void	DrawRectangle(const pt2d& p,const pt2d& sz) = 0;
  virtual void  DrawRectangle(int x, int y, unsigned int width, unsigned int height) = 0;
  virtual void	DrawString(const pt2d& p,const char* s) = 0;
  virtual void  DrawImageString(int x, int y, const char* string, int length) = 0;

  virtual void	FillRectangle(const pt2d& p,const pt2d& sz) = 0;
  virtual void  FillPolygon(pt2d* points, int npoints, bool convex) const = 0;

  virtual void	DoubleBufferBegin() = 0;
  virtual void	DoubleBufferEnd() = 0;
  virtual void  Flush() = 0;

  virtual void  LoadFont(const char* name) = 0;
  virtual void  UnloadFont() = 0;
  virtual void  SetFont() = 0;

  virtual void  FreePixmap() = 0;
  virtual void	GetGC() = 0;
  virtual void  FreeGC() = 0;

  virtual void  SetLineAttributes(unsigned int line_width, int line_style, int cap_style, int join_style) = 0;

  virtual int   Pending() = 0;
  virtual BBEvent NextEvent() = 0;
};


#endif
