// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


// 3D & 2D object shape definitions

#include "shapes.h"


#define COORD_MAX LONG_MAX


/************************************************************************/
double tankShdwPts[]= {10,-8,0, 10,8,0, -11,8,0, -11,-8,0, COORD_MAX};
int tankShdwPanels[]= {1, 0,1,2,3,-1, c_gray10, INT_MAX};
rgn3 tankShdw(tankShdwPts,tankShdwPanels,0.5);


//---------------------------------------------------------------------------
double tankPts[37]=     {-8,-8,6, -11,-8,3, -11,-8,0, 7,-8,0,
                         10,-8,3,   7,-8,6,
                         -8,8,6, -11,8,3, -11,8,0, 7,8,0,
                         10,8,3,   7,8,6,  COORD_MAX};
int tankPanels[]= {
  0, 0,5,11,6,-1,c_oliveDrab,         // Top
  0, 0,1,2,3,4,5,-1,c_steelBlue,      // Right
  0, 6,11,10,9,8,7,-1,c_steelBlue,    // Left
  0, 11,5,4,10,-1,c_oliveDrab,        // Front1
  0, 4,3,9,10,-1,c_darkOliveGreen,    // Front2
  0, 3,2,8,9,-1,c_gray40,             // Bttm
  0, 2,1,7,8,-1,c_oliveDrab,          // Back1
  0, 1,0,6,7,-1,c_darkGreen,          // Back2
  INT_MAX};
rgn3 tankRgn(tankPts,tankPanels,0.5,&tankShdw);


//---------------------------------------------------------------------------
double turrPts[28]=     { -8, -8,3, -10,-10,-3, 8,-10,-3, 8, -8,3,
                          -8,  8,3, -10, 10,-3, 8, 10,-3, 8,  8,3,
                         -12,  0,1,     // cart view pos
                         COORD_MAX};
int turrPanels[]= {
  0, 3,2,6,7,-1, c_darkGreen,     // Front
  0, 7,4,0,3,-1, c_oliveDrab,     // Top
  0, 0,1,2,3,-1, c_teamPrimary,   // Right
  0, 4,7,6,5,-1, c_teamPrimary,   // Left
  0, 4,5,1,0,-1, c_teamSecondary, // Back
  INT_MAX};
rgn3 turrRgn(turrPts,turrPanels,0.25);


//---------------------------------------------------------------------------
double barrPts[25]=     {0,-2,1, 0,-2,-1, 14,-2,-1, 14,-2,1,
                         0, 2,1, 0, 2,-1, 14, 2,-1, 14, 2,1,
                         COORD_MAX};

int barrShdwPanels[]=   {1, 0,3,7,4,-1, c_gray10, INT_MAX};
rgn3 barrShdw(barrPts,barrShdwPanels,0.25);

int barrPanels[]= {
  0, 0,1,2,3,-1, c_forestGreen,       // Right
  0, 4,7,6,5,-1, c_forestGreen,       // Left
  0, 0,3,7,4,-1, c_oliveDrab,         // Top
  0, 1,5,6,2,-1, c_darkGreen,         // Bttm
  0, 3,2,6,7,-1, c_gray40,            // Front
  INT_MAX};
rgn3 barrRgn(barrPts,barrPanels,0.25,&barrShdw);


//---------------------------------------------------------------------------
double shellPts[19]=    {-1,-1,1, -1,-1,0, 1,-1,0,
                         -1, 1,1, -1, 1,0, 1, 1,0, COORD_MAX};
int shellShdwPanels[]= {1, 1,2,5,4,-1, c_gray10, INT_MAX};
rgn3 shellShdw(shellPts,shellShdwPanels,0.5);

int shellPanels[]= {
  0, 0,2,5,3,-1, c_violet,    // Top
  0, 1,4,5,2,-1, c_violet,    // Bttm
  0, 0,1,2,  -1, c_violet,    // Right
  0, 3,5,4,  -1, c_violet,    // Left
  0, 0,3,4,1,-1, c_red,       // Back
  INT_MAX};
rgn3 shellRgn(shellPts,shellPanels,0.5,&shellShdw);


//---------------------------------------------------------------------------
double ballPts[34]=     {0,0,80,                                // top
                         32,-18,48, 0,37,48, -32,-18,48,        // perimeter
                         0,0,16,                                // bottom
                         -16,-9,32, -32,-18,0,                  // fin1
                          16,-9,32,  32,-18,0,                  // fin2
                           0,18,32,   0, 37,0,                  // fin3
                         COORD_MAX};
int ballShdwPanels[]= {1, 1,2,3,-1, c_gray10, INT_MAX};
rgn3 ballShdw(ballPts,ballShdwPanels,0.05);

int ballPanels[]= {
  0, 0,1,2,-1, c_teamPrimary,    // Top1
  0, 0,2,3,-1, c_teamSecondary,  // Top2
  0, 0,3,1,-1, c_teamPrimary,    // Top3
  0, 4,2,1,-1, c_teamSecondary,  // Bttm1
  0, 4,3,2,-1, c_teamPrimary,    // Bttm2
  0, 4,1,3,-1, c_teamSecondary,  // Bttm3
  1, 4,5,6,-1, c_black,          // Fin1
  1, 4,7,8,-1, c_black,          // Fin2
  1, 4,9,10,-1, c_black,         // Fin3
  INT_MAX};
rgn3 ballRgn(ballPts,ballPanels,0.05,&ballShdw);


//---------------------------------------------------------------------------
double hqPts[103]=              {0,-4,20, 0,-5,16, 0,-6,0, 0,6,0, 0,5,16,
                         0,4,20,                                // front
                         -8,-4,20, -8,-5,16, -8,-6,0, -8,6,0, -8,5,16,
                         -8,4,20,                               // back
                         0,-3,19, 0,-3,16, 0,-2,16, 0,-2,19,    // win1
                         0,-1,19, 0,-1,16, 0,0,16, 0,0,19,      // win2
                         0,2,19, 0,2,16, 0,3,16, 0,3,19,        // win3
                         COORD_MAX};
int hqShdwPanels[]= {
  1, 5,4,3,2,1,0,-1, c_gray10,
  1, 9,8,2,3,-1, c_gray10,
INT_MAX};
rgn3 hqShdw(hqPts,hqShdwPanels,2);

int hqPanels[]= {
  0, 0, 1, 2, 3, 4, 5, -1, c_navy,    // Front
  0, 6,11,10, 9, 8, 7, -1, c_navy,    // Back
  0, 1, 7, 8, 2, -1, c_steelBlue,     // Right
  0, 0, 6, 7, 1, -1, c_navy,          // RightTop
  0, 4, 3, 9,10, -1, c_steelBlue,     // Left
  0, 11, 5, 4,10,-1, c_navy,          // LeftTop
  0, 0, 5,11, 6, -1, c_steelBlue,     // Top
  0, 12,13,14,15,-1, c_cyan,          // Win1
  0, 16,17,18,19,-1, c_cyan,          // Win2
  0, 20,21,22,23,-1, c_cyan,          // Win3
  0, 3,2,8,9,    -1, c_black,         // bottom
  INT_MAX};
rgn3 hqRgns[NUMINSIGNIA];          // hq stands for headquarters


//---------------------------------------------------------------------------
double heliShdwPts[]= {
  48,-16,24, 24,-32,24, -24,-32,24, -48,-8,24, -152,0,24,
  -48,8,24, -24,32,24, 24,32,24, 48,16,24,
  COORD_MAX};
int heliShdwPanels[]= {1, 8,7,6,5,4,3,2,1,0,-1, c_gray10,INT_MAX};
rgn3 heliShdw(heliShdwPts,heliShdwPanels,1./16);

