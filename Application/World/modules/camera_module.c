//TODO Comments / Logging
#include "camera_module.h"
#include "cglm/cglm.h"

ECS_COMPONENT_DECLARE(CameraComponent);
ECS_COMPONENT_DECLARE(MainCamera);

void OnAddCameraComponent(ecs_iter_t *it)
{
  CameraComponent *cameraComp = ecs_column(it, CameraComponent, 1);
  for(int i = 0; i < it->count; ++i)
  {
    // TODO Load default values
    glm_mat4_identity(cameraComp[i].projectionMatrix);
    ecs_singleton_set(it->world, MainCamera, {it->entities[i]});
  }

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
