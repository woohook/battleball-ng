// Copyright (c) 1997 Philip A. Hardin (pahardin@cs.utexas.edu)
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License v2 or later.


#ifndef BBCOLOR_h
#define BBCOLOR_h

#define c_black         0
#define c_gray10        1
#define c_gray20        2
#define c_gray30        3
#define c_gray40        4
#define c_gray50        5
#define c_gray60        6
#define c_gray70        7
#define c_gray75        8
#define c_gray80        9
#define c_gray85        10
#define c_gray90        11
#define c_gray95        12
#define c_white         13
#define c_saddleBrown   14
#define c_sienna        15
#define c_brown         16
#define c_firebrick     17
#define c_red           18
#define c_orangeRed     19
#define c_darkOrange    20
#define c_orange        21
#define c_gold          22
#define c_yellow        23
#define c_greenYellow   24
#define c_green         25
#define c_lawnGreen     26
#define c_limeGreen     27
#define c_green3        28
#define c_forestGreen   29
#define c_darkGreen     30
#define c_darkOliveGreen 31
#define c_oliveDrab     32
#define c_cyan          33
#define c_lightSkyBlue  34
#define c_deepSkyBlue   35
#define c_dodgerBlue    36
#define c_blue          37
#define c_steelBlue1    38
#define c_steelBlue2    39
#define c_steelBlue3    40
#define c_steelBlue     41
#define c_steelBlue4    42
#define c_slateGray1    43
#define c_slateGray2    44
#define c_slateGray3    45
#define c_slateGray     46
#define c_slateGray4    47
#define c_navy          48
#define c_violet        49
#define c_turquoise     50

#define c_teamPrimary   51
#define c_teamSecondary 52

#define fixedColors c_teamPrimary
#define teamColors 2
#define totalColors (fixedColors+teamColors)

extern char *bbColorNames[];

#endif
