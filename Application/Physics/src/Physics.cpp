#include "Physics.h"

#include "btBulletDynamicsCommon.h"

#include "BulletState.h"

static int ev_physics_init();
static int ev_physics_deinit();
static int ev_physics_step();
static int ev_physics_step_dt(real deltaTime = 1.f/60.f);
static inline CollisionShape ev_physics_create_box(real x, real y, real z);
static inline void ev_physics_add_rigidbody(RigidBody *rb);

struct ev_Physics_Data {
  PhysicsState *state;
} PhysicsData;

struct _ev_Physics Physics = {
  ev_physics_init,
  ev_physics_deinit,
  ev_physics_step,
  ev_physics_step_dt,
  ev_physics_create_box,
  ev_physics_add_rigidbody,
};

static int ev_physics_init()
{
  PhysicsData.state = new BulletState;
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
	return 0;
}

static int ev_physics_step_dt(real deltaTime)
{
	return 0;
}

static inline CollisionShape ev_physics_create_box(real x, real y, real z)
{
  return PhysicsData.state->createBox(x, y, z);
}

static inline void ev_physics_add_rigidbody(RigidBody *rb)
{

  PhysicsData.state->addRigidBody(rb);

}
