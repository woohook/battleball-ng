#ifndef BBCOLLISIONS_h
#define BBCOLLISIONS_h

#include "collisions.h"
#include <vector>

class gob;

class CollidableColliderGob : public Collidable, public CollisionShape
{
 public:
  virtual ~CollidableColliderGob();
  virtual void* getType();
  virtual CollisionShape* getShape();
  virtual void getCollisionBox(double& offsetx, double& offsety, double& offsetz, double& lengthx, double& lengthy, double& lengthz);

  gob* m_gob;
  void* m_type;
};

class BBCollidables : public Collidables
{
 public:
  BBCollidables();
  virtual ~BBCollidables();
  virtual void addCollider(Collidable* collider);
  virtual void removeCollider(Collidable* collider);
  virtual void addCollidable(Collidable* collidable);
  virtual void removeCollidable(Collidable* collidable);
  virtual void setCollisionHandler(CollisionHandler* collisionhandler);
  virtual void detectCollisions();

 private:
  CollisionHandler*         m_CollisionHandler;
  std::vector<Collidable*> m_colliders;
  std::vector<Collidable*> m_collidables;
  std::vector<Collidable*> m_removed_collidables;
};

#endif // #ifndef BBCOLLISIONS_h

