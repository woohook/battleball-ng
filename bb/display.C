#include "display.h"
#include "surface.h"

Displayy::Displayy()
: m_GfxTarget(0)
, m_Surface(0)
, m_Surface2(0)
{
  m_Surface = new Surface(this);
  m_Surface2 = new Surface(this);
}

Displayy::~Displayy()
{
  delete m_Surface;
  delete m_Surface2;
}

void Displayy::render()
{
  m_Surface->render();
  m_Surface2->render();
}

