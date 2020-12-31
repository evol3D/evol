#include "Physics.h"

#include "BulletState.h"

static int ev_physics_init();
static int ev_physics_deinit();
static int ev_physics_step();
static int ev_physics_step_dt(real deltaTime = 1.f/60.f);
static inline CollisionShape ev_physics_create_box(real x, real y, real z);
static inline CollisionShape ev_physics_create_sphere(real r);
static inline CollisionShape ev_physics_create_mesh_from_index_vertex(int numTriangles, int *triangleIndexBase, int triangleIndexStride, int numVertices, real *vertexBase, int vertexStride);
static inline void ev_physics_set_gravity(real x, real y, real z);
static inline RigidBodyHandle ev_physics_add_rigidbody(RigidBody *rb);
static inline void ev_physics_remove_rigidbody(RigidBodyHandle handle);
static inline void ev_physics_update_rigidbody(RigidBodyHandle handle, RigidBody *rb);
static inline CollisionShape ev_physics_generateconvexhull(int vertexCount, ev_Vector3* vertices);


struct ev_Physics_Data {
  PhysicsState *state;
} PhysicsData;

struct _ev_Physics Physics = {
  ev_physics_init,
  ev_physics_deinit,
};

static int ev_physics_init()
{
  Physics.step            = ev_physics_step;
  Physics.step_dt         = ev_physics_step_dt;
  Physics.setGravity      = ev_physics_set_gravity;
  Physics.addRigidBody    = ev_physics_add_rigidbody;
  Physics.removeRigidBody = ev_physics_remove_rigidbody;
  Physics.updateRigidBody = ev_physics_update_rigidbody;
  Physics.createBox       = ev_physics_create_box;
  Physics.createSphere    = ev_physics_create_sphere;
  Physics.createStaticFromTriangleIndexVertex = ev_physics_create_mesh_from_index_vertex;
  Physics.generateConvexHull = ev_physics_generateconvexhull;

  PhysicsData.state = new BulletState();
  //PhysicsData.state->visualize();
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
