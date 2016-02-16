#include "train.h"
#include <string.h>          // for strcmp()
#include "gobs.h"            // for types of spawnable gobs
#include "battleballgame.h"  // for gobs spawning

gobList track;
gobList* g_track = &track;
tranGob* g_train = NULL;

Train::Train()
{
}

Train::~Train()
{
}

/*-------------------------------------------------------------------------*/
// Create the game objects for the train track

void InitTrack()
{
  double trackData[]= {
    40,0,0, 40,0,0.063, 100,0,0, 40,0,-0.063, 40,0,0,
    40,0.166,0, 40,0.333,0, 40,0.5,0, 40,0.666,0, 40,0.833,0,
    259.5,1,0,
    40,1.166,0, 40,1.333,0, 40,1.5,0, 40,1.666,0, 40,1.833,0,
    0
  };

  pt3d pos= pt3d(-129.2,0,0);
  double *data= trackData;
  while (*data != 0) {
    double len= *data++;
    ang3d ang= ang3d((int)(data[0]*MA_PI), (int)(data[1]*MA_PI), 0);
    data += 2;
    gob *rail= g_BattleBallGame->createRail(tcomp(pos,ang),len);
    g_track->push_back(rail);
    g_scenery->push_back(rail);
    pos += pt3d(len,0,0) >> ang;
    if (pos.z <0.001) pos.z= 0;
  }

  g_scenery->push_back(g_BattleBallGame->createPillar(pt3d(-46.4,0,0)));
  g_scenery->push_back(g_BattleBallGame->createPillar(pt3d( 46.4,0,0)));
}

void Train::initialize(int argc, char** argv)
{
  for (int i=1; i <argc; i++)
  {
    char *s= argv[i];
    if(!strcmp(s,"-train"))
    {
      InitTrack();
      g_train = (tranGob*)g_BattleBallGame->createTrain(pt3d(0,0,0),pt3d(0,0,0),-1);
      g_scenery->push_back(g_train);
    }
  }
}

void Train::processFrame(gobList& gobs)
{
}
