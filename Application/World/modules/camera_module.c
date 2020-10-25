//TODO Comments / Logging
#include "camera_module.h"

void OnAddCameraComponent(ecs_iter_t *it)
{
  ECS_IMPORT(it->world, CameraModule);
  for(int i = 0; i < it->count; ++i)
  {
    ecs_add(it->world, it->entities[i], CameraMatrixComponent);
  }
}

void OnRemoveCameraComponent(ecs_iter_t *it)
{
  ECS_IMPORT(it->world, CameraModule);
  for(int i = 0; i < it->count; ++i)
  {
    ecs_remove(it->world, it->entities[i], CameraMatrixComponent);
  }
}

#include "cglm/cglm.h"
#include "transform_module.h"
void OnChangeCamera(ecs_iter_t *it)
{
  CameraComponent *cameraComp = ecs_column(it, CameraComponent, 1);
  CameraMatrixComponent *cameraMatrixComp = ecs_column(it, CameraMatrixComponent, 2);
  TransformComponent *transformComp = ecs_column(it, TransformComponent, 3);

  for(int i = 0; i < it->count; ++i)
  {
    ev_Matrix4 projectionMatrix;//, viewMatrix;

    if(cameraComp[i].viewType == EV_CAMERA_PERSPECTIVE_VIEW)
    {
      glm_perspective(cameraComp[i].hfov, cameraComp[i].aspectRatio, cameraComp[i].nearPlane, cameraComp[i].farPlane, projectionMatrix);
    }
    else
    {
      assert(!"Unimplemented: Orthographic Camera");
    }

    glm_mul(projectionMatrix, transformComp[i].worldTransform, cameraMatrixComp->matrix);
  }
}

void CameraModuleImport(ecs_world_t *world)
{
  ECS_MODULE(world, CameraModule);

  ECS_COMPONENT(world, CameraComponent);
  ECS_COMPONENT(world, CameraMatrixComponent);

  ECS_TRIGGER(world, OnAddCameraComponent, EcsOnAdd, CameraComponent);
  ECS_TRIGGER(world, OnRemoveCameraComponent, EcsOnRemove, CameraComponent);

#ifdef FLECS_DASHBOARD
  ECS_SYSTEM(world, OnChangeCamera, EcsOnSet, CameraComponent, CameraMatrixComponent, TransformComponent);
#else
  ECS_SYSTEM(world, OnChangeCamera, EcsOnSet, CameraComponent, CameraMatrixComponent, transform.module.TransformComponent);
#endif

  ECS_EXPORT_COMPONENT(CameraComponent);
  ECS_EXPORT_COMPONENT(CameraMatrixComponent);
}
