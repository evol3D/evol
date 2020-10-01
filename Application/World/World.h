#ifndef EVOL_WORLD_H
#define EVOL_WORLD_H

#include <flecs.h>

typedef ecs_entity_t Entity;
typedef ecs_world_t* WorldInstance;
typedef ecs_type_t ComponentSet;
typedef ecs_type_t Component;
typedef ecs_iter_t SystemArgs;

extern struct ev_World {
    int (*init)();
    int (*deinit)();
    int (*progress)();
    WorldInstance instance;
} World;

#define RegisterComponent(type) ECS_COMPONENT(World.instance, type)
#define RegisterTag(name) ECS_TAG(World.instance, name)
#define RegisterComponentSet(...) ECS_TYPE(World.instance, __VA_ARGS__)

#define RegisterSystem_OnLoad(sys, ...)     ECS_SYSTEM(World.instance, sys, EcsOnLoad    , __VA_ARGS__)
#define RegisterSystem_PostLoad(sys, ...)   ECS_SYSTEM(World.instance, sys, EcsPostLoad  , __VA_ARGS__)
#define RegisterSystem_PreUpdate(sys, ...)  ECS_SYSTEM(World.instance, sys, EcsPreUpdate , __VA_ARGS__)
#define RegisterSystem_OnUpdate(sys, ...)   ECS_SYSTEM(World.instance, sys, EcsOnUpdate  , __VA_ARGS__)
#define RegisterSystem_OnValidate(sys, ...) ECS_SYSTEM(World.instance, sys, EcsOnValidate, __VA_ARGS__)
#define RegisterSystem_PostUpdate(sys, ...) ECS_SYSTEM(World.instance, sys, EcsPostUpdate, __VA_ARGS__)
#define RegisterSystem_PreStore(sys, ...)   ECS_SYSTEM(World.instance, sys, EcsPreStore  , __VA_ARGS__)
#define RegisterSystem_OnStore(sys, ...)    ECS_SYSTEM(World.instance, sys, EcsOnStore   , __VA_ARGS__)

#define CreateEntity() ecs_new(World.instance, 0)
#define CreateNamedEntity(name) ECS_ENTITY(World.instance, name)

#define Prefab_Create(pb) ECS_PREFAB(World.instance, pb)

// Instancing relationships will not be supported yet.
// It'll probably be needed (don't forget that instancing relationships can
// be encoded as ecs_type_t)
//#define CreateEntity_from(base) ecs_new_w_entity(World.instance, ECS_INSTANCEOF | base)
//#define Entity_AddEntity(instance, base) ecs_add_entity(World.instance, instance, ECS_INSTANCEOF | base)
//#define Entity_RemoveEntity(instance, base) ecs_remove_entity(World.instance, instance, ECS_INSTANCEOF | base)
//

#define Entity_AddTag(entt, tag) ecs_add(World.instance, entt, tag)
#define Entity_RemoveTag(entt, tag) ecs_remove(World.instance, entt, tag)

#define Entity_AddComponent(...) ecs_add(World.instance, __VA_ARGS__)
#define Entity_RemoveComponent(entt, cmp) ecs_remove(World.instance, entt, cmp)
#define Entity_SetComponent(entt, cmp, ...) ecs_set(World.instance, entt, cmp, __VA_ARGS__)
#define Entity_GetComponent(entt, cmp) ecs_get(World.instance, entt, cmp)
#define Entity_GetComponent_mut(entt, cmp) ecs_get_mut(World.instance, entt, cmp, NULL)

#define Entity_AddChild(parent) ecs_new_w_entity(World.instance, ECS_CHILDOF | parent)

#define Entity_GetName(entt) ecs_get_name(World.instance, entt)

#define Entity_AddComponentSet(entt, cmps) ecs_add(World.instance, entt, cmps)
#define Entity_GetComponentSet(entt) ecs_get_type(World.instance, entt)
#define Entity_PrintComponents(entt) ecs_type_str(World.instance, ecs_get_type(World.instance, entt))

#define ImportModule(...) ECS_IMPORT(World.instance, __VA_ARGS__)

#endif //EVOL_WORLD_H
