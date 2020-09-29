#pragma once

#include <physics_types.h>

class PhysicsState {
  public:
    PhysicsState() {};
    virtual ~PhysicsState() {};
    virtual void step() = 0;
    virtual void visualize() = 0;
    virtual CollisionShape createBox(real x, real y, real z) = 0;
    virtual void addRigidBody(RigidBody *rb) = 0;
};
