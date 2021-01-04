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
  float rotationSpeed;
  float engineForce;
  Entity frontWheels;
  Entity backWheels;
  Entity frontRightWheel;
  Entity frontLeftWheel;
  Entity backRightWheel;
  Entity backLeftWheel;
} PlayerData;

static void ev_game_setplayer(Entity p)
{
  GameData.player = p;
  PlayerData.verticalAxis = 0;
  PlayerData.horizontalAxis = 0;
  PlayerData.engineForce = 30;
  PlayerData.rotationSpeed = 1.0f;
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

void sandbox();

#include <types.h>
static void ev_game_loop()
{
  sandbox();

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
    /* printf("FPS: %f\n", 1.f/timeStep); */


    if(remainingTime<=0)
    {
      // Fixed Update stuff

      World.lockSceneAccess();
      Physics.step_dt(waitTime - remainingTime);
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
  glm_quat_lerp(GLM_QUAT_IDENTITY, (float*)&deltaRotationQuat, damping, (float*)&smoothedRotationQuat);

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

  glm_quat_rotate(eyeMat, (float*)&smoothedRotationQuat, eyeMat);
}

void cameraOnFixedUpdate(unsigned int entt)
{
  // Follow parameters
  float cameraHeight = 5.f;
  float cameraDistance = 10.f;
  float heightDamping = 0.03f;
  float rotationDamping = 0.03f;
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
  ev_Vector3 forwardVec;
  glm_mat4_forward(playerTransComp->worldTransform, (float*)&forwardVec);

  ev_Vector3 offset;
  glm_vec3_scale((float*)&forwardVec, cameraDistance, (float*)&offset);

  ev_Vector3 targetPosition = *playerPosition;
  glm_vec3_sub((float*)&targetPosition, (float*)&offset, (float*)&targetPosition);
  targetPosition.y = currentHeight;

  glm_vec3_lerp((float*)cameraPosition, (float*)&targetPosition, positionDamping, (float*)cameraPosition);

  glm_looktarget_lerp(transComp->worldTransform, playerTransComp->worldTransform, &upVec, rotationDamping);

  ecs_modified(World.getInstance(), entt, TransformComponent);
}

void cameraOnUpdate(unsigned int entt)
{
}


void playerOnFixedUpdate(unsigned int entt)
{
  float downForce = 100;
  ev_Vector3 upVec = {0, 1, 0};
  float wheelRotationDamping = 0.3f;

  RigidBodyHandle handle = Entity_GetComponent(entt, RigidBodyHandleComponent)->handle;
  RigidBodyHandle frontRightWheelRigidbody = Entity_GetComponent(PlayerData.frontRightWheel, RigidBodyHandleComponent)->handle;
  RigidBodyHandle frontLeftWheelRigidbody = Entity_GetComponent(PlayerData.frontLeftWheel, RigidBodyHandleComponent)->handle;
  RigidBodyHandle backRightWheelRigidbody = Entity_GetComponent(PlayerData.backRightWheel, RigidBodyHandleComponent)->handle;
  RigidBodyHandle backLeftWheelRigidbody = Entity_GetComponent(PlayerData.backLeftWheel, RigidBodyHandleComponent)->handle;
  const TransformComponent *playerTransform = Entity_GetComponent(entt, TransformComponent);

  Entity wheels[] = {
    PlayerData.frontLeftWheel,
    PlayerData.frontRightWheel,
    PlayerData.backLeftWheel,
    PlayerData.backRightWheel
  };

  ev_Vector3 playerAngularVelocity;
  Physics.getAngularVelocity(handle, &playerAngularVelocity);

  for(int i = 0; i < ARRAYSIZE(wheels); i++) {
    TransformComponent *wheelTransform = Entity_GetComponent_mut(wheels[i], TransformComponent);

    RigidBodyHandle wheelRigidbody = Entity_GetComponent(wheels[i], RigidBodyHandleComponent)->handle;
    ev_Vector3 playerEuler; glm_euler_angles(playerTransform->worldTransform, &playerEuler);
    ev_Vector3 wheelEuler; glm_euler_angles(wheelTransform->worldTransform, &wheelEuler);

    ev_Vector3 targetEuler = {
      .x = wheelEuler.x,
      .y = playerEuler.y,
      .z = playerEuler.z,
    };

    ev_Matrix4 targetRotationMat; glm_euler(&targetEuler, targetRotationMat);
    ev_Vector4 targetRotationQuat; glm_mat4_quat(targetRotationMat, &targetRotationQuat);

    /* ev_Vector4 smoothedRotationQuat; glm_quat_lerp(GLM_QUAT_IDENTITY, &targetRotationQuat, 0.01, &smoothedRotationQuat); */

    /* Physics.setRotation(wheelRigidbody, &smoothedRotationQuat); */
    Physics.setRotation(wheelRigidbody, &targetRotationQuat);

    /* ev_Vector3 targetVelocity = { */
    /*   .x = (targetEuler.x - wheelEuler.x) * wheelRotationDamping, */
    /*   .y = (targetEuler.y - wheelEuler.y) * wheelRotationDamping, */
    /*   .z = (targetEuler.z - wheelEuler.z) * wheelRotationDamping, */
    /* }; */
    /* ev_Matrix4 newWheelRotationMat; glm_euler(&playerEuler, newWheelRotationMat); */

    /* ev_Vector3 wheelAngularVelocity; Physics.getAngularVelocity(wheelRigidbody, &wheelAngularVelocity); */
    /* wheelAngularVelocity.y = playerAngularVelocity.y; */
    /* wheelAngularVelocity.z = playerAngularVelocity.z; */
    /* Physics.setAngularVelocity(wheelRigidbody, &targetVelocity); */

    /* glm_lookdir_lerp(Entity_GetComponent_mut(wheels[i], TransformComponent)->worldTransform, playerTransform->worldTransform, 0.05); */
    /* ecs_modified(World.getInstance(), wheels[i], TransformComponent); */
  }

  ev_Vector3 downForceVec = {0, -downForce, 0};
  Physics.applyForce(handle, &downForceVec);


  if(PlayerData.verticalAxis) {
    ev_Vector3 frontRightWheelAngularVelocity; Physics.getAngularVelocity(frontRightWheelRigidbody, &frontRightWheelAngularVelocity);
    ev_Vector3 frontLeftWheelAngularVelocity ; Physics.getAngularVelocity(frontLeftWheelRigidbody , &frontLeftWheelAngularVelocity)  ;
    ev_Vector3 backRightWheelAngularVelocity ; Physics.getAngularVelocity(frontRightWheelRigidbody, &backRightWheelAngularVelocity);
    ev_Vector3 backLeftWheelAngularVelocity  ; Physics.getAngularVelocity(frontLeftWheelRigidbody , &backLeftWheelAngularVelocity);

    frontRightWheelAngularVelocity.x = PlayerData.verticalAxis * PlayerData.engineForce * -1;
    frontRightWheelAngularVelocity.y = 0;
    frontRightWheelAngularVelocity.z = 0;
    frontLeftWheelAngularVelocity.x = PlayerData.verticalAxis * PlayerData.engineForce * -1;
    frontLeftWheelAngularVelocity.y = 0;
    frontLeftWheelAngularVelocity.z = 0;
    backRightWheelAngularVelocity.x = PlayerData.verticalAxis * PlayerData.engineForce * -1;
    backRightWheelAngularVelocity.y = 0;
    backRightWheelAngularVelocity.z = 0;
    backLeftWheelAngularVelocity.x = PlayerData.verticalAxis * PlayerData.engineForce * -1;
    backLeftWheelAngularVelocity.y = 0;
    backLeftWheelAngularVelocity.z = 0;

    Physics.setAngularVelocity(frontRightWheelRigidbody, &frontRightWheelAngularVelocity);
    Physics.setAngularVelocity(frontLeftWheelRigidbody , &frontLeftWheelAngularVelocity);
    Physics.setAngularVelocity(backRightWheelRigidbody , &backRightWheelAngularVelocity);
    Physics.setAngularVelocity(backLeftWheelRigidbody  , &backLeftWheelAngularVelocity);
  }

  if(PlayerData.horizontalAxis) {
    /* ev_Vector3 velocity = { */
    /*   .x = 0, */
    /*   .y = PlayerData.rotationSpeed * PlayerData.horizontalAxis * -1 * (PlayerData.verticalAxis?PlayerData.verticalAxis:1), */
    /*   .z = 0, */
    /* }; */
    /* Physics.setAngularVelocity(handle, &velocity); */
  }
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
      default:
      break;
    }
  }

});

