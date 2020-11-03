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

typedef struct
{
  ECS_DECLARE_COMPONENT(CameraComponent);
  ECS_DECLARE_COMPONENT(MainCamera);
} CameraModule;

void CameraModuleImport(ecs_world_t *world);

#define CameraModuleImportHandles(module)\
  ECS_IMPORT_COMPONENT(module, CameraComponent);\
  ECS_IMPORT_COMPONENT(module, MainCamera);\

#endif
