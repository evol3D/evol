#pragma once

#include "PhysicsState.h"
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"


class BulletState: public PhysicsState 
{
  private:
    btCollisionConfiguration * collisionConfiguration;
    btCollisionDispatcher * collisionDispatcher;
    btBroadphaseInterface * broadphase;
    btSequentialImpulseConstraintSolver * constraintSolver;
    btDynamicsWorld * world;
    bool visualDebugging = false;

    void clearCollisionObjects();
  public:
    BulletState();
    ~BulletState();
    void step();
    void visualize();
    CollisionShape createBox(real x, real y, real z);
    void addRigidBody(RigidBody *rb);
};
