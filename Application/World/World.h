#ifndef EVOL_WORLD_H
#define EVOL_WORLD_H

#include <flecs.h>

#ifdef DEBUG
# include <flecs_meta.h>
# include <flecs_dash.h>
# include <flecs_systems_civetweb.h>
#endif

typedef ecs_entity_t Entity;
typedef ecs_world_t* SceneInstance;
typedef ecs_type_t   ComponentSet;
typedef ecs_type_t   Component;
typedef ecs_iter_t   SystemArgs;


extern struct ev_World {
    int (*init)();
    int (*deinit)();
    int (*progress)();
    int (*loadScene)(const char *);
    int (*saveScene)(const char *);
    int (*newScene)();
    void (*lockSceneAccess)();
    void (*unlockSceneAccess)();
    SceneInstance (*getInstance)();
} World;


#define RegisterComponent(type)   ECS_COMPONENT(World.getInstance(), type)
#define RegisterTag(name)         ECS_TAG(World.getInstance(), name)
#define RegisterComponentSet(...) ECS_TYPE(World.getInstance(), __VA_ARGS__)

#define RegisterSystem_OnLoad(sys, ...)     ECS_SYSTEM(World.getInstance(), sys, EcsOnLoad    , __VA_ARGS__)
#define RegisterSystem_PostLoad(sys, ...)   ECS_SYSTEM(World.getInstance(), sys, EcsPostLoad  , __VA_ARGS__)
#define RegisterSystem_PreUpdate(sys, ...)  ECS_SYSTEM(World.getInstance(), sys, EcsPreUpdate , __VA_ARGS__)
#define RegisterSystem_OnUpdate(sys, ...)   ECS_SYSTEM(World.getInstance(), sys, EcsOnUpdate  , __VA_ARGS__)
#define RegisterSystem_OnValidate(sys, ...) ECS_SYSTEM(World.getInstance(), sys, EcsOnValidate, __VA_ARGS__)
#define RegisterSystem_PostUpdate(sys, ...) ECS_SYSTEM(World.getInstance(), sys, EcsPostUpdate, __VA_ARGS__)
#define RegisterSystem_PreStore(sys, ...)   ECS_SYSTEM(World.getInstance(), sys, EcsPreStore  , __VA_ARGS__)
#define RegisterSystem_OnStore(sys, ...)    ECS_SYSTEM(World.getInstance(), sys, EcsOnStore   , __VA_ARGS__)

#define CreateEntity() ecs_new(World.getInstance(), 0)
#define CreateNamedEntity(name) ECS_ENTITY(World.getInstance(), name, 0)

#define Prefab_Create(pb) ECS_PREFAB(World.getInstance(), pb)

// Instancing relationships will not be supported yet.
// It'll probably be needed (don't forget that instancing relationships can
// be encoded as ecs_type_t)
//#define CreateEntity_from(base) ecs_new_w_entity(World.getInstance(), ECS_INSTANCEOF | base)
//#define Entity_AddEntity(getInstance(), base) ecs_add_entity(World.getInstance(), getInstance(), ECS_INSTANCEOF | base)
//#define Entity_RemoveEntity(getInstance(), base) ecs_remove_entity(World.getInstance(), getInstance(), ECS_INSTANCEOF | base)
//

#define Entity_AddTag(entt, tag)    ecs_add(World.getInstance(), entt, tag)
#define Entity_RemoveTag(entt, tag) ecs_remove(World.getInstance(), entt, tag)

#define Entity_AddComponent(...)            ecs_add(World.getInstance(), __VA_ARGS__)
#define Entity_RemoveComponent(entt, cmp)   ecs_remove(World.getInstance(), entt, cmp)
#define Entity_SetComponent(entt, cmp, ...) ecs_set(World.getInstance(), entt, cmp, __VA_ARGS__)
#define Entity_GetComponent(entt, cmp)      ecs_get(World.getInstance(), entt, cmp)
#define Entity_GetComponent_mut(entt, cmp)  ecs_get_mut(World.getInstance(), entt, cmp, NULL)

#define Entity_AddChild(parent) ecs_new_w_entity(World.getInstance(), ECS_CHILDOF | parent)

#define Entity_GetName(entt) ecs_get_name(World.getInstance(), entt)

#define Entity_AddComponentSet(entt, cmps) ecs_add(World.getInstance(), entt, cmps)
#define Entity_GetComponentSet(entt)       ecs_get_type(World.getInstance(), entt)
#define Entity_PrintComponents(entt)       ecs_type_str(World.getInstance(), ecs_get_type(World.getInstance(), entt))

#define ImportModule(...) ECS_IMPORT(World.getInstance(), __VA_ARGS__)

#endif //EVOL_WORLD_H
