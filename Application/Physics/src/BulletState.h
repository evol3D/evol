#pragma once

#include "PhysicsState.h"
#include "btBulletDynamicsCommon.h"
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
    void visualize();
};
