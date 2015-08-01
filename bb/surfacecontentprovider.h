#ifndef SURFACECONTENTPROVIDER_h
#define SURFACECONTENTPROVIDER_h

struct gfxTarget;

class SurfaceContentProvider
{
 public:
  virtual void render(struct gfxTarget* gfxtarget) = 0;
};

#endif // #ifndef SURFACECONTENTPROVIDER_h

