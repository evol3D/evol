//TODO Comments / Logging
#include "World/modules/physics_module.h"
#include "World/modules/transform_module.h"
#include "cglm/affine.h"
#include "cglm/call/io.h"
#include "cglm/cam.h"
#include "cglm/euler.h"
#include "cglm/io.h"
#include "cglm/mat4.h"
#include "cglm/quat.h"
#include "cglm/vec3.h"
#include "cglm/vec4.h"
#include <time.h>
#include "Game.h"

#include "App.h"
#include "Physics/Physics.h"
#include "Renderer/Renderer.h"
#include "Renderer/renderer_types.h"
#include "Window.h"
#include "World/World.h"
#include "World/WorldModules.h"

#include "flecs.h"

#include <cglm/cglm.h>
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

static void ev_game_setplayer(Entity p);

struct ev_Game Game = {
  .init      = ev_game_init,
  .deinit    = ev_game_deinit,
  .loop      = ev_game_loop,
  .setPlayer = ev_game_setplayer,
};

struct ev_Game_Data {
  Entity player;
} GameData;

struct {
  float verticalAxis;
  float horizontalAxis;
  float cameraVerticalAxis;
  float cameraHorizontalAxis;
  float rotationSpeed;
  float engineForce;
  PhysicsVehicle physicsVehicle;
} PlayerData;

static void ev_game_setplayer(Entity p)
{
  GameData.player = p;
  PlayerData.verticalAxis = 0;
  PlayerData.horizontalAxis = 0;
  PlayerData.cameraVerticalAxis = 1;
  PlayerData.cameraHorizontalAxis = 0;
  PlayerData.engineForce = 3000;
}

static int ev_game_init()
{
  ev_log_trace("Started initializing the game");

  ev_log_trace("Starting a new scene");
  World.newScene();
  ev_log_trace("New scene started");

  ev_log_trace("Registering systems");


  DEFINE_GLOBAL_COMPONENTS(World.getInstance());
  REGISTER_GLOBAL_SYSTEMS(World.getInstance());

  RegisterSystem_OnUpdate(MaintainTransformConstraints, CASCADE: TransformComponent, TransformComponent);
  RegisterSystem_PostUpdate(RendererDispatchDrawCalls, SHARED: RenderingComponent, TransformComponent);
  ev_log_trace("Registered systems");

  ev_log_trace("Finished initializing the game");
  return 0;
}

static int ev_game_deinit()
{
  return 0;
}

void physicsLoop()
{
  double old = Window.getTime();
  double new;
  unsigned int physics_steprate = 240;
  double waitTime = 1.f/(float)physics_steprate;
  double remainingTime = waitTime;
  while(!App.closeSystem) {
    new = Window.getTime();
    double timeStep = new - old;
    old = new;
    remainingTime -= timeStep;

    if(remainingTime<=0) {
      // Fixed Update stuff

      World.lockSceneAccess();
      Physics.step_dt(waitTime - remainingTime);
      World.unlockSceneAccess();

      remainingTime = waitTime;
    }
    else {
      sleep_ms(remainingTime * 1000);
    }
  }
}

void sandbox();