double heliPts[58]=     {48,-16,16, 24,-32,56, -24,-32,56,
                         -48,-8,48, -128,-2,48, -152,0,72,
                         -152,0,32, -48,-8,24, -24,-32,0,
                         24,-32,0,
                         48,16,16, 24,32,56, -24,32,56,
                         -48,8,48, -128,2,48, -48,8,24,
                         -24,32,0, 24,32,0,
                         -32,0,40,      // cart view pos
                         COORD_MAX};
int heliPanels[]= {
  0, 11,12,2,1,-1,c_brown,   // Top1
  0, 13,14,4,3,-1,c_brown,   // Top2
  0, 14,5,4,-1,c_saddleBrown,         // Top3
  0, 9,8,16,17,-1,c_darkGreen,        // Bttm1
  0, 7,6,15,-1,c_darkGreen,           // Bttm2
  0, 11,17,16,12,-1,c_oliveDrab,      // Left1
  0, 13,15,6,5,14,-1,c_oliveDrab,     // Left2
  0, 1,2,8,9,-1,c_oliveDrab,          // Right1
  0, 3,4,5,6,7,-1,c_oliveDrab,        // Right2
  0, 11,1,0,10,-1,c_cyan,             // Front1
  0, 1,9,0,-1,c_teamPrimary,          // Front2
  0, 10,0,9,17,-1,c_turquoise,        // Front3
  0, 11,10,17,-1,c_teamPrimary,       // Front4
  0, 2,12,13,3,-1,c_teamSecondary,    // Back1
  0, 13,12,16,15,-1,c_firebrick,      // Back2
  0, 7,15,16,8,-1,c_darkOliveGreen,   // Back3
  0, 2,3,7,8,-1,c_firebrick,          // Back4
  INT_MAX};
rgn3 heliRgn(heliPts,heliPanels,1./16,&heliShdw);


//---------------------------------------------------------------------------
double bladePts[]=      {
  1,10,0, -1,10,0, 0,0,0, -1,-10,0, 1,-10,0,
  COORD_MAX};
int bladePanels[]= {
  1, 0,1,2,3,4,2,-1, c_darkGreen,     // blade
  INT_MAX};
rgn3 bladeRgn(bladePts,bladePanels,0.5,&bladeRgn);


//---------------------------------------------------------------------------
// heli/tank transformation region
const int TANKHELIRGNS= 16;          // num tank/heli transformation rgn3s

double tankXfrmShdwPts[]= {
  10,-8,3, 7,-8,3, -8,-8,3, -11,-8,3, -11,0,3, -11,8,3, -8,8,3, 7,8,3,
  10,8,3,
  COORD_MAX};
double heliXfrmShdwPts[]= {
  6,-2,3, 3,-4,3, -3,-4,3, -6,-1,3, -19,0,3, -6,1,3, -3,4,3, 3,4,3, 6,2,3,
  COORD_MAX};
int tankHeliShdwPanels[]= {
  1, 0,1,2,3,4,5,6,7,8,-1, c_gray10,
  INT_MAX};
rgn3 tankHeliShdws[TANKHELIRGNS];

double tankXfrmPts[]= {
  10,-8,3, 7,-8,6, -8,-8,6, -11,-8,3,
  -11,0,3, -11,0,3, -11,0,0, -11,-8,0,
  -8,-8,0, 7,-8,0,
  10, 8,3, 7, 8,6, -8, 8,6, -11, 8,3,
  -11,0,3, -11, 8,0, -8,8,0, 7, 8,0,
  COORD_MAX};
double heliXfrmPts[]= {
  48,-16,16, 24,-32,56, -24,-32,56,
  -48,-8,48, -152,0,48, -152,0,72,
  -152,0,32, -48,-8,24, -24,-32,0,
  24,-32,0,
  48,16,16, 24,32,56, -24,32,56,
  -48,8,48, -128,0,48, -48,8,24,
  -24,32,0, 24,32,0,
  COORD_MAX};
int tankHeliPanels[]= {
  0, 11,12,2,1,-1,c_oliveDrab,       // Top1
  0, 13,4,3,-1,c_brown,              // Top2
  1, 14,5,4,-1,c_oliveDrab,          // Fin
  0, 9,8,16,17,-1,c_gray40,          // Bttm1
  0, 7,6,15,-1,c_darkGreen,          // Bttm2
  0, 11,17,16,12,-1,c_steelBlue,     // Left1
  0, 13,15,6,4,-1,c_oliveDrab,       // Left2
  0, 1,2,8,9,-1,c_steelBlue,         // Right1
  0, 3,4,6,7,-1,c_oliveDrab,         // Right2
  0, 11,1,0,10,-1,c_oliveDrab,       // Front1
  0, 1,9,0,-1,c_steelBlue,           // Front2
  0, 10,0,9,17,-1,c_darkGreen,       // Front3
  0, 11,10,17,-1,c_steelBlue,        // Front4
  0, 2,12,13,3,-1,c_darkGreen,       // Back1
  0, 13,12,16,15,-1,c_firebrick,     // BackLeft
  0, 7,15,16,8,-1,c_darkOliveGreen,  // BackBttm
  0, 2,3,7,8,-1,c_firebrick,         // BackRight
  INT_MAX};
rgn3 tankHeliRgns[TANKHELIRGNS];


//---------------------------------------------------------------------------
double turrXfrmPts[49]= {-4,-4,9, -5,-5,6, 4,-5,6, 4,-4,9,
                         -4, 4,9, -5, 5,6, 4, 5,6, 4, 4,9,
                         4,-1,8, 4,-1,7, 11,-1,7, 11,-1,8,
                         4, 1,8, 4, 1,7, 11, 1,7, 11, 1,8,
                         COORD_MAX};
double bladXfrmPts[49]= {-20,-2,15, -20,-2,15, 0,0,15, 0,0,15,
                         -20, 2,15, -20, 2,15, 0,0,15, 0,0,15,
                         0,0,15, 0,0,15, 20,-2,15, 20,-2,15,
                         0,0,15, 0,0,15, 20, 2,15, 20, 2,15,
                         COORD_MAX};
int turrBladPanels[]= {
  0,  3, 2, 6, 7, -1, c_darkGreen,            // turrFront
  0,  7, 4, 0, 3, -1, c_oliveDrab,            // turrTop
  0,  0, 1, 2, 3, -1, c_teamPrimary,          // turrRight
  0,  4, 7, 6, 5, -1, c_teamPrimary,          // turrLeft
  0,  4, 5, 1, 0, -1, c_teamSecondary,        // turrBack
  0,  8, 9, 10, 11, -1, c_forestGreen,        // barrRight
  0,  12, 15, 14, 13, -1, c_forestGreen,      // barrLeft
  0,  8, 11, 15, 12, -1, c_oliveDrab,         // barrTop
  0,  9, 13, 14, 10, -1, c_darkGreen,         // barrBttm
  INT_MAX};
rgn3 turrBladRgns[TANKHELIRGNS];

double planeShdwPts[]= {
  12,-2,0, 10,-3,0, 4,-3,0, 4,-25,0, -2,-25,0, -5,-3,0, -23,-1.44,0,
  -25,-9,0, -28,-9,0,
  -28,9,0, -25,9,0,
  -23,1.44,0, -5,3,0, -2,25,0, 4,25,0, 4,3,0, 10,3,0, 12,2,0,
  COORD_MAX};
int planeShdwPanels[]= {
  1, 17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,-1, c_gray10,
  INT_MAX};
rgn3 planeShdw(planeShdwPts,planeShdwPanels,0.375);


