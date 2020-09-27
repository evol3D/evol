#include "Physics.h"
#include "btBulletDynamicsCommon.h"

#include "PhysicsState.h"
#include "BulletState.h"

static int ev_physics_init();
static int ev_physics_deinit();
static int ev_physics_step();
static int ev_physics_step_dt(real deltaTime = 1.f/60.f);

struct ev_Physics_Data {
  PhysicsState *state;
} PhysicsData;

struct _ev_Physics Physics = {
  .init = ev_physics_init,
  .deinit = ev_physics_deinit,
  .step = ev_physics_step,
  .step_dt = ev_physics_step_dt,
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

}

static int ev_physics_step_dt(real deltaTime)
{

}
