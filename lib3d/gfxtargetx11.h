// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.

#ifndef GFXTARGETX11_h
#define GFXTARGETX11_h

#include "gfxtarget.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

//===========================================================================
/* Graphics Target class.
   This class contains all the OS-specific data needed to draw graphics.
   In this case, this data is X Windows-specific stuff.
*/

struct gfxTargetX11 : public gfxTarget
{
  Display       *disp;		// X Display
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

  virtual bool	OpenDisplay(char *dispName, int argc, char *argv[]);
  virtual void  CloseDisplay();
  virtual void	ResizeWindow(const pt2d& sz);
  virtual void  CreateWindow(int argc, char *argv[]);
  virtual void	ConnectToWM(char*[],int,char*,short,short,short,short);
  virtual void  HandleResize(BBEvent *event, bool refit);

  virtual ulong	Black() const;
  virtual ulong	White() const;
  virtual void	SetForeground(ulong color) const;  // should this really be const?
  virtual void  SetBackground(ulong color) const;
  virtual void  AllocColors(char *colorNames[], int numColors);
  virtual ulong	*Colors();
  virtual void  GetColor(char *name, ulong& colorPixel);

  virtual void  DrawSegments(pt2d* segs, int numSegs) const;
  virtual void  DrawLine(int x1, int y1, int x2, int y2);
  virtual void  DrawLines(pt2d* points, int npoints);
  virtual void	DrawRectangle(const pt2d& p,const pt2d& sz);
  virtual void  DrawRectangle(int x, int y, unsigned int width, unsigned int height);
  virtual void	DrawString(const pt2d& p,const char* s);
  virtual void  DrawImageString(int x, int y, const char* string, int length);

  virtual void	FillRectangle(const pt2d& p,const pt2d& sz);
  virtual void  FillPolygon(pt2d* points, int npoints, bool convex) const;

  virtual void	DoubleBufferBegin();
  virtual void	DoubleBufferEnd();
  virtual void  Flush();

  virtual void  LoadFont(const char* name);

  virtual void	GetGC();

  virtual void  SetLineAttributes(unsigned int line_width, int line_style, int cap_style, int join_style);

  virtual int   Pending();
  virtual BBEvent NextEvent();

  KB_Key LookupKeysym(XKeyEvent* key_event);
};

#endif // #ifndef GFXTARGETX11_h

