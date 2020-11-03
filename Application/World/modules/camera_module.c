//TODO Comments / Logging
#include "camera_module.h"
#include "cglm/cglm.h"
#include "cglm/util.h"

void OnAddCameraComponent(ecs_iter_t *it)
{
  ECS_IMPORT(it->world, CameraModule);

  CameraComponent *cameraComp = ecs_column(it, CameraComponent, 1);
  for(int i = 0; i < it->count; ++i)
  {
    // TODO Load default values
    glm_mat4_identity(cameraComp[i].projectionMatrix);
    ecs_singleton_set(it->world, MainCamera, {it->entities[i]});
  }

}

void EV_UNUSED OnRemoveCameraComponent(ecs_iter_t *it)
{
}

void OnChangeCameraParameters(ecs_iter_t *it)
{
  CameraComponent *cameraComp = ecs_column(it, CameraComponent, 1);

  for(int i = 0; i < it->count; ++i)
  {
    if(cameraComp[i].viewType == EV_CAMERA_PERSPECTIVE_VIEW)
    {
      glm_perspective(glm_rad(cameraComp[i].hfov), cameraComp[i].aspectRatio, cameraComp[i].nearPlane, cameraComp[i].farPlane, cameraComp[i].projectionMatrix);
    }
    else
    {
      assert(!"Unimplemented: Orthographic Camera");
    }
  }
}

void CameraModuleImport(ecs_world_t *world)
{
  ECS_MODULE(world, CameraModule);

  ECS_COMPONENT(world, CameraComponent);
  ECS_COMPONENT(world, MainCamera);

  ECS_TRIGGER(world, OnAddCameraComponent   , EcsOnAdd   , CameraComponent);
  /* ECS_TRIGGER(world, OnRemoveCameraComponent, EcsOnRemove, CameraComponent); */

#ifdef FLECS_DASHBOARD
  ECS_SYSTEM(world, OnChangeCameraParameters, EcsOnSet, CameraComponent);
#else
  ECS_SYSTEM(world, OnChangeCameraParameters, EcsOnSet, CameraComponent);
#endif

  ECS_EXPORT_COMPONENT(CameraComponent);
  ECS_EXPORT_COMPONENT(MainCamera);
}