//---------------------------------------------------------------------------
double planePts[97]=    {12,-2,1, 10,-3,2, 6,-3,4, -28,-1,4,
                         -28,-1,2, -5,-3,-4, -2,-25,0, 4,-25,0,
                         4,-3,-4, 10,-3,-4, 12,-2,-3,
                         12,2,1, 10,3,2, 6,3,4, -28,1,4,
                         -28,1,2, -5,3,-4, -2,25,0, 4,25,0,
                         4,3,-4, 10,3,-4, 12,2,-3,
                         -21,0,4, -23,0,10, -28,0,10, -28,0,4,
                         -28,-9,10, -25,-9,10, -25,9,10, -28,9,10,
                         -5,-3,4, -5,3,4,
                         COORD_MAX};
int planePanels[]= {
  0, 13,31,14,3,30,2,-1,c_blue,      // Top
  0, 9,5,16,20,-1,c_blue,            // Bttm1
  0, 5,4,15,16,-1,c_blue,            // Bttm2
  0, 12,20,16,31,13,-1,c_steelBlue,  // Left1
  0, 31,16,15,14,-1,c_steelBlue,     // Left2
  0, 1,2,30,5,9,-1,c_steelBlue,      // Right1
  0, 30,3,4,5,-1,c_steelBlue,        // Right2
  1, 16,17,18,19,-1,c_blue,          // LWing
  1, 5,6,7,8,-1,c_blue,              // RWing
  0, 13,2,1,0,11,12,-1,c_cyan,       // Windshield
  0, 0,1,9,10,-1,c_steelBlue,        // FrontRight
  0, 12,11,21,20,-1,c_steelBlue,     // FrontLeft
  0, 21,10,9,20,-1,c_blue,           // FrontBttm
  0, 11,0,10,21,-1,c_orangeRed,      // Nose
  1, 22,23,24,25,-1,c_orangeRed,     // VTail
  1, 23,28,29,26,27,-1,c_blue,       // HTail
  0, 3,14,15,4,-1,c_steelBlue,       // Back
  INT_MAX};
rgn3 planeRgn(planePts,planePanels,0.375,&planeShdw);


//---------------------------------------------------------------------------
double treePts[31]=     {0,0,120, 32,-18,32, 0,37,32, -32,-18,32,
                         -11,6,32, 0,-12,32, 11,6,32,
                         -11,6, 0, 0,-12, 0, 11,6, 0, COORD_MAX};
int treeShdwPanels[]= {1, 1,2,3,-1, c_gray10,INT_MAX};
rgn3 treeShdw(treePts,treeShdwPanels,1./8);
int treePanels[]= {
  0, 0,1,2,-1,c_forestGreen,  // Brch1
  0, 0,2,3,-1,c_oliveDrab,    // Brch2
  0, 0,3,1,-1,c_darkGreen,    // Brch3
  0, 3,2,1,-1,c_forestGreen,  // BrchBttm
  0, 4,7,8,5,-1,c_brown,      // Trunk1
  0, 5,8,9,6,-1,c_brown,      // Trunk2
  0, 6,9,7,4,-1,c_brown,      // Trunk3
  0, 9,8,7,-1,c_brown,        // TrunkBttm
  INT_MAX};
rgn3 treeRgn(treePts,treePanels,1./8,&treeShdw);


//---------------------------------------------------------------------------
double mtnPts[]= {
  0,0,64, 128,128,0, -128,128,0,
  -128,-128,0, 128,-128,0,
  COORD_MAX};
int mtnPanels[]= {
  0, 0,1,2,-1,c_saddleBrown,  // Top1
  0, 0,2,3,-1,c_sienna,       // Top2
  0, 0,3,4,-1,c_saddleBrown,  // Top3
  0, 0,4,1,-1,c_sienna,       // Top4
  INT_MAX};
rgn3 mtnRgn(mtnPts,mtnPanels,1); // mtn stands for mountain

double mtn2Pts[]= {
  4,-4,0, 4,4,0, -4,4,0, -4,-4,0,
  2,-2,1.2, 2,2,1.2, -1,1,1.8, -1,-1,1.8,
  COORD_MAX
};
int mtn2Panels[]= {
  0, 0,1,5,4, -1,c_saddleBrown,
  0, 1,2,6,5, -1,c_sienna,
  0, 2,3,7,6, -1,c_saddleBrown,
  0, 3,0,4,7, -1,c_sienna,
  0, 4,5,6,7, -1,c_brown,
  INT_MAX
};
rgn3 mtn2Rgn(mtn2Pts,mtn2Panels,32);

double mtn3Pts[]= {
  0,0,16, -32,-18,0, 32,-18,0, 0,37,0,
  COORD_MAX
};
int mtn3Panels[]= {
  0, 0,1,2, -1,c_saddleBrown,
  0, 0,2,3, -1,c_sienna,
  0, 0,3,1, -1,c_brown,
  INT_MAX
};
rgn3 mtn3Rgn(mtn3Pts,mtn3Panels,pt3d(3.8,3.8,4.7));

double mtn4Pts[]= {
  0,37,0, -32,-18,0, 32,-18,0, -8,-4.5,15, 8,-4.5,15,
  COORD_MAX
};
int mtn4Panels[]= {
  0, 0,1,3, -1,c_saddleBrown,
  0, 1,2,4,3, -1,c_sienna,
  0, 2,0,4, -1,c_saddleBrown,
  0, 0,3,4, -1,c_brown,
  INT_MAX
};
rgn3 mtn4Rgn(mtn4Pts,mtn4Panels,4);
  

//---------------------------------------------------------------------------
double saucShdwPts[]= {
  6,2,0, 8,2,0, 14,3,0, 8,4,0, 7,4,0, 6,8.5,0, 2.5,11,0, 
  -2.5,11,0, -6,8.5,0, -7,4,0, -10,4,0, -10,2,0, -6,2,0, 
  -6,-2,0, -10,-2,0, -10,-4,0, -7,-4,0, -6,-8.5,0, -2.5,-11,0, 
  2.5,-11,0, 6,-8.5,0, 7,-4,0, 8,-4,0, 14,-3,0, 8,-2,0, 6,-2,0, 
  COORD_MAX
};
int saucShdwPanels[]= {
  1, 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,
     -1, c_gray10,
  INT_MAX
};
rgn3 saucerShdw(saucShdwPts,saucShdwPanels,0.5);


//---------------------------------------------------------------------------
double saucPts[]= {
  6,1,0, 2,2,0, -3,2,0, -3,-2,0, 2,-2,0,
  6,-1,0, 2,1,2, -3,1,2, -3,-1,2, 2,-1,2,
  6,2,0, -6,2,0, -6,-2,0, 6,-2,0,
  COORD_MAX};
int saucPanels[]= {
  0, 6,7,8,9,-1,c_gray40,     // canpTop
  0, 0,6,9,5,-1,c_black,      // canpFront
  0, 9,8,3,4,-1,c_gray40,     // canpLeft1
  0, 5,9,4,-1,c_black,        // canpLeft2
  0, 1,2,7,6,-1,c_gray40,     // canpRight1
  0, 0,1,6,-1,c_black,        // canpRight2
  0, 7,2,3,8,-1,c_gray20,     // canpBack
  1, 10,11,12,13,-1,c_darkGreen, // canpBttm
  INT_MAX};
rgn3 saucerRgn(saucPts,saucPanels,0.5,&saucerShdw);


//---------------------------------------------------------------------------
double engnPts[]= {
  14,0,0, 8,1,0, 8,0,1, 8,-1,0, 8,0,-1,
  -10,1,0, -10,0,1, -10,-1,0, -10,0,-1, 
  -5,0,1, -6,0,5, -8,0,5, -8,0,1,
  COORD_MAX};
