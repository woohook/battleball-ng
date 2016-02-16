#ifndef TERMINAL_h
#define TERMINAL_h

class Displayy;
class player;
class tranGob;

class Terminal
{
 public:
  Terminal();
  virtual ~Terminal();

  virtual void processInput();
  virtual void processOutput();

  Displayy* m_Display;

  player* thePlayer;
};

#endif // #ifndef TERMINAL_h

