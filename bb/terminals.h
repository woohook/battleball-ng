#ifndef TERMINALS_h
#define TERMINALS_h

class Terminal;

class Terminals
{
 public:
  Terminals();
  virtual ~Terminals();

  virtual Terminal* create();
  virtual void processInput();
  virtual void processOutput();
};

#endif // #ifndef TERMINALS_h

