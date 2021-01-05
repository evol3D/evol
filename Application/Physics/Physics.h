#ifndef EVOL_PHYSICS_H
#define EVOL_PHYSICS_H

#include <physics_types.h>

#ifdef __cplusplus
extern "C" {
#endif



extern struct _ev_Physics {
  int (*init)();
  int (*deinit)();
  int (*step)();
  int (*step_dt)(real);
  CollisionShape (*createBox)(real, real, real);
  CollisionShape (*createCapsule)(real, real);
  CollisionShape (*createCylinderX) (real width, real radiusX, real radiusY);
  RigidBodyHandle (*addRigidBody)(RigidBody*);
  void (*setGravity)(real, real, real);
  CollisionShape (*createSphere)(real);
  void (*removeRigidBody)(RigidBodyHandle);
  void (*updateRigidBody)(RigidBodyHandle, RigidBody*);
  CollisionShape (*createStaticFromTriangleIndexVertex)(int, int *, int, int, real *, int);
  CollisionShape (*generateConvexHull)(int vertexCount, ev_Vector3* vertices);
  void (*setLinearVelocity)(RigidBodyHandle, ev_Vector3 *velocity);
  void (*getLinearVelocity)(RigidBodyHandle, ev_Vector3 *velocity);
  void (*setAngularVelocity)(RigidBodyHandle, ev_Vector3 *velocity);
  void (*getAngularVelocity)(RigidBodyHandle, ev_Vector3 *velocity);
  void (*applyForce)(RigidBodyHandle, ev_Vector3 *force);
  void (*setFriction)(RigidBodyHandle, real frict);
  void (*setDamping)(RigidBodyHandle, real linearDamping, real angularDamping);
  void (*addHingeConstraint)(RigidBodyHandle parentHandle, RigidBodyHandle childHandle, ev_Vector3 *anchor, ev_Vector3 *parentAxis, ev_Vector3 *childAxis);
  void (*setRotation)(RigidBodyHandle handle, ev_Vector4 *rotationQuat);
  PhysicsVehicle (*createRaycastVehicle)(RigidBodyHandle handle);
  void (*applyEngineForce)(PhysicsVehicle physVehicle, real force);
  void (*setVehicleSteering)(PhysicsVehicle physVehicle, real steeringVal);
} Physics;











#ifdef __cplusplus
};
#endif
#endif