int engnPanels[]= {
  0, 0,1,2,-1,c_saddleBrown,  // engnFront1
  0, 0,2,3,-1,c_saddleBrown,  // engnFront2
  0, 0,3,4,-1,c_saddleBrown,  // engnFront3
  0, 0,4,1,-1,c_saddleBrown,  // engnFront4
  0, 1,5,6,2,-1,c_saddleBrown,        // engnSide1
  0, 2,6,7,3,-1,c_saddleBrown,        // engnSide2
  0, 3,7,8,4,-1,c_saddleBrown,        // engnSide3
  0, 4,8,5,1,-1,c_saddleBrown,        // engnSide4
  0, 6,5,8,7,-1,c_orange,     // engnBack
  1, 9,10,11,12,-1,c_black,   // tail
  INT_MAX};
rgn3 engineRgn(engnPts,engnPanels,0.5);


//---------------------------------------------------------------------------
double wingPts[19]=     {7,0,0, 6,4.5,0, 2.5,7,0, -2.5,7,0,
                         -6,4.5,0, -7,0,0, COORD_MAX};
int wingPanels[]= {
  1, 0,1,2,3,4,5,-1,c_darkGreen,      // wing
  INT_MAX};
rgn3 lwingRgn(wingPts,wingPanels,0.5);
rgn3 rwingRgn(wingPts,wingPanels,pt3d(0.5,-0.5,0.5));


//---------------------------------------------------------------------------
// Tank "B" definition (tank body, turret, barrel)
double tankBShdwPts[]= {
  -10,-10,0, 10,-10,0, 10,-7,0, 12,-7,0, 12,7,0, 10,7,0, 10,10,0, -10,10,0,
  COORD_MAX};
int tankBShdwPanels[]= {1, 0,1,2,3,4,5,6,7,-1, c_gray10,INT_MAX};
rgn3 tankBShdw(tankBShdwPts,tankBShdwPanels,0.5);

double tankBPts[]=      {-10,-10,4,  -10,-10,1.5, -9,-10,0,   8.5,-10,0,
                         10,-10,3,   9,-10,4,     -10,-7,1.5, -9,-7,0,
                         8.5,-7,0,   9.25,-7,1.5, 10,-7,3,    9,-7,4,
                         -10,10,4,   -10,10,1.5,  -9,10,0,    8.5,10,0,
                         10,10,3,    9,10,4,      -10,7,1.5,  -9,7,0,
                         8.5,7,0,    9.25,7,1.5,  10,7,3,     9,7,4,
                         10.5,7,1.5, 12,7,4,      9,7,6,      -6.5,7,6,
                         -8,5.5,7,   -10,5.5,7,   -10,-5.5,7, -8,-5.5,7,
                         -6.5,-7,6,  9,-7,6,      12,-7,4,    10.5,-7,1.5,
			 -10,7,6,  -10,-7,6,
                         COORD_MAX};
int tankBPanels[]= {
  0, 26,27,32,33,-1,c_oliveDrab,             // Top
  0, 37,36,12,13,1,0,-1,c_saddleBrown,       // Back
  0, 12,17,16,15,14,13,-1,c_steelBlue4,      // Left
  0, 17,12,36,26,-1,c_steelBlue,             // LeftTop
  0, 17,26,23,-1,c_saddleBrown,              // LeftFront1
  0, 17,23,22,16,-1,c_oliveDrab,             // LeftFront2
  0, 16,22,20,15,-1,c_darkGreen,             // LeftFront3
  0, 13,14,19,18,-1,c_gray40,                // LeftBack
  0, 19,14,15,20,-1,c_black,                 // LeftBttm
  0, 18,19,20,21,-1,c_steelBlue4,            // LeftRight
  0, 25,24,21,22,23,26,-1,c_saddleBrown,     // LeftFwd
  0, 0,1,2,3,4,5,-1,c_steelBlue4,            // Right
  0, 33,37,0,5,-1,c_steelBlue,               // RightTop
  0, 33,5,11,-1,c_saddleBrown,               // RightFront1
  0, 11,5,4,10,-1,c_oliveDrab,               // RightFront2
  0, 10,4,3,8,-1,c_darkGreen,                // RightFront3
  0, 6,7,2,1,-1,c_gray40,                    // RightBack
  0, 2,7,8,3,-1,c_black,                     // RightBttm
  0, 9,8,7,6,-1,c_steelBlue4,                // RightLeft
  0, 34,33,11,10,9,35,-1,c_saddleBrown,      // RightFwd
  0, 25,26,33,34,-1,c_oliveDrab,             // FrontTop
  0, 25,34,35,24,-1,c_darkGreen,             // FrontBttm
  0, 6,18,24,35,-1,c_black,                  // Bttm
  INT_MAX};
rgn3 tankBRgn(tankBPts,tankBPanels,0.5,&tankBShdw);


//---------------------------------------------------------------------------
int tankBFinPanels[]= {
  0, 28,29,30,31,-1,c_oliveDrab,             // BackTop
  0, 27,28,31,32,-1,c_darkGreen,             // BackRise
  0, 30,29,36,37,-1,c_saddleBrown,           // Back
  0, 37,32,31,30,-1,c_steelBlue,             // RightTop
  0, 36,29,28,27,-1,c_steelBlue,             // Lefttop
  INT_MAX};
rgn3 tankBFinRgn(tankBPts,tankBFinPanels,0.5);


//---------------------------------------------------------------------------
double turrBPts[31]=    {3,3,1.5, -4,3,1.5, -4,-3,1.5, 3,-3,1.5,
                         3,5,-1.5, -6,5,-1.5, -6,-5,-1.5, 3,-5,-1.5,
                         6,-2.5,-1.5, 6,2.5,-1.5, COORD_MAX};
int turrBPanels[]= {
  0, 0,3,8,9,-1,c_darkGreen,          // Front
  0, 0,1,2,3,-1,c_oliveDrab,          // Top
  0, 1,5,6,2,-1,c_teamSecondary,      // Back
  0, 0,4,5,1,-1,c_teamPrimary,        // Left
  0, 3,2,6,7,-1,c_teamPrimary,        // Right
  0, 0,9,4,-1,c_forestGreen,          // LFront
  0, 3,7,8,-1,c_forestGreen,          // RFront
  0, 9,8,7,6,5,4,-1,c_black,          // Bttm
  INT_MAX};
rgn3 turrBRgn(turrBPts,turrBPanels,0.5);


//---------------------------------------------------------------------------
double barrBShdwPts[]= {
  9,-4,0, 14,-4,0, 14,4,0, 9,4,0, 9,2.5,0, 1,2.5,0, 1,0.5,0, 9,0.5,0,
  9,-0.5,0, 1,-0.5,0, 1,-2.5,0, 9,-2.5,0,
  COORD_MAX};
int barrBShdwPanels[]= {1, 0,1,2,3,4,5,6,7,8,9,10,11,-1, c_gray10,INT_MAX};
rgn3 barrBShdw(barrBShdwPts,barrBShdwPanels,0.25);

double barrBPts[73]= {
  14,4,0, 9,4,0, 9,2.5,1.25, 9,-2.5,1.25,
  9,-4,0, 14,-4,0, 14,-2.5,1.25, 14,2.5,1.25,
  14,2.5,-1.25, 14,-2.5,-1.25, 9,-2.5,-1.25, 9,2.5,-1.25,
  9,1.5,0.75, 9,2.5,-0.75, 0.75,2.5,-0.75, -0.75,1.5,0.75,
  0.75,0.5,-0.75, 9,0.5,-0.75, 9,-1.5,0.75, 9,-0.5,-0.75,
  0.75,-0.5,-0.75, -0.75,-1.5,0.75, 0.75,-2.5,-0.75,
  9,-2.5,-0.75, COORD_MAX};
