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
  
  bool	OpenDisplay(char*);
  void	ConnectToWM(char*[],int,char*,short,short,short,short);

  ulong	Black() const;
  ulong	White() const;
  void	SetForeground(ulong color) const;  // should this really be const?
  void	DrawString(const pt2d& p,const char* s);
  void	DrawRectangle(const pt2d& p,const pt2d& sz);
  void	FillRectangle(const pt2d& p,const pt2d& sz);
  void	ResizeWindow(const pt2d& sz);
};


#endif
