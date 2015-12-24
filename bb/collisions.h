#ifndef COLLISIONS_h
#define COLLISIONS_h

class CollisionShape
{
 public:
  virtual bool isOverlappedBy(CollisionShape* otherShape);
  virtual void getCollisionBox(double& offsetx, double& offsety, double& offsetz, double& lengthx, double& lengthy, double& lengthz) = 0;
};

class Collidable
{
 public:
  virtual void* getType() = 0;
  virtual CollisionShape* getShape() = 0;
};

class CollisionHandler
{
 public:
  virtual void handleCollision(Collidable* collider, Collidable* collidable) = 0;
};

class Collidables
{
 public:
  virtual void addCollider(Collidable* collider) = 0;
  virtual void removeCollider(Collidable* collider) = 0;
  virtual void addCollidable(Collidable* collidable) = 0;
  virtual void removeCollidable(Collidable* collidable) = 0;
  virtual void setCollisionHandler(CollisionHandler* collisionhandler) = 0;
  virtual void detectCollisions() = 0;
};

extern Collidables* g_Collidables;

#endif // #ifndef COLLISIONS_h
