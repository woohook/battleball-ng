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
bool gfxTarget::OpenDisplay(char *dispName) {
  disp= XOpenDisplay(dispName);
  if (disp==NULL) {
    cerr << "cannot connect to X server " << XDisplayName(dispName) << endl;
    return false;
  }
  screenNo= DefaultScreen(disp);
  rootWin= RootWindow(disp, screenNo);
  return true;
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


