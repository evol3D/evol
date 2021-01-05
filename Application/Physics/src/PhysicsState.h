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
    virtual CollisionShape createCapsule(real radius, real height) = 0;
    virtual CollisionShape createStaticFromTriangleIndexVertex(int numTriangles, int *triangleIndexBase, int triangleIndexStride, int numVertices, real *vertexBase, int vertexStride) = 0;
    virtual CollisionShape generateConvexHull(int vertexCount, ev_Vector3* vertices) = 0;
    virtual CollisionShape createCylinderX(real width, real radiusX, real radiusY) = 0;
    virtual void setLinearVelocity(RigidBodyHandle handle, ev_Vector3 *velocity) = 0;
    virtual void getLinearVelocity(RigidBodyHandle handle, ev_Vector3 *velocity) = 0;
    virtual void setAngularVelocity(RigidBodyHandle handle, ev_Vector3 *velocity) = 0;
    virtual void getAngularVelocity(RigidBodyHandle handle, ev_Vector3 *velocity) = 0;
    virtual void applyForce(RigidBodyHandle handle, ev_Vector3 *force) = 0;
    virtual void setFriction(RigidBodyHandle handle, real frict) = 0;
    virtual void setDamping(RigidBodyHandle handle, real linearDamping, real angularDamping) = 0;
    virtual void addHingeConstraint(RigidBodyHandle parentHandle, RigidBodyHandle childHandle, ev_Vector3 *anchor, ev_Vector3 *parentAxis, ev_Vector3 *childAxis) = 0;
    virtual void setRotation(RigidBodyHandle handle, ev_Vector4 *rotationQuat) = 0;
    virtual PhysicsVehicle createRaycastVehicle(RigidBodyHandle handle) = 0;
    virtual void applyEngineForce(PhysicsVehicle physVehicle, real force) = 0;
    virtual void setVehicleSteering(PhysicsVehicle physVehicle, real steeringVal) = 0;
};
