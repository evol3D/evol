#ifndef EVOL_WORLD_H
#define EVOL_WORLD_H

#include <flecs.h>

typedef ecs_entity_t Entity;
typedef ecs_world_t* WorldInstance;
typedef ecs_type_t ComponentSet;

extern struct ev_World {
    int (*init)();
    int (*deinit)();
    WorldInstance instance;
} World;

#define CreateEntity() ecs_new(World.instance, 0)
#define CreateNamedEntity(name) ECS_ENTITY(World.instance, name)

#define RegisterComponent(type) ECS_COMPONENT(World.instance, type)
#define RegisterTag(name) ECS_TAG(World.instance, name)
#define RegisterComponentSet(...) ECS_TYPE(World.instance, __VA_ARGS__)

#define Entity_AddTag(entt, tag) ecs_add(World.instance, entt, tag)
#define Entity_RemoveTag(entt, tag) ecs_remove(World.instance, entt, tag)

#define Entity_AddComponent(entt, cmp) ecs_add(World.instance, entt, cmp)
#define Entity_RemoveComponent(entt, cmp) ecs_remove(World.instance, entt, cmp)
#define Entity_SetComponent(entt, cmp, value) ecs_set(World.instance, entt, cmp, value)
#define Entity_GetComponent(entt, cmp) ecs_get(World.instance, entt, cmp)
#define Entity_GetComponent_mut(entt, cmp) ecs_get_mut(World.instance, entt, cmp)

#define Entity_GetName(entt) ecs_get_name(World.instance, entt)

#define Entity_AddComponentSet(entt, cmps) ecs_add(World.instance, entt, cmps)
#define Entity_GetComponentSet(entt) ecs_get_type(World.instance, entt)
#define Entity_PrintComponents(entt) ecs_type_str(World.instance, ecs_get_type(World.instance, entt))

#endif //EVOL_WORLD_H
