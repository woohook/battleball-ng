#include "surface.h"
#include "display.h"
#include "surfacecontentprovider.h"

Surface::Surface(Displayy* display)
{
  m_Display = display;
  m_ContentProvider = 0;
}

Surface::~Surface()
{
}

void Surface::render()
{
  if(m_ContentProvider != 0)
  {
    m_ContentProvider->render(m_Display->m_GfxTarget);
  }
}

