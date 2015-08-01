#ifndef HUDRENDERER_h
#define HUDRENDERER_h

#include "surfacecontentprovider.h"

class player;
class roundInfo;
class battleBall;

class HudRenderer : public SurfaceContentProvider
{
 public:
  HudRenderer();
  virtual ~HudRenderer();

  virtual void render(struct gfxTarget* gfxtarget);

  player* thePlayer;
  roundInfo* ri;
  battleBall* bb;
};

#endif // #ifndef HUDRENDERER_h

