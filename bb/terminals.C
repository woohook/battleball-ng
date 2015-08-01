#include "terminals.h"
#include "terminal.h"
#include <vector>

std::vector<Terminal*> g_terminals;

Terminals::Terminals()
{
}

Terminals::~Terminals()
{
  std::vector<Terminal*>::iterator currentTerminal = g_terminals.begin();
  while(currentTerminal != g_terminals.end())
  {
    delete *currentTerminal;
    currentTerminal++;
  }
}

Terminal* Terminals::create()
{
  Terminal* newTerminal = new Terminal();
  g_terminals.push_back(newTerminal);
  return newTerminal;
}

void Terminals::processInput()
{
  std::vector<Terminal*>::iterator currentTerminal = g_terminals.begin();
  while(currentTerminal != g_terminals.end())
  {
    (*currentTerminal)->processInput();
    currentTerminal++;
  }
}

void Terminals::processOutput()
{
  std::vector<Terminal*>::iterator currentTerminal = g_terminals.begin();
  while(currentTerminal != g_terminals.end())
  {
    (*currentTerminal)->processOutput();
    currentTerminal++;
  }
}