#include <types.h>
static void ev_game_loop()
{
  sandbox();
  pthread_t physicsThread;
  pthread_create(&physicsThread, NULL, physicsLoop, NULL);

  double old = Window.getTime();
  double new;

  while(!App.closeSystem)
  {
    ev_log_trace("Starting gameloop iteration");
    new = Window.getTime();
    double timeStep = new - old;
    old = new;

    ev_log_debug("timestep: %f", timeStep);
    ev_log_debug("FPS: %f", 1.f/timeStep);
    /* printf("FPS: %f\n", 1.f/timeStep); */

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
  pthread_join(physicsThread, NULL);
  ev_log_debug("Exiting game loop");
}

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

void sphereOnUpdate(unsigned int entt)
{
}

void glm_mat4_forward(const vec4* m, float *v)
{
  ev_Vector4 forwardQuat;
  glm_mat4_quat((vec4*)m, (float*)&forwardQuat);
  v[0] = 2 * (forwardQuat.x * forwardQuat.z + forwardQuat.w * forwardQuat.y);
  v[1] = 2 * (forwardQuat.y * forwardQuat.z - forwardQuat.w * forwardQuat.x);
  v[2] = 1 - 2 * (forwardQuat.x * forwardQuat.x + forwardQuat.y * forwardQuat.y);
  v[0] *= -1;
  v[2] *= -1;
}

void glm_mat4_left(const vec4* m, float *v)
{
  ev_Vector4 forwardQuat;
  glm_mat4_quat((vec4*)m, (float*)&forwardQuat);
  v[0] = 1 - 2 * (forwardQuat.y*forwardQuat.y + forwardQuat.z*forwardQuat.z);
  v[1] = 2 * (forwardQuat.x*forwardQuat.y + forwardQuat.w*forwardQuat.z);
  v[2] = 2 * (forwardQuat.x*forwardQuat.z - forwardQuat.w*forwardQuat.y);
  v[0] *= -1;
}

void glm_lookdir(ev_Matrix4 eyeMat, ev_Matrix4 targetMat)
{
  ev_Vector4 oldRotationQuat; glm_mat4_quat(eyeMat, (float*)&oldRotationQuat);
  ev_Vector4 targetRotationQuat; glm_mat4_quat(targetMat, (float*)&targetRotationQuat);

  ev_Vector4 oldRotationQuatInv;
  glm_quat_inv((float*)&oldRotationQuat, (float*)&oldRotationQuatInv);
  ev_Vector4 deltaRotationQuat;
  glm_quat_mul((float*)&oldRotationQuatInv, (float*)&targetRotationQuat, (float*)&deltaRotationQuat);

  glm_quat_rotate(eyeMat, (float*)&deltaRotationQuat, eyeMat);
}

void glm_lookdir_lerp(ev_Matrix4 eyeMat, ev_Matrix4 targetMat, real damping)
{
  ev_Vector4 oldRotationQuat; glm_mat4_quat(eyeMat, (float*)&oldRotationQuat);
  ev_Vector4 targetRotationQuat; glm_mat4_quat(targetMat, (float*)&targetRotationQuat);

  ev_Vector4 oldRotationQuatInv;
  glm_quat_inv((float*)&oldRotationQuat, (float*)&oldRotationQuatInv);
  ev_Vector4 deltaRotationQuat;
  glm_quat_mul((float*)&oldRotationQuatInv, (float*)&targetRotationQuat, (float*)&deltaRotationQuat);

  ev_Vector4 smoothedRotationQuat;
  glm_quat_slerp(GLM_QUAT_IDENTITY, (float*)&deltaRotationQuat, damping, (float*)&smoothedRotationQuat);

  glm_quat_rotate(eyeMat, (float*)&smoothedRotationQuat, eyeMat);
}

void glm_looktarget(ev_Matrix4 eyeMat, ev_Matrix4 targetMat, ev_Vector3 *upVec)
{
  ev_Vector3 *eyePosition = (ev_Vector3*)eyeMat[3];
  ev_Vector3 *targetPosition = (ev_Vector3*)targetMat[3];

  ev_Vector4 newRotationQuat;
  ev_Vector4 oldRotationQuat;
  glm_mat4_quat(eyeMat, (float*)&oldRotationQuat);
  glm_quat_forp((float*)eyePosition, (float*)targetPosition, (float*)upVec, (float*)&newRotationQuat);
  ev_Vector4 oldRotationQuatInv;
  glm_quat_inv((float*)&oldRotationQuat, (float*)&oldRotationQuatInv);
  ev_Vector4 deltaRotationQuat;
  glm_quat_mul((float*)&oldRotationQuatInv, (float*)&newRotationQuat, (float*)&deltaRotationQuat);

  glm_quat_rotate(eyeMat, (float*)&deltaRotationQuat, eyeMat);
}

void glm_looktarget_lerp(ev_Matrix4 eyeMat, ev_Matrix4 targetMat, ev_Vector3 *upVec, real damping)
{
  ev_Vector3 *eyePosition = (ev_Vector3*)eyeMat[3];
  ev_Vector3 *targetPosition = (ev_Vector3*)targetMat[3];

  ev_Vector4 newRotationQuat;
  ev_Vector4 oldRotationQuat;
  glm_mat4_quat(eyeMat, (float*)&oldRotationQuat);
  glm_quat_forp((float*)eyePosition, (float*)targetPosition, (float*)upVec, (float*)&newRotationQuat);
  ev_Vector4 oldRotationQuatInv;
  glm_quat_inv((float*)&oldRotationQuat, (float*)&oldRotationQuatInv);
  ev_Vector4 deltaRotationQuat;
  glm_quat_mul((float*)&oldRotationQuatInv, (float*)&newRotationQuat, (float*)&deltaRotationQuat);

  ev_Vector4 smoothedRotationQuat;
  glm_quat_lerp(GLM_QUAT_IDENTITY, (float*)&deltaRotationQuat, damping, (float*)&smoothedRotationQuat);
  glm_quat_normalize(&smoothedRotationQuat);

  glm_quat_rotate(eyeMat, (float*)&smoothedRotationQuat, eyeMat);
}

void cameraOnFixedUpdate(unsigned int entt)
{
  // Follow parameters
  float cameraHeight = 2.f;
  float cameraDistance = 10.f;
  float heightDamping = 0.10f;
  float positionDamping = 0.10f;

  TransformComponent *transComp = Entity_GetComponent_mut(entt, TransformComponent);
  TransformComponent *playerTransComp = (TransformComponent *)Entity_GetComponent(GameData.player, TransformComponent);

  ev_Vector3 cameraEulerAngles, playerEulerAngles;
  glm_euler_angles(transComp->worldTransform, (float*)&cameraEulerAngles);
  glm_euler_angles(playerTransComp->worldTransform, (float*)&playerEulerAngles);

  ev_Vector3 *cameraPosition = (ev_Vector3*)transComp->worldTransform[3];
  ev_Vector3 *playerPosition = (ev_Vector3*)playerTransComp->worldTransform[3];

  float targetHeight = playerPosition->y + cameraHeight;

  float currentHeight = cameraPosition->y;

  currentHeight = glm_lerp(currentHeight, targetHeight, heightDamping);


  ev_Vector3 upVec = {0, 1, 0};

  ev_Vector3 reverseVec = {0, 0, 0};
  ev_Vector3 forwardVec;
  glm_mat4_forward(playerTransComp->worldTransform, (float*)&forwardVec);
  glm_vec3_scale(&forwardVec, PlayerData.cameraVerticalAxis, &forwardVec);
  glm_vec3_add(&reverseVec, &forwardVec, &reverseVec);

  ev_Vector3 leftVec;
  glm_mat4_left(playerTransComp->worldTransform, (float*)&leftVec);
  glm_vec3_scale(&leftVec, PlayerData.cameraHorizontalAxis, &leftVec);
  glm_vec3_add(&reverseVec, &leftVec, &reverseVec);

  ev_Vector3 offset;
  glm_vec3_scale((float*)&reverseVec, cameraDistance, (float*)&offset);

  ev_Vector3 targetPosition = *playerPosition;
  glm_vec3_sub((float*)&targetPosition, (float*)&offset, (float*)&targetPosition);
  targetPosition.y = currentHeight;

  glm_vec3_lerp((float*)cameraPosition, (float*)&targetPosition, positionDamping, (float*)cameraPosition);

  glm_looktarget(transComp->worldTransform, playerTransComp->worldTransform, &upVec);

  ecs_modified(World.getInstance(), entt, TransformComponent);
}

void cameraOnUpdate(unsigned int entt)
{
}


void playerOnFixedUpdate(unsigned int entt)
{
  Physics.applyEngineForce(PlayerData.physicsVehicle, PlayerData.engineForce * PlayerData.verticalAxis);

  Physics.setVehicleSteering(PlayerData.physicsVehicle, PlayerData.horizontalAxis);
}

DECLARE_EVENT_HANDLER(PlayerKeyHandler, (KeyEvent *keyEvent) {
  if(keyEvent->type == EVENT_TYPE(KeyPressedEvent)) {
    KeyPressedEvent *event = (KeyPressedEvent *) keyEvent;
    switch(event->keyCode) {
      case KEY_W:
      PlayerData.verticalAxis += 1;
      break;
      case KEY_S:
      PlayerData.verticalAxis -= 1;
      break;
      case KEY_A:
      PlayerData.horizontalAxis -= 1;
      break;
      case KEY_D:
      PlayerData.horizontalAxis += 1;
      break;
      case KEY_UP_ARROW:
      PlayerData.cameraVerticalAxis += 1;
      break;
      case KEY_DOWN_ARROW:
      PlayerData.cameraVerticalAxis -= 2;
      break;
      case KEY_LEFT_ARROW:
      PlayerData.cameraHorizontalAxis -= 1;
      PlayerData.cameraVerticalAxis -= 1;
      break;
      case KEY_RIGHT_ARROW:
      PlayerData.cameraHorizontalAxis += 1;
      PlayerData.cameraVerticalAxis -= 1;
      break;
      default:
      break;
    }

  } else if(keyEvent->type == EVENT_TYPE(KeyReleasedEvent)) {
    KeyReleasedEvent *event = (KeyReleasedEvent *) keyEvent;

    switch(event->keyCode) {
      case KEY_W:
      PlayerData.verticalAxis -= 1;
      break;
      case KEY_S:
      PlayerData.verticalAxis += 1;
      break;
      case KEY_A:
      PlayerData.horizontalAxis += 1;
      break;
      case KEY_D:
      PlayerData.horizontalAxis -= 1;
      break;
      case KEY_UP_ARROW:
      PlayerData.cameraVerticalAxis -= 1;
      break;
      case KEY_DOWN_ARROW:
      PlayerData.cameraVerticalAxis += 2;
      break;
      case KEY_LEFT_ARROW:
      PlayerData.cameraHorizontalAxis += 1;
      PlayerData.cameraVerticalAxis += 1;
      break;
      case KEY_RIGHT_ARROW:
      PlayerData.cameraHorizontalAxis -= 1;
      PlayerData.cameraVerticalAxis += 1;
      break;
      case KEY_R:
      {
        TransformComponent *playerTransform = Entity_GetComponent_mut(GameData.player, TransformComponent);
        ev_Vector3 *playerPosition = playerTransform->worldTransform[3];

        playerPosition->x = 10;
        playerPosition->y = 10;
        playerPosition->z = -20;

        ev_Vector4 playerRotation; glm_mat4_quat(playerTransform->worldTransform, &playerRotation);
        ev_Vector4 targetRotation; glm_quat_inv(&playerRotation, &targetRotation);

        glm_quat_rotate(playerTransform->worldTransform, &targetRotation, playerTransform->worldTransform);

        ecs_modified(World.getInstance(), GameData.player, TransformComponent);
      }
      break;
      default:
      break;
    }
  }

});

void sandbox()
{
  ev_log_trace("Loading GLTF file");
  AssetLoader.loadGLTF("Car.gltf");
  AssetLoader.loadGLTF("map.gltf");
  ev_log_trace("Loaded GLTF file");

  World.lockSceneAccess();

  {
    Entity camera = CreateEntity();
    Entity_SetComponent(camera, CScriptOnUpdate, {cameraOnUpdate});
    Entity_SetComponent(camera, CScriptOnFixedUpdate, {cameraOnFixedUpdate, 0});
    Entity_SetComponent(camera, CameraComponent, {
        .viewType = EV_CAMERA_PERSPECTIVE_VIEW,
        .hfov = 90,
        .aspectRatio = 1,
        .nearPlane = 0.01,
        .farPlane = 1000,
        });
    TransformComponent * transformComp = Entity_GetComponent_mut(camera, TransformComponent);
    ev_Vector3 cameraPosition = {0, 0, 15};
    glm_mat4_identity(transformComp->worldTransform);
    glm_translate(transformComp->worldTransform, (real*)&cameraPosition);
    Entity_SetComponent(camera,
      RigidBodyComponent, {
      .mass = 0,
      .restitution = 0.5,
      .collisionShape = Physics.createBox(0.1, 0.1, 0.1),
      .type = EV_RIGIDBODY_KINEMATIC,
      });
  }

  /* Entity sphere = CreateEntity(); */
  /* Entity_SetComponent(sphere, EcsName, {"sphere_1"}); */

  /* Entity_SetComponent(sphere, CScriptOnUpdate, {sphereOnUpdate}); */

  /* ev_Matrix4 *transform = &(Entity_GetComponent_mut(sphere, TransformComponent)->worldTransform); */
  /* ev_Vector3 position = {0, -10, 0}; */
  /* glm_mat4_identity(*transform); */
  /* glm_translate(*transform, (real*)&position); */

  /* Entity_SetComponent(sphere, */
  /*   RigidBodyComponent, { */
  /*   .mass = 0, */
  /*   .restitution = 0.0, */
  /*   .collisionShape = Physics.createBox(100, 4, 100), */
  /*   }); */

  Entity_SetComponent(GameData.player, CScriptOnFixedUpdate, {playerOnFixedUpdate, 0});

  {
    RigidBodyHandle chassisRigidbody = Entity_GetComponent_mut(GameData.player, RigidBodyHandleComponent)->handle;
    PlayerData.physicsVehicle = Physics.createRaycastVehicle(chassisRigidbody);
  }


  World.unlockSceneAccess();

  ACTIVATE_EVENT_HANDLER(PlayerKeyHandler, KeyEvent);
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
