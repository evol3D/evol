#include "physics_module.h"

void PhysicsModuleImport(ecs_world_t *world)
{
  ECS_MODULE(world, PhysicsModule);

  ECS_COMPONENT(world, LinearVelocity);
  ECS_COMPONENT(world, AngularVelocity);

  ECS_TYPE(world, Physics, LinearVelocity, AngularVelocity);

  ECS_EXPORT_COMPONENT(LinearVelocity);
  ECS_EXPORT_COMPONENT(AngularVelocity);
  ECS_EXPORT_TYPE(Physics);
}
