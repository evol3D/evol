//TODO Comments / Logging
#include "physics_module.h"
#include "transform_module.h"

#include "physics_types.h"
#include "Physics.h"

// When a RigidBodyComponent's value is updated
void SetRigidBody(ecs_iter_t *it) {

  RigidBodyComponent *rb = ecs_column(it, RigidBodyComponent, 1);
  RigidBodyHandleComponent *rb_handle = ecs_column(it, RigidBodyHandleComponent, 2);

  // Fill an intermediate struct that is used by the physics module to initialize
  // rigidbodies
  RigidBody rb_phys;
  rb_phys.entt_id = *it->entities;
  rb_phys.mass = rb->mass;
  rb_phys.collisionShape = rb->collisionShape;

  // If the handle points to an existing rigidbody, then the rigidbody is updated
  // with the new values. Otherwise, it creates a new rigidbody in the simulation
  if(rb_handle->handle)
  {
    Physics.updateRigidBody(rb_handle->handle, &rb_phys);
  }
  else
  {
    rb_handle->handle = Physics.addRigidBody(&rb_phys);
  }
}

// Automatically add a RigidBodyHandleComponent with any RigidBodyComponent
// added. 
void AddRigidBody(ecs_iter_t *it) {
  ECS_IMPORT(it->world, PhysicsModule);
  
// Set the handle to NULL so that when the RigidBodyComponent value
// is set, it adds a new RigidBody to the simulation instead of updating an
// existing one.
  ecs_set(it->world, *it->entities, RigidBodyHandleComponent, {NULL});
}

void RemoveRigidBody(ecs_iter_t *it) {
  // TODO
  assert(!"TODO: RemoveRigidBody not implemented");
}

#include <flecs_meta.h>

void PhysicsModuleImport(ecs_world_t *world)
{
  ECS_IMPORT(world, TransformModule);

  ECS_MODULE(world, PhysicsModule);

  ECS_COMPONENT(world, RigidBodyComponent);
  ECS_COMPONENT(world, RigidBodyHandleComponent);

  ECS_EXPORT_COMPONENT(RigidBodyComponent);
  ECS_EXPORT_COMPONENT(RigidBodyHandleComponent);

  ECS_TRIGGER(world, AddRigidBody, EcsOnAdd, RigidBodyComponent);
  ECS_TRIGGER(world, RemoveRigidBody, EcsOnRemove, RigidBodyComponent);

  ECS_SYSTEM(world, SetRigidBody, EcsOnSet, RigidBodyComponent, RigidBodyHandleComponent, transform.module.TransformComponent);

  ECS_IMPORT(world, FlecsMeta);

}
