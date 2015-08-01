#include "hudrenderer.h"
#include "player.h"
#include "battleball.h"

HudRenderer::HudRenderer()
{
}

HudRenderer::~HudRenderer()
{
}

void HudRenderer::render(struct gfxTarget* gfxtarget)
{
  if (ri->state==counting)
  {
    bb->DrawStartingMsg(thePlayer->gt, ri->timer);
  }
  bb->DrawTextArea(*ri, *thePlayer);
  thePlayer->gt.Flush();
}

