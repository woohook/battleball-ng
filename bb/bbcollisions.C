#include "bbcollisions.h"
#include <stdio.h>
#include "region3d.h"
#include "gobs.h"
#include "battleballgame.h"

Collidables* g_Collidables = new BBCollidables();

BBCollidables::BBCollidables()
: m_CollisionHandler(NULL)
{
}

BBCollidables::~BBCollidables()
{
}

void BBCollidables::addCollider(Collidable* collider)
{
  m_colliders.push_back(collider);
}

void BBCollidables::removeCollider(Collidable* collider)
{
  m_removed_collidables.push_back(collider);
}

void BBCollidables::addCollidable(Collidable* collidable)
{
  m_collidables.push_back(collidable);
}

void BBCollidables::removeCollidable(Collidable* collidable)
{
  m_removed_collidables.push_back(collidable);
}

void BBCollidables::setCollisionHandler(CollisionHandler* collisionhandler)
{
  m_CollisionHandler = collisionhandler;
}

void BBCollidables::detectCollisions()
{
  // clear removed collidables first
  std::vector<Collidable*>::iterator removedCollidable = m_removed_collidables.begin();
  while(removedCollidable != m_removed_collidables.end())
  {
    // clear colliders
    std::vector<Collidable*>::iterator currentCollidable = m_colliders.begin();
    while(currentCollidable != m_colliders.end())
    {
      if((*removedCollidable) == (*currentCollidable))
      {
        m_colliders.erase(currentCollidable);
      }
      else
      {
        currentCollidable++;
      }
    }

    // clear collidables
    currentCollidable = m_collidables.begin();
    while(currentCollidable != m_collidables.end())
    {
      if((*removedCollidable) == (*currentCollidable))
      {
        m_collidables.erase(currentCollidable);
      }
      else
      {
        currentCollidable++;
      }
    }
    delete *removedCollidable;
    m_removed_collidables.erase(removedCollidable);
  }

  if(m_CollisionHandler != NULL)
  {
    std::vector<Collidable*>::iterator currentCollider = m_colliders.begin();
    while(currentCollider != m_colliders.end())
    {
      // check collisions with other colliders
      std::vector<Collidable*>::iterator currentCollidable = currentCollider;
      currentCollidable++;
      while(currentCollidable != m_colliders.end())
      {
        if( (*currentCollider)->getShape()->isOverlappedBy((*currentCollidable)->getShape()))
        {
          m_CollisionHandler->handleCollision(*currentCollider, *currentCollidable);
        }
        currentCollidable++;
      }

      // check collisions with collidables
      currentCollidable = m_collidables.begin();
      while(currentCollidable != m_collidables.end())
      {
        if( (*currentCollider)->getShape()->isOverlappedBy((*currentCollidable)->getShape()))
        {
          m_CollisionHandler->handleCollision(*currentCollider, *currentCollidable);
        }
        currentCollidable++;
      }
      currentCollider++;
    }
  }
}

bool CollisionShape::isOverlappedBy(CollisionShape* otherShape)
{
  bool isOverlapped = false;
  double x,y,z,offsetx,offsety,offsetz,lengthx,lengthy,lengthz;

  getCollisionBox(x,y,z,lengthx,lengthy,lengthz);
  x += (lengthx/2.0);
  y += (lengthy/2.0);
  z += (lengthz/2.0);
  otherShape->getCollisionBox(offsetx,offsety,offsetz,lengthx,lengthy,lengthz);

  if( (offsetx <= x) && (x <= offsetx+lengthx)
  &&  (offsety <= y) && (y <= offsety+lengthy)
  &&  (offsetz <= z) && (z <= offsetz+lengthz) )
  {
    isOverlapped = true;
  }
  else
  {
    otherShape->getCollisionBox(x,y,z,lengthx,lengthy,lengthz);
    x += (lengthx/2.0);
    y += (lengthy/2.0);
    z += (lengthz/2.0);
    getCollisionBox(offsetx,offsety,offsetz,lengthx,lengthy,lengthz);

    if( (offsetx <= x) && (x <= offsetx+lengthx)
    &&  (offsety <= y) && (y <= offsety+lengthy)
    &&  (offsetz <= z) && (z <= offsetz+lengthz) )
    {
      isOverlapped = true;
    }
  }

  return isOverlapped;
}

CollidableColliderGob::~CollidableColliderGob()
{
}

void CollidableColliderGob::getCollisionBox(double& offsetx, double& offsety, double& offsetz, double& lengthx, double& lengthy, double& lengthz)
{
  rect3d bounds = m_gob->Shape()->Box();
  pt3d   offset = m_gob->WorldPos().Cart() + bounds.low;
  pt3d   length = bounds.high - bounds.low;
  offsetx = offset.x;
  offsety = offset.y;
  offsetz = offset.z;
  lengthx = length.x;
  lengthy = length.y;
  lengthz = length.z;
}

void* CollidableColliderGob::getType()
{
  return m_type;
}

CollisionShape* CollidableColliderGob::getShape()
{
  return this;
}