int barrBPanels[]= {
  0, 2,3,6,7,-1,c_gray60,             // NozzleTop
  0, 0,1,2,7,-1,c_steelBlue,          // NozzleLeftTop
  0, 6,3,4,5,-1,c_steelBlue,          // NozzleRightTop
  0, 8,9,10,11,-1,c_gray20,           // NozzleBttm
  0, 1,0,8,11,-1,c_steelBlue,         // NozzleLeftBttm
  0, 5,4,10,9,-1,c_steelBlue,         // NozzleRightBttm
  0, 0,7,6,5,9,8,-1,c_gray40,         // NozzleFront
  0, 4,3,2,1,11,10,-1,c_darkGreen,    // NozzleBack
  0, 12,13,14,15,-1,c_oliveDrab,     // LeftTop1
  0, 12,15,16,17,-1,c_oliveDrab,     // LeftTop2
  0, 13,17,16,14,-1,c_forestGreen,   // LeftBttm
  0, 18,19,20,21,-1,c_oliveDrab,     // RightTop1
  0, 18,21,22,23,-1,c_oliveDrab,     // RightTop2
  0, 23,22,20,19,-1,c_forestGreen,   // RightBttm
  INT_MAX};
rgn3 barrBRgn(barrBPts,barrBPanels,0.25,&barrBShdw);


//---------------------------------------------------------------------------
double turrCPts[]= {
   3,-2.5,-1.5, 3,-2.5,0.5, 2.5,-2.5,1.5, -2.5,-2.5,1.5, -3,-2.5,0.5,
  -3,-2.5,-1.5,
   3, 2.5,-1.5, 3, 2.5,0.5, 2.5, 2.5,1.5, -2.5, 2.5,1.5, -3, 2.5,0.5,
  -3, 2.5,-1.5,
  COORD_MAX
};
int turrCPanels[]= {
  0, 6,7,1,0,-1,c_gray70,     // front
  0, 5,4,10,11,-1,c_gray70,   // back
  0, 0,1,4,5,-1,c_gray60,     // right
  0, 11,10,7,6,-1,c_gray60,   // left
  0, 7,8,2,1,-1,c_gray80,     // top front
  0, 4,3,9,10,-1,c_gray80,    // top back
  0, 1,2,3,4,-1,c_gray70,     // top right
  0, 10,9,8,7,-1,c_gray70,    // top left
  0, 8,9,3,2,-1,c_gray95,     // top
  INT_MAX
};
rgn3 turrCRgn(turrCPts,turrCPanels,0.6);


//---------------------------------------------------------------------------
double barrCPts[]= {
  1,-1,-1, 1,0,1, 0,0,1, 0,-1,-1, 1,1,-1, 0,1,-1,
  COORD_MAX
};
int barrCShdwPanels[]= {1, 0,3,5,4,-1, c_gray10, INT_MAX};
rgn3 barrCShdw(barrCPts,barrCShdwPanels,pt3d(3.6,0.3,0.24));

int barrCPanels[]= {
  0, 0,1,2,3,-1, c_gray80,
  0, 5,2,1,4,-1, c_gray80,
  0, 4,0,3,5,-1, c_gray50,
  0, 4,1,0,-1,   c_gray30,   // front
  INT_MAX
};
rgn3 barrCRgn(barrCPts,barrCPanels,pt3d(3.6,0.3,0.24),&barrCShdw);


//---------------------------------------------------------------------------
double trainPts[]= {
  10.5,-2.5,0, 11,-3,1, 11,-3,3, 8,-2.4,6,
  -10,-2.4,6, -10,-3,3, -10,-3,1, -10,-2.5,0,
  10.5, 2.5,0, 11, 3,1, 11, 3,3, 8, 2.4,6,
  -10, 2.4,6, -10, 3,3, -10, 3,1, -10, 2.5,0,
  10.1,2,4, 9.1,2,5, 9.1,-2,5, 10.1,-2,4,
  COORD_MAX};
int trainShdwPanels[]= {1, 2,10,13,5,-1, c_gray10,INT_MAX};
rgn3 trainShdw(trainPts,trainShdwPanels,0.7);

int trainPanels[]= {
  0, 0,1,6,7, -1,c_gray60,              // right
  0, 1,2,5,6, -1,c_gray70,
  0, 2,3,4,5, -1,c_gray75,
  0, 8,15,14,9, -1,c_gray60,            // left
  0, 9,14,13,10, -1,c_gray70,
  0, 10,13,12,11, -1,c_gray75,
  0, 0,8,9,1, -1,c_gray70,              // front
  0, 1,9,10,2, -1,c_gray80,
  0, 2,10,11,3, -1,c_gray85,
  0, 16,17,18,19, -1,c_cyan,            // (window)
  0, 3,11,12,4, -1,c_gray95,            // top
  0, 7,6,5,4,12,13,14,15, -1,c_gray50,  // back
  0, 0,7,15,8, -1,c_gray40,             // bottom
  INT_MAX};
rgn3 trainRgn(trainPts,trainPanels,0.7,&trainShdw);


//---------------------------------------------------------------------------
double railShdwPts[]= {
  0,-1,0, 0,-0.95,0, 1,-0.95,0, 1,-1,0,
  0, 1,0, 0, 0.95,0, 1, 0.95,0, 1, 1,0,
  COORD_MAX};
double railPts[]= {
  0,-1,0, 0,-1,0.25, 1,-1,0.25, 1,-1,0,
  0, 1,0, 0, 1,0.25, 1, 1,0.25, 1, 1,0,
  COORD_MAX};
int railPanels[]= {
  1, 0,1,2,3, -1,c_saddleBrown,
  1, 4,7,6,5, -1,c_saddleBrown,
  INT_MAX};
rgn3 railShdw40 (railShdwPts,railPanels,pt3d( 40,1.5,1));
rgn3 railRgn40  (railPts,    railPanels,pt3d( 40,1.5,1),&railShdw40);
rgn3 railShdw260(railShdwPts,railPanels,pt3d(260,1.5,1));
rgn3 railRgn260 (railPts,    railPanels,pt3d(260,1.5,1),&railShdw260);

double rail100Pts[]= {
  7,-1,0, 7,-1,0.25, 93,-1,0.25, 93,-1,0,
  7, 1,0, 7, 1,0.25, 93, 1,0.25, 93, 1,0,
  COORD_MAX};
rgn3 railShdw100(railShdwPts,railPanels,pt3d(100,1.5,1));
rgn3 railRgn100 (rail100Pts, railPanels,pt3d(  1,1.5,1),&railShdw100);


//---------------------------------------------------------------------------
double pillarShdwPts[]= {
  7,-3,15.6, 7,3,15.6, -7,3,15.6, -7,-3,15.6,
  COORD_MAX};
int pillarShdwPanels[]= {1, 0,1,2,3,-1, c_gray10,INT_MAX};
rgn3 pillarShdw(pillarShdwPts,pillarShdwPanels,pt3d(0.5,0.55,0.5));

