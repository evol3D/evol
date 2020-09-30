#include "stdio.h"
#include "BulletState.h"
#include "PhysicsDebugWindow.h"

BulletState::BulletState()
{
  collisionConfiguration = new btDefaultCollisionConfiguration();
  collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
  broadphase = new btDbvtBroadphase();
  constraintSolver = new btSequentialImpulseConstraintSolver();
  world = new btDiscreteDynamicsWorld(collisionDispatcher, broadphase, constraintSolver, collisionConfiguration);
}

BulletState::~BulletState()
{
  PhysicsDebugWindow::deinit();

  clearCollisionObjects();

  delete world;
  delete constraintSolver;
  delete broadphase;
  delete collisionDispatcher;
  delete collisionConfiguration;
}

void BulletState::clearCollisionObjects()
{
  for(int i = world->getNumCollisionObjects()-1; i >=0 ; i--)
  {
    btCollisionObject *collisionObject = world->getCollisionObjectArray()[i];
    btRigidBody *rigidBody = btRigidBody::upcast(collisionObject);

    if(rigidBody && rigidBody->getMotionState())
    {
      delete rigidBody->getMotionState();
    }

    world->removeCollisionObject(collisionObject);
    delete collisionObject;
  }
}

void BulletState::visualize()
{
  if(visualDebugging)
    return;

  PhysicsDebugWindow::init(world);
  visualDebugging = true;
}

CollisionShape BulletState::createBox(real x, real y, real z)
{
  return new btBoxShape(btVector3(btScalar(x), btScalar(y), btScalar(z)));
}

CollisionShape BulletState::createSphere(real r)
{
  return new btSphereShape(btScalar(r));
}

btVector3 PhysicsPosition2btVector3(PhysicsPosition *position)
{
  return btVector3(
      position->x,
      position->y,
      position->z
  );
}

void BulletState::addRigidBody(RigidBody *rb)
{
  btTransform startTransform;
  startTransform.setIdentity();

  bool isDynamic = rb->mass;

  btVector3 localInertia(0, 0, 0);

  if(isDynamic)
  {
    ((btCollisionShape*)rb->collisionShape)->calculateLocalInertia(rb->mass, localInertia);
  }

  startTransform.setOrigin(PhysicsPosition2btVector3(&(rb->position)));

  btDefaultMotionState *motionState = new btDefaultMotionState(startTransform);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(rb->mass, motionState, (btCollisionShape*)rb->collisionShape, localInertia);

  btRigidBody* body = new btRigidBody(rbInfo);

  world->addRigidBody(body);
}

void BulletState::setGravity(real x, real y, real z)
{
  world->setGravity(btVector3(btScalar(x), btScalar(y), btScalar(z)));
}

void BulletState::step()
{
  world->stepSimulation(0.3167f, 10);
}

void BulletState::step_dt(real dt)
{
  world->stepSimulation(dt, 10);
}
