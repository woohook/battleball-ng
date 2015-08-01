#ifndef DISPLAY_h
#define DISPLAY_h

class Surface;
struct gfxTarget;

class Displayy
{
 public:
  Displayy();
  virtual ~Displayy();

  virtual void render();

  struct gfxTarget* m_GfxTarget;
  Surface* m_Surface;
  Surface* m_Surface2;
};

#endif // #ifndef DISPLAY_h