double pillarPts[]= {
  3,-3,0, 3,-3,12, 7,-3,15.6, -7,-3,15.6, -3,-3,12, -3,-3,0,
  3, 3,0, 3, 3,12, 7, 3,15.6, -7, 3,15.6, -3, 3,12, -3, 3,0,
  COORD_MAX
};
int pillarPanels[]= {
  0, 0,1,2,3,4,5,-1, c_gray70,    // right
  0, 11,10,9,8,7,6,-1, c_gray70,  // left
  0, 6,7,1,0,-1, c_gray60,        // front
  0, 5,4,10,11,-1, c_gray60,      // back
  0, 7,8,2,1,-1, c_gray50,        // front bttm
  0, 4,3,9,10,-1, c_gray50,       // back bttm
  0, 3,2,8,9,-1, c_gray80,        // top
  INT_MAX
};
rgn3 pillarRgn(pillarPts,pillarPanels,pt3d(0.5,0.55,0.5),&pillarShdw);


//---------------------------------------------------------------------------
double bangPts1[]= {0,-2,4, 0,2,4, 0,0,12, COORD_MAX};
double bangPts2[]= {0,-0.7,21, 0,0.7,21, 0,0,25, COORD_MAX};
int    bangPanels[]= {1, 0,1,2,-1, c_orange,INT_MAX};

const int BANGRGNS= 4;                  // num bang rgns
rgn3 bangRgns[BANGRGNS];


//---------------------------------------------------------------------------
double horizonPts[]= {
  120,-120,0, 120,120,0, -16,0,-8, -16,0,8,
  0.3,0.125,4.5, 0.125,0.3,4.5, -0.125,0.3,4.5, -0.3,0.125,4.5, -0.3,-0.125,4.5, -0.125,-0.3,4.5, 0.125,-0.3,4.5, 0.3,-0.125,4.5, 
  COORD_MAX};
int horizonPanels[]= {
  1, 0,1,2,0,-1, c_lawnGreen,
  1, 0,1,3,0,-1, c_lightSkyBlue,
  1, 4,5,6,7,8,9,10,11,-1, c_yellow,
  INT_MAX};
rgn3 horizonRgn(horizonPts,horizonPanels,1);


//---------------------------------------------------------------------------
// Shapes with no initialization data:
const int EXPLSPIKES= 15;
rgn3 explRgn;
rgn3 boundRgn;


#ifdef NONRELEASE
// Experimental stuff, fun stuff, etc. that shouldn't go into a real release:

double destroyerPts[]=  {
  19,0,5, 15,3,4, 9,5,4, -13,5,4, -15,5,2, -18,3,2,
  -18,-3,2, -15,-5,2, -13,-5,4, 9,-5,4, 15,-3,4,
  19,0,0, 15,3,0, 9,5,0, -13,5,2, -15,5,0, -18,3,0,
  -18,-3,0, -15,-5,0, -13,-5,2, 9,-5,0, 15,-3,0,
  8,3,7, 7,4,7, 3,4,7, -3,4,7, -3,-4,7, 3,-4,7, 7,-4,7, 8,-3,7,
  8,3,4, 7,4,4, -3,4,4, -3,-4,4, 7,-4,4, 8,-3,4,
  2,3,9, -3,3,9, -3,-3,9, 2,-3,9,
  -8,3,7, -12,3,7, -12,-3,7, -8,-3,7, -8,3,4, -12,3,4, -12,-3,4, -8,-3,4,
  COORD_MAX};
int destroyerPanels[]= {
  0, 0,11,12,1,-1,c_gray60,           // sides (going clockwise)
  0, 1,12,13,2,-1,c_gray60,           // "
  0, 2,13,15,4,14,3,-1,c_gray60,      // "
  0, 4,15,16,5,-1,c_gray60,           // "
  0, 5,16,17,6,-1,c_gray50,           // " (back side)
  0, 6,17,18,7,-1,c_gray60,           // "
  0, 7,18,20,9,8,19,-1,c_gray60,      // "
  0, 9,20,21,10,-1,c_gray60,          // "
  0, 10,21,11,0,-1,c_gray60,         // "
  0, 0,1,10,-1,c_slateGray,           // prow top
  0, 1,2,3,8,9,10,-1,c_slateGray3,    // main top
  0, 14,4,5,6,7,19,-1,c_slateGray3,  // back top
  0, 8,3,14,19,-1,c_gray50,           // back drop
  0, 29,35,30,22,-1,c_gray80,        // bridge sides (going clkwise)
  0, 22,30,31,23,-1,c_gray70,        // "
  0, 23,31,32,25,-1,c_gray60,        // "
  0, 25,32,33,26,38,37,-1,c_gray50,  // " (bridge back)
  0, 26,33,34,28,-1,c_gray60,        // "
  0, 28,34,35,29,-1,c_gray70,        // "
  0, 22,23,24,27,28,29,-1,c_slateGray, // front bridge top
  0, 36,24,25,37,-1,c_gray30,        // rear bridge sides
  0, 38,26,27,39,-1,c_gray30,        // "
  0, 39,27,24,36,-1,c_gray30,        // "
  0, 36,37,38,39,-1,c_slateGray,     // rear bridge top
  0, 43,47,44,40,-1,c_gray80,        // rear tower sides
  0, 40,44,45,41,-1,c_gray60,        // "
  0, 41,45,46,42,-1,c_gray50,        // " (back side)
  0, 42,46,47,43,-1,c_gray60,        // "
  0, 40,41,42,43,-1,c_slateGray,     // rear tower top
  INT_MAX};
rgn3 destroyerRgn(destroyerPts,destroyerPanels,4);


double wheelPts[]= {
  0,0,-0.5, -0.75,0,-1.25, 0,0,-2, 0.75,0,-1.25,
  COORD_MAX};
int wheelPanels[]= {
  1, 0,1,2,3,0,-1, c_gray10,
  INT_MAX};
rgn3 wheelRgn(wheelPts,wheelPanels,0.5);


double tunnelPts[]=     {0,0,8, 8,-8,0, 8,8,0, -8,8,0, -8,-8,0,
                         -8,1.5,0, -5,0,3, -3,1,5, -3,-1,5,
                          8,1.5,0,  5,0,3,  3,1,5,  3,-1,5,
                         0,0,0.5, 0,1,3.5, 0,-1,3.5,
                         3,-3,5, 3,3,5, -3,3,5, -3,-3,5, COORD_MAX};

int tunnelPanels[]= {
  0, 0,16,17,0,-1, c_saddleBrown,       // front
  0, 1,9,12,16,1,-1, c_saddleBrown,     // front
  0, 9,2,17,11,10,9,-1, c_saddleBrown,  // front
  0, 0,2,3,0,-1, c_sienna,              // left
  0, 0,18,19,0,-1, c_saddleBrown,       // back
  0, 5,4,19,8,5,-1, c_saddleBrown,      // back
  0, 3,5,6,7,18,3,-1, c_saddleBrown,    // back
  0, 0,4,1,0,-1, c_sienna,              // right
  0, 14,11,12,15,14,-1, c_brown,        // front roof
  0, 10,11,14,13,10,-1, c_firebrick,    // front floor
  0, 10,13,15,12,10,-1, c_sienna,       // front floor
  0, 7,14,15,8,7,-1, c_brown,           // back roof
  0, 13,14,7,6,13,-1, c_sienna,         // back floor
  0, 15,13,6,8,15,-1, c_firebrick,      // back floor
  0, 4,3,2,1,4,-1, c_gray40,            // bttm
  INT_MAX};


double mntn2Pts[]= {
  0,0,9, 4,4,5, -4,4,5, -4,-4,5, 4,-4,5, 1,1,2, -1,1,2, -1,-1,2, 1,-1,2, 3,3,0,
-3,3,0, -3,-3,0, 3,-3,0,
  COORD_MAX};
