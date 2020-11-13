//TODO Comments / Logging
#include "physics_module.h"
#include "transform_module.h"

#include "physics_types.h"
#include "Physics.h"

ECS_COMPONENT_DECLARE(RigidBodyComponent);
ECS_COMPONENT_DECLARE(RigidBodyHandleComponent);

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
  rb_phys.restitution = rb->restitution;
  rb_phys.type = rb->type;

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
  
// Set the handle to NULL so that when the RigidBodyComponent value
// is set, it adds a new RigidBody to the simulation instead of updating an
// existing one.
  ecs_set(it->world, *it->entities, RigidBodyHandleComponent, {NULL});
}

void RemoveRigidBody(ecs_iter_t *it) {
  // TODO
  assert(!"TODO: RemoveRigidBody not implemented");
}
