#include "terminal.h"
#include "player.h"
#include "display.h"

Terminal::Terminal()
: m_Display(new Displayy())
, thePlayer(NULL)
{
}

Terminal::~Terminal()
{
  delete m_Display;
}

void Terminal::processInput()
{
  thePlayer->HandleEvents(*gob::gobs);
}

void Terminal::processOutput()
{
  m_Display->render();
}

