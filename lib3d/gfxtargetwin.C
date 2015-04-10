#include "gfxtargetwin.h"

HINSTANCE gfxTargetWin::hInstance;
int gfxTargetWin::nCmdShow;

bool gfxTargetWin::OpenDisplay(char *dispName, int argc, char *argv[])
{
  return true;
}

void gfxTargetWin::CloseDisplay()
{
}

void gfxTargetWin::ResizeWindow(const pt2d& sz)
{
}

void gfxTargetWin::HandleResize(BBEvent *event, bool refit)
{
}


ulong gfxTargetWin::Black() const
{
  return 0;
}

ulong gfxTargetWin::White() const
{
  return (ulong)0xFFFFFF;
}

void gfxTargetWin::SetForeground(ulong color) const
{
}

void gfxTargetWin::SetBackground(ulong color) const
{
}

void gfxTargetWin::AllocColors(char *colorNames[], int numColors)
{
}

ulong* gfxTargetWin::Colors()
{
  return colors;
}

void gfxTargetWin::DrawSegments(pt2d* segs, int numSegs) const
{
}

void gfxTargetWin::DrawLine(int x1, int y1, int x2, int y2)
{
}

void gfxTargetWin::DrawLines(pt2d* points, int npoints)
{
}

void gfxTargetWin::DrawRectangle(const pt2d& p,const pt2d& sz)
{
}

void gfxTargetWin::DrawRectangle(int x, int y, unsigned int width, unsigned int height)
{
}

void gfxTargetWin::DrawString(const pt2d& p,const char* s)
{
}

void gfxTargetWin::DrawImageString(int x, int y, const char* string, int length)
{
}


void gfxTargetWin::FillRectangle(const pt2d& p,const pt2d& sz)
{
}

void gfxTargetWin::FillPolygon(pt2d* points, int npoints, bool convex) const
{
}

void gfxTargetWin::DoubleBufferBegin()
{
}

void gfxTargetWin::DoubleBufferEnd()
{
}

void gfxTargetWin::Flush()
{
}

void gfxTargetWin::SetLineAttributes(unsigned int line_width, int line_style, int cap_style, int join_style)
{
}

int gfxTargetWin::Pending()
{
}

BBEvent gfxTargetWin::NextEvent()
{
  BBEvent event;
  return event;
}

