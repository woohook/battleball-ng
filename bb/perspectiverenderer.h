#ifndef PERSPECTIVERENDERER_h
#define PERSPECTIVERENDERER_h

#include "surfacecontentprovider.h"

class player;
class team;
class hrznGob;

class PerspectiveRenderer : public SurfaceContentProvider
{
 public:
  PerspectiveRenderer();
  virtual ~PerspectiveRenderer();

  virtual void render(struct gfxTarget* gfxtarget);

  player* thePlayer;
  int   playerIndex;
  team* teams;
  int   teamIndex;
  hrznGob** horizon;
  bool* doubleBuffer;
};

#endif // #ifndef PERSPECTIVERENDERER_h

