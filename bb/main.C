#include "battleball.h"

#ifdef WIN32
#include <windows.h>
#include "gfxtargetwin.h"

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  gfxTargetWin::hInstance = hInstance;
  gfxTargetWin::nCmdShow  = nCmdShow;

  char cmdlinebuf[1000];
  strcpy(cmdlinebuf,lpCmdLine);

  int argc = 1;
  char* argv[100];
  argv[0] = "battleball";

  char* currentChar = cmdlinebuf;
  char* lastArg = currentChar;
  while(*currentChar != '\0')
  {
    if(*currentChar == ' ')
    {
      *currentChar = '\0';
      argv[argc++] = lastArg;
      lastArg = currentChar+1;
    }
    currentChar++;
  }

  if(*lastArg != '\0')
  {
    argv[argc++] = lastArg;
  }
#else
int main (int argc, char *argv[])
{
#endif

  battleBall bb(argc,argv);
  bb.Play();
  return 0;
}