int mntn2Panels[]= {
  0, 0,1,2,-1,c_saddleBrown,          // mntn top
  0, 0,2,3,-1,c_sienna,
  0, 0,3,4,-1,c_saddleBrown,
  0, 0,4,1,-1,c_sienna,
  0, 2,1,5,6,2,-1, c_brown,             // mntn underside
  0, 3,2,6,7,3,-1, c_saddleBrown,
  0, 4,3,7,8,4,-1, c_brown,
  0, 1,4,8,5,1,-1, c_saddleBrown,
  0, 6,5,9,10,-1,c_saddleBrown,       // mntn base
  0, 7,6,10,11,7,-1, c_sienna,
  0, 8,7,11,12,-1,c_saddleBrown,
  0, 5,8,12,9,5,-1, c_sienna,
  0, 4,3,2,1,-1,c_gray40,     // mtnBttm
  INT_MAX};
rgn3 mntn2Rgn(mntn2Pts,mntn2Panels,32);


double vtolPts[]= {
  22,-1,-1, 12,-3,3, -10,-3,3, -22,-1.8,3, -8,-3,-3, 13,-3,-3, // right fus.
  22,1,-1,  12,3,3,  -10,3,3,  -22,1.8,3,  -8,3,-3,  13,3,-3,  // left fus.
  5.5,-3,2, -4,-3,2, -4,-9,2, 4,-9,2,                     // right wing
  -15,-2.5,3, -21,-1.9,3, -22,-3,3, -22,-10,3, -19,-10,3, // right stabilizer
  5.5,3,2,  -4,3,2,  -4,9,2,  4,9,2,                      // left wing
  -15,2.5,3, -21,1.9,3, -22,3,3, -22,10,3, -19,10,3,      // left stabilizer
  -15,0,3, -19,0,10, -23,0,10, -22,0,3,                   // rudder
  COORD_MAX};
int vtolPanels[]= {
  0, 0,6,7,1,0,-1, c_cyan,                      // nose
  0, 0,1,5,0,-1, c_lightSkyBlue,
  0, 6,11,7,6,-1, c_lightSkyBlue,
  0, 6,0,5,11,6,-1, c_deepSkyBlue,
  0, 7,8,9,3,2,1,7,-1, c_steelBlue1,            // fuselage
  0, 1,2,4,5,1,-1, c_steelBlue2,
  0, 11,5,4,10,11,-1, c_steelBlue4,
  0, 7,11,10,8,7,-1, c_steelBlue2,
  0, 2,3,4,2,-1, c_steelBlue2,                  // (rear fuselage)
  0, 10,9,8,10,-1, c_steelBlue2,
  0, 4,3,9,10,4,-1, c_steelBlue3,
  1, 12,13,14,15,12,-1, c_gray80,               // r wing
  1, 24,23,22,21,24,-1, c_gray80,               // l wing
  1, 16,16,17,18,19,20,16,-1, c_gray80,         // r stabilizer
  1, 29,28,27,26,25,29,-1, c_gray80,            // l stabilizer
  1, 30,31,32,33,30,-1, c_gray80,               // rudder
  INT_MAX};
rgn3 vtolRgn(vtolPts,vtolPanels,0.333);


double rpelPts[]= {    // "Rotatable Propeller Engine Left"
  6,-2,2, -4,-2,2, -7,-1.2,2, -4,-2,-2, 6,-2,-2,
  6, 2,2, -4, 2,2, -7, 1.2,2, -4, 2,-2, 6, 2,-2,
  3, 2,0, 0, 10,0, -4, 10,0, -4, 2,0,
  COORD_MAX};
int rpelPanels[]= {
  0, 5,6,7,2,1,0,5,-1, c_steelBlue1, // top
  0, 0,1,3,4,0,-1, c_steelBlue2,
  0, 5,9,8,6,5,-1, c_steelBlue2,
  0, 4,3,8,9,4,-1, c_steelBlue4,  // bottom
  0, 0,4,9,5,0,-1, c_steelBlue2,  // front
  0, 1,2,3,1,-1, c_steelBlue2,
  0, 6,8,7,6,-1, c_steelBlue2,
  0, 2,7,8,3,2,-1, c_steelBlue3, // back
  1, 10,11,12,13,-1, c_gray80,   // wing tip
  INT_MAX};
rgn3 rpelRgn(rpelPts,rpelPanels,0.333);
rgn3 rperRgn(rpelPts,rpelPanels,pt3d(1,-1,1)*0.333);


double prp3Pts[]= {    // 3-tipped propeller
  0,0,0,
  0,-0.6,-6,      0,0.6,-6,
  0, 5.496,2.480, 0, 4.896,3.520,
  0,-5.496,2.480, 0,-4.896,3.520,
  COORD_MAX};
int prp3Panels[]= {
  1, 0,1,2,0,-1, c_white,
  1, 0,3,4,0,-1, c_white,
  1, 0,5,6,0,-1, c_white,
  INT_MAX};
rgn3 prp3Rgn(prp3Pts,prp3Panels,0.333);


#endif



/************************************************************************/
double arrowPts[]=      {0,-3, 22,-3, 22,3, 22,-3, 26,-3, 26,3, 0,3,
			 COORD_MAX};
double turr2dPts[]=     {-1,-4, -4,-4, -4,4, 4,4, 4,-4, -1,-4,
                         -1,-8, 1,-8, 1,-4, COORD_MAX};
double heli2dPts[]=     {0,-7, -4,-4, -16,-4, -20,-8, -20,0, -4,3,
                         0,8, 8,8, 12,2, 8,-7, 16,-7, -12,-7,
                         COORD_MAX};
double plne2dPts[]=     {-2,-4, 6,-4, 8,-3, 8,-1, 4,1, -6,1, -8,4, -10,4,
                         -10,-2, -2,-4, 0,-1, 4,-1, 4,-4,
                         COORD_MAX};
double sauc2dPts[]=     {4,0, -10,0, -10,-3, 4,-3, 10,-1.5, 4,0, 2,0,
                         0,2, -5,2, -5,0, -6,0, -7.5,4, -9.5,4, -9.5,0,
                         COORD_MAX};

// Insignia
double starPts[]=       {0,6, -3,2, -8,2, -4,-2, -5,-6,
                         0,-3, 5,-6, 4,-2, 8,2, 3,2,
                         COORD_MAX};
double star2Pts[]=      {10,6.5, -10,6.5, -10,-6.5, 10,-6.5, COORD_MAX};

double deltaPts[]=      {0,6, -9,-6, -6,-6, 0,2, 6,-6, 9,-6, COORD_MAX};
double delta2Pts[]=     {0,-2, -3,-6, 3,-6, COORD_MAX};

double boltPts[]=        {-9,-2, 1,-2, 0,0, 9,2, -1,2, 0,0, COORD_MAX};
double bolt2Pts[]=       {0,4.5, -4.5,2, -4.5,-2, 0,-4.5, 4.5,-2, 4.5,2,
			  COORD_MAX};

double stabPts[]=       {0,7, -1,2, -4,2, -6,-1, -5,-7, -4,-1,
                         -3,0, -1,0, 0,-7, 1,0, 3,0, 4,-1, 5,-7,
                         6,-1, 4,2, 1,2, COORD_MAX};
double stab2Pts[]=      {9.5,6.5, -9.5,6.5, -9.5,-6.5, 9.5,-6.5, COORD_MAX};

double flamePts[]=      {0,5, -1,2, -4,3, -2,0, -8,-6, -6,-6,
                         -1,-1, -1,-6, 1,-6, 1,-1, 6,-6, 8,-6,
                         2,0, 4,3, 1,2, COORD_MAX};
