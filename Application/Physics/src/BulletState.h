#pragma once

#include "PhysicsState.h"
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "physics_utils.h"
#include <mutex>


class BulletState: public PhysicsState 
{
  private:
    btCollisionConfiguration * collisionConfiguration;
    btCollisionDispatcher * collisionDispatcher;
    btBroadphaseInterface * broadphase;
    btSequentialImpulseConstraintSolver * constraintSolver;
    btDynamicsWorld * world;
    std::mutex world_mutex;
    std::mutex collision_shapes_mutex;
    bool visualDebugging = false;

    btAlignedObjectArray<btCollisionShape*> collisionShapes;

    void clearCollisionObjects();
    void clearCollisionShapes();
  public:
    BulletState();
    ~BulletState();
    void step();
    void step_dt(real dt);
    void visualize();
    CollisionShape createBox(real x, real y, real z);
    CollisionShape createSphere(real r);
    CollisionShape createCapsule(real radius, real height);
    void setGravity(real x, real y, real z);
    RigidBodyHandle addRigidBody(RigidBody *rb);
    void removeRigidBody(RigidBodyHandle handle);
    void updateRigidBody(RigidBodyHandle handle, RigidBody *rb);
    CollisionShape createStaticFromTriangleIndexVertex(int numTriangles, int *triangleIndexBase, int triangleIndexStride, int numVertices, real *vertexBase, int vertexStride);
    CollisionShape generateConvexHull(int vertexCount, ev_Vector3* vertices);
    void setLinearVelocity(RigidBodyHandle handle, ev_Vector3 *velocity);
    void getLinearVelocity(RigidBodyHandle handle, ev_Vector3 *velocity);
    void setAngularVelocity(RigidBodyHandle handle, ev_Vector3 *velocity);
    void getAngularVelocity(RigidBodyHandle handle, ev_Vector3 *velocity);
    void applyForce(RigidBodyHandle handle, ev_Vector3 *force);
    void setFriction(RigidBodyHandle handle, real frict);
    void setDamping(RigidBodyHandle handle, real linearDamping, real angularDamping);
    void addHingeConstraint(RigidBodyHandle parentHandle, RigidBodyHandle childHandle, ev_Vector3 *anchor, ev_Vector3 *parentAxis, ev_Vector3 *childAxis);
    CollisionShape createCylinderX(real width, real radiusX, real radiusY);
    void setRotation(RigidBodyHandle handle, ev_Vector4 *rotationQuat);

};
