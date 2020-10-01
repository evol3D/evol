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
    void step_dt(real dt);
    void visualize();
    CollisionShape createBox(real x, real y, real z);
    CollisionShape createSphere(real r);
    void setGravity(real x, real y, real z);
    RigidBodyHandle addRigidBody(RigidBody *rb);
    void removeRigidBody(RigidBodyHandle handle);
    void updateRigidBody(RigidBodyHandle handle, RigidBody *rb);
};