double flame2Pts[]=     {0,7, -8,-6, 8,-6, COORD_MAX};

double zeroPts[]=        {-3,2, -5,0, -3,-2, 5,0, COORD_MAX};
double zero2Pts[]=       {-4,6, -8,0, -4,-6, 4,-6, 8,0, 4,6, COORD_MAX};


double capitalBPts[]= {
  -7,10, -6,9, -6,-9, -7,-10, 2,-10, 6,-8, 6,-2, 2,0, 6,2,
  6,8, 2,10, COORD_MAX};

double decorPts[]= {
  0,3, -12,0, 0,-3, COORD_MAX};


double *insigniaPts[]=
{ starPts, deltaPts, boltPts, stabPts, flamePts, zeroPts};

// These 2nd parts of insignia will appear underneath the first parts, if
// the two parts overlap
double *insignia2Pts[]=
{ star2Pts, delta2Pts, bolt2Pts, stab2Pts, flame2Pts, zero2Pts};


/////////////////////////////////////////////////////////////////////////////
rgn2
  turr2dRgn(turr2dPts,2),
  heli2dRgn(heli2dPts),
  capitalBRgn(capitalBPts,0.25),
  decorRgn(decorPts,0.25),
  plne2dRgn(plne2dPts,2),
  sauc2dRgn(sauc2dPts,2),
  arrowRgn(arrowPts),
  bound2dRgn,
  insigniaRgns[NUMINSIGNIA],
  insignia2Rgns[NUMINSIGNIA];


/////////////////////////////////////////////////////////////////////////////
/*-------------------------------------------------------------------------*/
void InitExplRegion(rgn3 *rgn, int spikes) {
  pt3d explPts[EXPLSPIKES+1];
  table<int> explPanels;

  explPts[0]= pt3d(0,0,0);              // center of explosion
  for(int i=1; i<spikes; i++) {
    ang3d ang(rand()%(2*MA_PI),rand()%MA_PI,0);
    if (ang.xz <MA_PI/2) ang.xz= (ang2d)(( 1-cosTbl[ang.xz])*MA_PI/2);
    else                 ang.xz= (ang2d)((-1-cosTbl[ang.xz])*MA_PI/2);

    explPts[i]= pt3d(0.5,0,0) >> ang;
    explPanels.Add(1);                  // spikes panels are "double-sided"
    explPanels.Add(0);
    explPanels.Add(i);  // what if region3d collapses duplicate pts?  Then
                        // this pt num will be wrong! -PAH
    explPanels.Add(-1);
    explPanels.Add(c_red);

    // add a mirror spike, which is symmetric to the one above:
    i++;
    explPts[i]= explPts[i-1];
    explPts[i].y= -explPts[i].y;
    explPanels.Add(1);
    explPanels.Add(0);
    explPanels.Add(i);
    explPanels.Add(-1);
    explPanels.Add(c_red);
  }
  explPts[spikes].x= COORD_MAX;
  explPanels.Add(INT_MAX);
  *rgn= rgn3(explPts,explPanels.Array());
}


/*-------------------------------------------------------------------------*/
void InitPlayfieldBounds(rgn3 *rgn, rgn2 *rgn2d, int numSides, double hqDist) {

  // create the perimeter points of the playfield
  double xscale= 1;
  if (numSides <3) {
    numSides= 4;
    xscale= 0.5;
  }
  pt3d *pts= new pt3d[numSides+1];
  pt3d *endPt= RegularPolygon(numSides,hqDist,pts);
  forii(numSides) pts[i].x *= xscale;
  *endPt= pt3d(COORD_MAX,0,0);

  // create panel initialization data
  table<int> panels;
  panels.Add(0);  // single-sided panel
  fori(numSides) panels.Add(i);
  panels.Add(-1);
  panels.Add(c_gray90);
  panels.Add(INT_MAX);

  // create the 3d region
  *rgn= rgn3(pts,panels.Array());

  // create the 2d region
  pt2d *pt2ds= new pt2d[numSides+1];
  fori(numSides+1) {
    pt2ds[i].x= pts[i].x;
    pt2ds[i].y= pts[i].y;
  }
  rgn2d->Init(pt2ds,1);

  delete [] pts;
  delete [] pt2ds;
}


/*-------------------------------------------------------------------------*/
// assumes numTeams < NUMINSIGNIA
void InitRegions(int numTeams, bool fancy, double hqDist,
		 int insigniaRandomizer) {
  InitPlayfieldBounds(&boundRgn,&bound2dRgn,numTeams,hqDist);
  InitExplRegion(&explRgn,EXPLSPIKES);
  InitMorphRegion3ds(tankHeliShdws,TANKHELIRGNS,tankXfrmShdwPts,0.5,
                     heliXfrmShdwPts,0.5,tankHeliShdwPanels,false,false);
  InitMorphRegion3ds(tankHeliRgns,TANKHELIRGNS,tankXfrmPts,0.5,
                     heliXfrmPts,1./16,tankHeliPanels,false,false);
  InitMorphRegion3ds(turrBladRgns,TANKHELIRGNS,turrXfrmPts,0.5,
                     bladXfrmPts,0.25,turrBladPanels,false,false);
  InitMorphRegion3ds(bangRgns,BANGRGNS,bangPts1,0.09,
                     bangPts2,0.1,bangPanels,true,true);

  forii(TANKHELIRGNS)
    tankHeliRgns[i].Shadow()= &tankHeliShdws[i];
  fori(BANGRGNS) {
    rgn3 tempRgn= bangRgns[i];
    bangRgns[i].Add(tempRgn,pt3d(0,0,0),ang3d(0,0,2*MA_PI/3));
    bangRgns[i].Add(tempRgn,pt3d(0,0,0),ang3d(0,0,4*MA_PI/3));
  }

  saucerRgn.Add(engineRgn,pt3d(0, 1.5,0));
  saucerRgn.Add(engineRgn,pt3d(0,-1.5,0));
  saucerRgn.Add(lwingRgn, pt3d(0, 2,0));
  saucerRgn.Add(lwingRgn, pt3d(0,-2,0),ang3d(0),pt3d(1,-1,1));

  ang3d ang= ang3d(MA_PI/2,0,MA_PI/2);
  fori(numTeams) {
    int j= (i +insigniaRandomizer) %NUMINSIGNIA;
    hqRgns[i]= rgn3(hqPts,hqPanels,2,&hqShdw);
    insigniaRgns[i].Init(insigniaPts[j],0.5);
    insignia2Rgns[i].Init(insignia2Pts[j],0.5);
    hqRgns[i].Add(xpanel3d(insignia2Rgns[i],hqRgns[i].pts,
			   pt3d(0.001,0,19),ang,pt3d(1,1,1),c_teamSecondary));
    hqRgns[i].Add(xpanel3d(insigniaRgns[i],hqRgns[i].pts,
			   pt3d(0.002,0,19),ang,pt3d(1,1,1),c_teamPrimary));
  }

  if (fancy) {
    boundRgn.Add(xpanel3d(capitalBRgn,boundRgn.pts,
                          pt3d(-10, 2.5,0),0,pt3d(5,5,5),c_red));
    boundRgn.Add(xpanel3d(capitalBRgn,boundRgn.pts,
                          pt3d( 10,-2.5,0),0,pt3d(5,5,5),c_red));
    boundRgn.Add(xpanel3d(decorRgn,boundRgn.pts,
                          pt3d(-22.5,0,0),0,pt3d(5,5,5),c_red));
    boundRgn.Add(xpanel3d(decorRgn,boundRgn.pts,
                          pt3d( 22.5,0,0),MA_PI,pt3d(5,5,5),c_red));
  }
}
