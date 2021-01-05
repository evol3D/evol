#include "Physics.h"

#include "BulletState.h"

static int ev_physics_init();
static int ev_physics_deinit();
static int ev_physics_step();
static int ev_physics_step_dt(real deltaTime = 1.f/60.f);
static inline CollisionShape ev_physics_create_box(real x, real y, real z);
static inline CollisionShape ev_physics_create_sphere(real r);
static inline CollisionShape ev_physics_create_capsule(real radius, real height);
static inline CollisionShape ev_physics_createcylinderx(real width, real radiusX, real radiusY);
static inline CollisionShape ev_physics_create_mesh_from_index_vertex(int numTriangles, int *triangleIndexBase, int triangleIndexStride, int numVertices, real *vertexBase, int vertexStride);
static inline void ev_physics_set_gravity(real x, real y, real z);
static inline RigidBodyHandle ev_physics_add_rigidbody(RigidBody *rb);
static inline void ev_physics_remove_rigidbody(RigidBodyHandle handle);
static inline void ev_physics_update_rigidbody(RigidBodyHandle handle, RigidBody *rb);
static inline CollisionShape ev_physics_generateconvexhull(int vertexCount, ev_Vector3* vertices);
static void ev_physics_setlinearvelocity(RigidBodyHandle handle, ev_Vector3 *velocity);
static void ev_physics_getlinearvelocity(RigidBodyHandle handle, ev_Vector3 *velocity);
static void ev_physics_setangularvelocity(RigidBodyHandle handle, ev_Vector3 *velocity);
static void ev_physics_getangularvelocity(RigidBodyHandle handle, ev_Vector3 *velocity);
static void ev_physics_applyforce(RigidBodyHandle handle, ev_Vector3 *force);
static void ev_physics_setfriction(RigidBodyHandle handle, real frict);
static void ev_physics_setdamping(RigidBodyHandle handle, real linearDamping, real angularDamping);
static void ev_physics_addhingeconstraint(RigidBodyHandle parentHandle, RigidBodyHandle childHandle, ev_Vector3 *anchor, ev_Vector3 *parentAxis, ev_Vector3 *childAxis);
static void ev_physics_setrotation(RigidBodyHandle handle, ev_Vector4 *rotationQuat);
static PhysicsVehicle ev_physics_createraycastvehicle(RigidBodyHandle handle);
static void ev_physics_applyengineforce(PhysicsVehicle physVehicle, real force);
static void ev_physics_setvehiclesteering(PhysicsVehicle physVehicle, real steeringVal);

struct ev_Physics_Data {
  PhysicsState *state;
} PhysicsData;

struct _ev_Physics Physics = {
  ev_physics_init,
  ev_physics_deinit,
};

static int ev_physics_init()
{
  Physics.step                                = ev_physics_step;
  Physics.step_dt                             = ev_physics_step_dt;
  Physics.setGravity                          = ev_physics_set_gravity;
  Physics.addRigidBody                        = ev_physics_add_rigidbody;
  Physics.removeRigidBody                     = ev_physics_remove_rigidbody;
  Physics.updateRigidBody                     = ev_physics_update_rigidbody;
  Physics.createBox                           = ev_physics_create_box;
  Physics.createSphere                        = ev_physics_create_sphere;
  Physics.createCapsule                       = ev_physics_create_capsule;
  Physics.createCylinderX                     = ev_physics_createcylinderx;
  Physics.createStaticFromTriangleIndexVertex = ev_physics_create_mesh_from_index_vertex;
  Physics.generateConvexHull                  = ev_physics_generateconvexhull;
  Physics.setLinearVelocity                   = ev_physics_setlinearvelocity;
  Physics.getLinearVelocity                   = ev_physics_getlinearvelocity;
  Physics.setAngularVelocity                  = ev_physics_setangularvelocity;
  Physics.getAngularVelocity                  = ev_physics_getangularvelocity;
  Physics.applyForce                          = ev_physics_applyforce;
  Physics.setFriction                         = ev_physics_setfriction;
  Physics.setDamping                          = ev_physics_setdamping;
  Physics.addHingeConstraint                  = ev_physics_addhingeconstraint;
  Physics.setRotation                         = ev_physics_setrotation;
  Physics.createRaycastVehicle                = ev_physics_createraycastvehicle;
  Physics.applyEngineForce                    = ev_physics_applyengineforce;
  Physics.setVehicleSteering                  = ev_physics_setvehiclesteering;


  PhysicsData.state = new BulletState();
  PhysicsData.state->visualize();
  return 0;
}

