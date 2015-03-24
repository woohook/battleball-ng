#!/bin/csh -f

set CC="CC"
set MISC="-c -fast -unroll=5 -ptr.."
set INCL="-I../lib3d -I../bsp -I../stl -I/usr/openwin/include"
set LIBS="-lm -L/usr/openwin/lib -lX11 -lsocket"
setenv LD_LIBRARY_PATH /usr/openwin/lib

cd bsp;      $CC $MISC $INCL *.C
cd ../lib3d; $CC $MISC $INCL *.C
cd ../bb;    $CC $MISC $INCL *.C
cd ..;       $CC lib3d/*.o bsp/*.o bb/*.o $LIBS -o battleball
