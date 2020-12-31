//TODO Comments / Logging
#include <time.h>
#include "Game.h"

#include "App.h"
#include "Physics/Physics.h"
#include "Renderer/Renderer.h"
#include "Renderer/renderer_types.h"
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
void RendererDispatchDrawCalls(SystemArgs *args);

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
  ImportModule(RenderingModule);

  RegisterSystem_OnUpdate(MaintainTransformConstraints, CASCADE: transform.module.TransformComponent, transform.module.TransformComponent);
  RegisterSystem_PostUpdate(RendererDispatchDrawCalls, SHARED: rendering.module.RenderingComponent, transform.module.TransformComponent);
  ev_log_trace("Registered systems");

  ev_log_trace("Finished initializing the game");
  return 0;
}

static int ev_game_deinit()
{
  return 0;
}

void sandbox();

#include <types.h>
static void ev_game_loop()
{
  sandbox();

  ImportModule(CameraModule);
  ImportModule(TransformModule);

  double old = Window.getTime();
  unsigned int physics_steprate = 60;
  double new;
  double waitTime = 1.f/(float)physics_steprate;
  double remainingTime = waitTime;

  while(!App.closeSystem)
  {
    ev_log_trace("Starting gameloop iteration");
    new = Window.getTime();
    double timeStep = new - old;
    old = new;
    remainingTime -= timeStep;

    ev_log_debug("timestep: %f", timeStep);
    ev_log_debug("FPS: %f", 1.f/timeStep);


    if(remainingTime<=0)
    {
      // Fixed Update stuff

      World.lockSceneAccess();
      Physics.step();
      World.unlockSceneAccess();

      remainingTime = waitTime;
    }

    {
      const MainCamera *cam = World_GetComponent(MainCamera);

      const CameraComponent *cameraData         = Entity_GetComponent(cam->entityId, CameraComponent);
      const TransformComponent *cameraTransform = Entity_GetComponent(cam->entityId, TransformComponent);

      ev_RenderCamera renderCamera;

      glm_mat4_dup((vec4*)cameraData->projectionMatrix, renderCamera.projectionMatrix);
      glm_mat4_dup((vec4*)cameraTransform->worldTransform, renderCamera.viewMatrix);
      glm_inv_tr(renderCamera.viewMatrix);

      ev_log_trace("Initializing new frame : Renderer.startFrame()");
      Renderer.startFrame(&renderCamera);
      ev_log_trace("Finished initializing new frame : Renderer.startFrame()");

      World.progress();

      ev_log_trace("Ending frame : Renderer.endFrame()");
      Renderer.endFrame();
      ev_log_trace("Ended frame : Renderer.endFrame()");
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
  //AssetLoader.loadGLTF("DamagedHelmet.gltf");
  //AssetLoader.loadGLTF("DamagedHelmet.gltf");
  //AssetLoader.loadGLTF("CesiumMilkTruck.gltf");
  //AssetLoader.loadGLTF("CesiumMilkTruck.gltf");
   //AssetLoader.loadGLTF("Duck.gltf"); 
  /* AssetLoader.loadGLTF("RiggedFigure.gltf"); */
  AssetLoader.loadGLTF("untitled.gltf");
  /* AssetLoader.loadGLTF("WaterBottle.gltf"); */
  /* AssetLoader.loadGLTF("SciFiHelmet.gltf"); */
  /* AssetLoader.loadGLTF("Duck.gltf"); */
  ev_log_trace("Loaded GLTF file");

  Renderer.registerBuffer(MATERIAL_BUFFER, MaterialSystem.getMaterials().data, MaterialSystem.getMaterials().length * sizeof(Material));

  MaterialSystem.getMaterials().length;

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
        .nearPlane = 0.01,
        .farPlane = 100,
        });
    TransformComponent * transformComp = Entity_GetComponent_mut(camera, TransformComponent);
    ev_Vector3 cameraPosition = {0, 0, 8};
    glm_mat4_identity(transformComp->worldTransform);
    glm_translate(transformComp->worldTransform, (real*)&cameraPosition);

    /* Entity_SetComponent(camera, */
    /*   RigidBodyComponent, { */
    /*   .mass = 1, */
    /*   .restitution = 0.5, */
    /*   .collisionShape = Physics.createSphere(1), */
    /*   }); */
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

void RendererDispatchDrawCalls(SystemArgs *args)
{
  RenderingComponent *renderingComp = ecs_column(args, RenderingComponent, 1);
  TransformComponent *transformComp = ecs_column(args, TransformComponent, 2);

  for(int i = 0; i < args->count; ++i)
  {
    Renderer.draw(renderingComp->meshRenderData, transformComp[i].worldTransform);
  }
}
