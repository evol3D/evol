#ifndef WORLD_CAMERA_MODULE_H
#define WORLD_CAMERA_MODULE_H

#include "flecs.h"
#include "types.h"

typedef enum
{
  EV_CAMERA_PERSPECTIVE_VIEW,
  EV_CAMERA_ORTHOGRAPHIC_VIEW,
} CameraViewType;

typedef struct
{
  CameraViewType viewType;

  unsigned int hfov;
  unsigned int vfov;

  float aspectRatio;

  float nearPlane;
  float farPlane;

  ev_Matrix4 projectionMatrix;
} CameraComponent;

typedef struct
{
  unsigned long entityId;
} MainCamera;

ECS_COMPONENT_EXTERN(CameraComponent);
ECS_COMPONENT_EXTERN(MainCamera);

void OnAddCameraComponent(ecs_iter_t *it);
void OnChangeCameraParameters(ecs_iter_t *it);

#define DEFINE_COMPONENTS_CAMERA(world) \
  ECS_COMPONENT_DEFINE(world, CameraComponent); \
  ECS_COMPONENT_DEFINE(world, MainCamera)

#define REGISTER_SYSTEMS_CAMERA(world) \
  ECS_SYSTEM(world, OnChangeCameraParameters, EcsOnSet, CameraComponent); \
  ECS_TRIGGER(world, OnAddCameraComponent   , EcsOnAdd, CameraComponent)


#endif
