//TODO Comments / Logging
#include "Game.h"

#include "App.h"
#include "Physics/Physics.h"
#include "Renderer/Renderer.h"
#include "Window.h"
#include "World/World.h"
#include "World/WorldModules.h"

#include "cglm/call/mat4.h"
#include "cglm/mat4.h"
#include "flecs.h"

#include "EventSystem.h"
#include "events/events.h"

#include "ev_log/ev_log.h"

#include "physics_types.h"
#include "utils.h"

// Game Systems
void MaintainTransformConstraints(SystemArgs *args);

// API functions
static int ev_game_init();
static int ev_game_deinit();
static void ev_game_loop();

struct ev_Game Game = {
  .init   = ev_game_init,
  .deinit = ev_game_deinit,
  .loop   = ev_game_loop,
};

struct ev_Game_Data {
  int placeholder;
} GameData;

static int ev_game_init()
{
  ev_log_trace("Started initializing the game");

  ev_log_trace("Starting a new scene");
  World.newScene();
  ev_log_trace("New scene started");

  ev_log_trace("Registering systems");
  ImportModule(GeometryModule);
  ImportModule(TransformModule);

#ifdef FLECS_DASHBOARD
  RegisterSystem_OnUpdate(MaintainTransformConstraints, CASCADE: TransformComponent, TransformComponent);
#else
  RegisterSystem_OnUpdate(MaintainTransformConstraints, CASCADE: transform.module.TransformComponent, transform.module.TransformComponent);
#endif
  ev_log_trace("Registered systems");

  ev_log_trace("Finished initializing the game");
  return 0;
}

static int ev_game_deinit()
{
  return 0;
}

void ev_game_loop_physics(real dt)
{
  Physics.step_dt(dt);
  /* ev_log_info("Physics Step. dt = %f", dt); */
  /* ev_log_info("Iterations: %u", ++iterations); */

}

void sandbox();

#include <types.h>
static void ev_game_loop()
{
  sandbox();

  double old = Window.getTime();
  unsigned int physics_steprate = 60;
  double new;
  while(!App.closeSystem)
  {
    ev_log_trace("Starting gameloop iteration");
    new = Window.getTime();
    double timeStep = new - old;
    double remainingTime = (1.f/(double)physics_steprate) - timeStep;

    if(remainingTime<=0)
    {
      World.progress();
      World.lockSceneAccess();
      ev_game_loop_physics(timeStep);

      {
        // TODO This query can persist. Move it somewhere.
#ifdef FLECS_DASHBOARD
        ecs_query_t *q = ecs_query_new(World.getInstance(), "TransformComponent, SHARED: RenderingComponent");
        ecs_query_t *cameraQuery = ecs_query_new(World.getInstance(), "camera.module.CameraComponent, TransformComponent");
#else
        ecs_query_t *q = ecs_query_new(World.getInstance(), "transform.module.TransformComponent, SHARED: rendering.module.RenderingComponent");
        ecs_query_t *cameraQuery = ecs_query_new(World.getInstance(), "camera.module.CameraComponent, transform.module.TransformComponent");
#endif
        ecs_iter_t it = ecs_query_iter(q);
        ecs_iter_t cameraIter = ecs_query_iter(cameraQuery);

        ecs_query_next(&cameraIter);
        CameraComponent *cameraData = ecs_column(&cameraIter, CameraComponent, 1);
        TransformComponent *cameraTransform = ecs_column(&cameraIter, TransformComponent, 2);

        ev_RenderCamera renderCamera;

        glm_mat4_dup(cameraData->projectionMatrix, renderCamera.projectionMatrix);
        glm_mat4_inv(cameraTransform->worldTransform, renderCamera.viewMatrix);

        ev_log_trace("Initializing new frame : Renderer.startFrame()");
        Renderer.startFrame(&renderCamera);
        ev_log_trace("Finished initializing new frame : Renderer.startFrame()");


        while(ecs_query_next(&it))
        {
          TransformComponent *transformComp = ecs_column(&it, TransformComponent, 1);
          RenderingComponent *renderingComp = ecs_column(&it, RenderingComponent, 2);

          for(int i = 0; i < it.count; ++i)
          {
            for(int primitiveIdx = 0; primitiveIdx < renderingComp[i].meshRenderData.length; ++primitiveIdx)
            {
              Renderer.draw(renderingComp[i].meshRenderData.data[primitiveIdx], transformComp[i].worldTransform);
            }
          }
        }

        ev_log_trace("Ending frame : Renderer.endFrame()");
        Renderer.endFrame();
        ev_log_trace("Ended frame : Renderer.endFrame()");
      }
      World.unlockSceneAccess();

      old = new;
    }
    else
    {
      ev_log_trace("Gameloop going to sleep for %f milliseconds", remainingTime * 1000);
      sleep_ms(remainingTime * 1000);
    }
    ev_log_trace("Finished gameloop iteration");
  }
  ev_log_debug("Exiting game loop");
}

#include <cglm/cglm.h>
void MaintainTransformConstraints(SystemArgs *args)
{
  TransformComponent *parentTransform = ecs_column(args, TransformComponent, 1);
  TransformComponent *transform = ecs_column(args, TransformComponent, 2);

  if(!parentTransform)
    return;

  for(int k = 0; k < args->count; ++k)
  {
    glm_mat4_mul(
        parentTransform->worldTransform,
        transform[k].localTransform,
        transform[k].worldTransform
        );
  }
}

void sandbox()
{
  ev_log_trace("Loading GLTF file");
  /* AssetLoader.loadGLTF("Triangle.gltf"); */
  /* AssetLoader.loadGLTF("Cube.gltf"); */
  /* AssetLoader.loadGLTF("InterpolationTest.gltf"); */
  AssetLoader.loadGLTF("CesiumMilkTruck.gltf");
  /* AssetLoader.loadGLTF("Duck.gltf"); */
  /* AssetLoader.loadGLTF("RiggedFigure.gltf"); */
  /* AssetLoader.loadGLTF("CesiumMan.gltf"); */
  /* AssetLoader.loadGLTF("WaterBottle.gltf"); */
  /* AssetLoader.loadGLTF("SciFiHelmet.gltf"); */
  /* AssetLoader.loadGLTF("Duck.gltf"); */
  ev_log_trace("Loaded GLTF file");


  World.lockSceneAccess();
  ImportModule(TransformModule);
  ImportModule(PhysicsModule);
  ImportModule(CameraModule);

  {
    Entity camera = CreateEntity();
    Entity_SetComponent(camera, CameraComponent, {
        .viewType = EV_CAMERA_PERSPECTIVE_VIEW,
        .hfov = 90,
        .aspectRatio = 1,
        .nearPlane = 0,
        .farPlane = 100,
        });
    TransformComponent * transformComp = Entity_GetComponent_mut(camera, TransformComponent);
    ev_Vector3 cameraPosition = {0, 0, 5};
    glm_mat4_identity(transformComp->worldTransform);
    glm_translate(transformComp->worldTransform, (real*)&cameraPosition);
  }

  Entity sphere = CreateEntity();
  Entity_SetComponent(sphere, EcsName, {"sphere_1"});

  ev_Matrix4 *transform = &(Entity_GetComponent_mut(sphere, TransformComponent)->worldTransform);
  ev_Vector3 position = {0, -10, 0};
  glm_mat4_identity(*transform);
  glm_translate(*transform, (real*)&position);

  Entity_SetComponent(sphere,
    RigidBodyComponent, {
    .mass = 0,
    .restitution = 0.5,
    .collisionShape = Physics.createBox(2, 2, 2),
    });
  World.unlockSceneAccess();
}
