#pragma once

#include <physics_types.h>

class PhysicsState {
  public:
    PhysicsState() {};
    virtual ~PhysicsState() {};
    virtual void step() = 0;
    virtual void step_dt(real dt) = 0;
    virtual void visualize() = 0;
    virtual void setGravity(real x, real y, real z) = 0;
    virtual RigidBodyHandle addRigidBody(RigidBody *rb) = 0;
    virtual void removeRigidBody(RigidBodyHandle handle) = 0;
    virtual void updateRigidBody(RigidBodyHandle handle, RigidBody *rb) = 0;
    virtual CollisionShape createBox(real x, real y, real z) = 0;
    virtual CollisionShape createSphere(real r) = 0;
    virtual CollisionShape createStaticFromTriangleIndexVertex(int numTriangles, int *triangleIndexBase, int triangleIndexStride, int numVertices, real *vertexBase, int vertexStride) = 0;
};
