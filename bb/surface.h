#ifndef SURFACE_h
#define SURFACE_h

class Displayy;
class SurfaceContentProvider;

class Surface
{
 public:
  Surface(Displayy* display);
  virtual ~Surface();

  virtual void render();

  SurfaceContentProvider* m_ContentProvider;

 protected:
  Displayy* m_Display;
};

#endif // #ifndef SURFACE_h

