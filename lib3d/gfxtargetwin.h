// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.

#ifndef GFXTARGETWIN_h
#define GFXTARGETWIN_h

#include "gfxtarget.h"
#include <windows.h>

struct gfxTargetWin : public gfxTarget
{
  virtual bool	OpenDisplay(char *dispName, int argc, char *argv[]);
  virtual void  CloseDisplay();
  virtual void	ResizeWindow(const pt2d& sz);
  virtual void  HandleResize(BBEvent *event, bool refit);

  virtual ulong	Black() const;
  virtual ulong	White() const;
  virtual void	SetForeground(ulong color) const;  // should this really be const?
  virtual void  SetBackground(ulong color) const;
  virtual void  AllocColors(char *colorNames[], int numColors);
  virtual ulong	*Colors();

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

  virtual void  SetLineAttributes(unsigned int line_width, int line_style, int cap_style, int join_style);

  virtual int   Pending();
  virtual BBEvent NextEvent();

  ulong* colors;

  BBEvent MsgQ[1000];
  BBEvent* nextMsg;
  BBEvent* lastMsg;
  HWND hWnd;
  HDC  hdc;

  int newWidth;

  static HINSTANCE hInstance;
  static int nCmdShow;
};

#endif // #ifndef GFXTARGETWIN_h