void sandbox()
{
  ev_log_trace("Loading GLTF file");
  /* AssetLoader.loadGLTF("Triangle.gltf"); */
  /* AssetLoader.loadGLTF("Cube.gltf"); */
  /* AssetLoader.loadGLTF("InterpolationTest.gltf"); */
  AssetLoader.loadGLTF("Truck.gltf");
  AssetLoader.loadGLTF("map.gltf");
  /* AssetLoader.loadGLTF("terrain.gltf"); */
  /* AssetLoader.loadGLTF("DamagedHelmet.gltf"); */
  // AssetLoader.loadGLTF("Box.gltf");
  /* AssetLoader.loadGLTF("Duck.gltf"); */
  /* AssetLoader.loadGLTF("RiggedFigure.gltf"); */
  /* AssetLoader.loadGLTF("CesiumMan.gltf"); */
  /* AssetLoader.loadGLTF("WaterBottle.gltf"); */
  /* AssetLoader.loadGLTF("SciFiHelmet.gltf"); */
  /* AssetLoader.loadGLTF("Duck.gltf"); */
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
  /*   .restitution = 1.0, */
  /*   .collisionShape = Physics.createBox(100, 4, 100), */
  /*   }); */

  Entity_SetComponent(GameData.player, CScriptOnFixedUpdate, {playerOnFixedUpdate, 0});

  RigidBodyHandle handle = Entity_GetComponent(GameData.player, RigidBodyHandleComponent)->handle;
  Physics.setFriction(handle, 0.1f);
  Physics.setDamping(handle, 0.2f, 0.7f);
  TransformComponent *playerTransform = Entity_GetComponent_mut(GameData.player, TransformComponent);
  ev_Vector3 *playerPosition = (ev_Vector3 *)(playerTransform->worldTransform)[3];
  playerPosition->y += 20.f;
  playerPosition->x += 5.f;
  ecs_modified(World.getInstance(), GameData.player, TransformComponent);

  {

    RigidBodyHandle *chassisRigidbody = Entity_GetComponent_mut(GameData.player, RigidBodyHandleComponent)->handle;
    CollisionShape wheelShape = Physics.createCylinderX(0.2, 1, 1);

    ev_Vector3 parentAxis = {0.f, 1.f, 0.f};
    ev_Vector3 childAxis  = {1.f, 0.f, 0.f};

    Entity frontRightWheel = CreateEntity();

    TransformComponent *frontRightWheelTransform = Entity_GetComponent_mut(frontRightWheel, TransformComponent);

    glm_mat4_identity(frontRightWheelTransform->worldTransform);

    ev_Vector3 frontRightWheelPosition = {
      .x = playerPosition->x + 2.5,
      .y = playerPosition->y - 1,
      .z = playerPosition->z - 1.5,
    };

    glm_translate(frontRightWheelTransform->worldTransform, (float*)&frontRightWheelPosition);

    ecs_modified(World.getInstance(), frontRightWheel, TransformComponent);

    Entity_SetComponent(frontRightWheel, RigidBodyComponent, {
      .type = EV_RIGIDBODY_DYNAMIC,
      .mass = 20.f,
      .restitution = 0.1f,
      .collisionShape = wheelShape,
    });

    RigidBodyHandle *frontRightWheelRigidbody = Entity_GetComponent_mut(frontRightWheel, RigidBodyHandleComponent)->handle;
    Physics.setFriction(frontRightWheelRigidbody, 100);
    Physics.addHingeConstraint(chassisRigidbody, frontRightWheelRigidbody, &frontRightWheelPosition, &parentAxis, &childAxis);
    PlayerData.frontRightWheel = frontRightWheel;

    Entity frontLeftWheel  = CreateEntity();
    TransformComponent *frontLeftWheelTransform  = Entity_GetComponent_mut(frontLeftWheel , TransformComponent);
    glm_mat4_identity(frontLeftWheelTransform->worldTransform);
    ev_Vector3 frontLeftWheelPosition  = {
      .x = playerPosition->x - 2.5,
      .y = playerPosition->y - 1,
      .z = playerPosition->z - 1.5,
    };

    glm_translate(frontLeftWheelTransform->worldTransform , (float*)&frontLeftWheelPosition );
    ecs_modified(World.getInstance(), frontLeftWheel , TransformComponent);

    Entity_SetComponent(frontLeftWheel, RigidBodyComponent, {
      .type = EV_RIGIDBODY_DYNAMIC,
      .mass = 20.f,
      .restitution = 0.1f,
      .collisionShape = wheelShape,
    });

    RigidBodyHandle *frontLeftWheelRigidbody  = Entity_GetComponent_mut(frontLeftWheel, RigidBodyHandleComponent)->handle;
    Physics.setFriction(frontLeftWheelRigidbody, 100);
    Physics.addHingeConstraint(chassisRigidbody, frontLeftWheelRigidbody , &frontLeftWheelPosition , &parentAxis, &childAxis);
    PlayerData.frontLeftWheel  = frontLeftWheel;

    Entity backRightWheel  = CreateEntity();
    TransformComponent *backRightWheelTransform = Entity_GetComponent_mut(backRightWheel, TransformComponent);
    glm_mat4_identity(backRightWheelTransform->worldTransform);
    ev_Vector3 backRightWheelPosition  = {
      .x = playerPosition->x + 2.5,
      .y = playerPosition->y - 1,
      .z = playerPosition->z + 1.5,
    };
    glm_translate(backRightWheelTransform->worldTransform , (float*)&backRightWheelPosition );
    ecs_modified(World.getInstance(), backRightWheel , TransformComponent);
    Entity_SetComponent(backRightWheel, RigidBodyComponent, {
      .type = EV_RIGIDBODY_DYNAMIC,
      .mass = 20.f,
      .restitution = 0.1f,
      .collisionShape = wheelShape,
    });
    RigidBodyHandle *backRightWheelRigidbody  = Entity_GetComponent_mut(backRightWheel, RigidBodyHandleComponent)->handle;
    Physics.setFriction(backRightWheelRigidbody, 1000);
    Physics.addHingeConstraint(chassisRigidbody, backRightWheelRigidbody , &backRightWheelPosition , &parentAxis, &childAxis);
    PlayerData.backRightWheel  = backRightWheel;

    Entity backLeftWheel   = CreateEntity();
    TransformComponent *backLeftWheelTransform  = Entity_GetComponent_mut(backLeftWheel , TransformComponent);
    glm_mat4_identity(backLeftWheelTransform->worldTransform);
    ev_Vector3 backLeftWheelPosition   = {
      .x = playerPosition->x - 2.5,
      .y = playerPosition->y - 1,
      .z = playerPosition->z + 1.5,
    };
    glm_translate(backLeftWheelTransform->worldTransform  , (float*)&backLeftWheelPosition  );
    ecs_modified(World.getInstance(), backLeftWheel  , TransformComponent);
    Entity_SetComponent(backLeftWheel, RigidBodyComponent, {
      .type = EV_RIGIDBODY_DYNAMIC,
      .mass = 20.f,
      .restitution = 0.1f,
      .collisionShape = wheelShape,
    });
    RigidBodyHandle *backLeftWheelRigidbody   = Entity_GetComponent_mut(backLeftWheel, RigidBodyHandleComponent)->handle;
    Physics.addHingeConstraint(chassisRigidbody, backLeftWheelRigidbody  , &backLeftWheelPosition  , &parentAxis, &childAxis);
    Physics.setFriction(backLeftWheelRigidbody, 1000);
    PlayerData.backLeftWheel   = backLeftWheel;
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
