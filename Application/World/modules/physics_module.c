#include "physics_module.h"
#include "transform_module.h"

#include "physics_types.h"
#include "Physics.h"

void SetRigidBody(ecs_iter_t *it) {

  RigidBodyComponent *rb = ecs_column(it, RigidBodyComponent, 1);
  TransformComponent *tr = ecs_column(it, TransformComponent, 2);
  RigidBodyHandleComponent *rb_handle = ecs_column(it, RigidBodyHandleComponent, 3);

  for(int i = 0; i < it->count; ++i)
  {
    RigidBody rb_phys;
    rb_phys.position = &(tr[i].position);
    rb_phys.rotation = &(tr[i].rotation);
    rb_phys.mass = rb[i].mass;
    rb_phys.collisionShape = rb[i].collisionShape;

    if(rb_handle[i].handle)
    {
      Physics.updateRigidBody(rb_handle[i].handle, &rb_phys);
    }
    else
    {
      rb_handle[i].handle = Physics.addRigidBody(&rb_phys);
    }
  }
}

void AddRigidBody(ecs_iter_t *it) {
  ECS_IMPORT(it->world, PhysicsModule);

  for(int i = 0; i < it->count; ++i)
  {
    if(!ecs_get(it->world, it->entities[i], RigidBodyHandleComponent))
    {
      ecs_get_mut(it->world, it->entities[i], RigidBodyHandleComponent, NULL)->handle = NULL;
    }
  }
}

void RemoveRigidBody(ecs_iter_t *it) {
  assert(!"TODO: RemoveRigidBody not implemented");
}

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

  ECS_SYSTEM(world, SetRigidBody, EcsOnSet, RigidBodyComponent, transform.module.TransformComponent, RigidBodyHandleComponent);
}