static int ev_physics_deinit()
{
  delete PhysicsData.state;
  return 0;
}

static int ev_physics_step()
{
  PhysicsData.state->step();
	return 0;
}

static int ev_physics_step_dt(real deltaTime)
{
  PhysicsData.state->step_dt(deltaTime);
	return 0;
}

static inline CollisionShape ev_physics_create_box(real x, real y, real z)
{
  return PhysicsData.state->createBox(x, y, z);
}

static inline CollisionShape ev_physics_create_sphere(real r)
{
  return PhysicsData.state->createSphere(r);
}
static inline CollisionShape ev_physics_create_mesh_from_index_vertex(int numTriangles, int *triangleIndexBase, int triangleIndexStride, int numVertices, real *vertexBase, int vertexStride)
{
  return PhysicsData.state->createStaticFromTriangleIndexVertex(numTriangles, triangleIndexBase, triangleIndexStride, numVertices, vertexBase, vertexStride);
}

static inline RigidBodyHandle ev_physics_add_rigidbody(RigidBody *rb)
{
  return PhysicsData.state->addRigidBody(rb);
}
static inline void ev_physics_remove_rigidbody(RigidBodyHandle rb)
{
  PhysicsData.state->removeRigidBody(rb);
}

static inline void ev_physics_set_gravity(real x, real y, real z)
{
  PhysicsData.state->setGravity(x, y, z);
}

static inline void ev_physics_update_rigidbody(RigidBodyHandle handle, RigidBody *rb)
{
  PhysicsData.state->updateRigidBody(handle, rb);
}

static inline CollisionShape ev_physics_generateconvexhull(int vertexCount, ev_Vector3* vertices)
{
  return PhysicsData.state->generateConvexHull(vertexCount, vertices);
}

static void ev_physics_setlinearvelocity(RigidBodyHandle handle, ev_Vector3 *velocity)
{
  PhysicsData.state->setLinearVelocity(handle, velocity);
}

static void ev_physics_getlinearvelocity(RigidBodyHandle handle, ev_Vector3 *velocity)
{
  PhysicsData.state->getLinearVelocity(handle, velocity);
}

static void ev_physics_setangularvelocity(RigidBodyHandle handle, ev_Vector3 *velocity)
{
  PhysicsData.state->setAngularVelocity(handle, velocity);
}

static void ev_physics_getangularvelocity(RigidBodyHandle handle, ev_Vector3 *velocity)
{
  PhysicsData.state->getAngularVelocity(handle, velocity);
}

static void ev_physics_applyforce(RigidBodyHandle handle, ev_Vector3 *force)
{
  PhysicsData.state->applyForce(handle, force);
}

static void ev_physics_setfriction(RigidBodyHandle handle, real frict)
{
  PhysicsData.state->setFriction(handle, frict);
}

static void ev_physics_setdamping(RigidBodyHandle handle, real linearDamping, real angularDamping)
{
  PhysicsData.state->setDamping(handle, linearDamping, angularDamping);
}

static inline CollisionShape ev_physics_create_capsule(real radius, real height)
{
  return PhysicsData.state->createCapsule(radius, height);
}

static void ev_physics_addhingeconstraint(RigidBodyHandle parentHandle, RigidBodyHandle childHandle, ev_Vector3 *anchor, ev_Vector3 *parentAxis, ev_Vector3 *childAxis)
{
  PhysicsData.state->addHingeConstraint(parentHandle, childHandle, anchor, parentAxis, childAxis);
}

static CollisionShape ev_physics_createcylinderx(real width, real radiusX, real radiusY)
{
  return PhysicsData.state->createCylinderX(width, radiusX, radiusY);
}

static void ev_physics_setrotation(RigidBodyHandle handle, ev_Vector4 *rotationQuat)
{
  PhysicsData.state->setRotation(handle, rotationQuat);
}

static PhysicsVehicle ev_physics_createraycastvehicle(RigidBodyHandle handle)
{
  return PhysicsData.state->createRaycastVehicle(handle);
}

static void ev_physics_applyengineforce(PhysicsVehicle physVehicle, real force)
{
  PhysicsData.state->applyEngineForce(physVehicle, force);
}

static void ev_physics_setvehiclesteering(PhysicsVehicle physVehicle, real steeringVal)
{
  PhysicsData.state->setVehicleSteering(physVehicle, steeringVal);
}
