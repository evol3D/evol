#include "stdio.h"
#include "BulletState.h"
#include "PhysicsDebugWindow.h"

#define ev2btVector3(vec_p) \
  (btVector3(vec_p->x, vec_p->y, vec_p->z))

ATTRIBUTE_ALIGNED16(struct)
  EvMotionState : public btMotionState
{
  btTransform m_centerOfMassOffset;
  btTransform m_startWorldTrans;
  ev_Vector3* ev_PositionVector;
  ev_Vector3* ev_RotationVector;

  BT_DECLARE_ALIGNED_ALLOCATOR();

  EvMotionState(btVector3* graphicsVec = 0, const btTransform& startTransform = btTransform::getIdentity(), const btTransform& centerOfMassOffset = btTransform::getIdentity())
    : m_centerOfMassOffset(centerOfMassOffset),
      m_startWorldTrans(startTransform)
  {
  }

  virtual void getWorldTransform(btTransform & centerOfMassWorldTrans) const
  {
    if(ev_PositionVector)
    {
      centerOfMassWorldTrans.setOrigin(ev2btVector3(ev_PositionVector));
    }
    if(ev_RotationVector)
    {
      btQuaternion rot;
      rot.setEulerZYX(
          ev_RotationVector->z,
          ev_RotationVector->y,
          ev_RotationVector->x
          );
      centerOfMassWorldTrans.setRotation(rot);
    }
  }

  virtual void setWorldTransform(const btTransform & centerOfMassWorldTrans)
  {
    if(ev_PositionVector)
    {
      btVector3 pos = centerOfMassWorldTrans.getOrigin();
      ev_PositionVector->x = pos.getX();
      ev_PositionVector->y = pos.getY();
      ev_PositionVector->z = pos.getZ();
    }
    if(ev_RotationVector)
    {
      centerOfMassWorldTrans.getRotation().getEulerZYX(
          ev_RotationVector->x,
          ev_RotationVector->y,
          ev_RotationVector->z
        );
    }
  }
};

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


RigidBodyHandle BulletState::addRigidBody(RigidBody *rb)
{
  /* btTransform startTransform; */
  /* startTransform.setIdentity(); */

  bool isDynamic = rb->mass;

  btVector3 localInertia(0, 0, 0);

  printf("rb->mass = %f\n", rb->mass);

  if(isDynamic)
  {
    ((btCollisionShape*)rb->collisionShape)->calculateLocalInertia(rb->mass, localInertia);
  }

  /* startTransform.setOrigin(ev2btVector3(&(rb->position))); */

  /* btQuaternion rot; */
  /* rot.setEulerZYX( */
  /*     btScalar(ANG2RAD(rb->rotation.z)), */ 
  /*     btScalar(ANG2RAD(rb->rotation.y)), */ 
  /*     btScalar(ANG2RAD(rb->rotation.x)) */
  /* ); */

  /* startTransform.setRotation(rot); */

  EvMotionState *motionState = new EvMotionState();

  motionState->ev_PositionVector = rb->position;
  motionState->ev_RotationVector = rb->rotation;

  btRigidBody::btRigidBodyConstructionInfo rbInfo(rb->mass, motionState, (btCollisionShape*)rb->collisionShape, localInertia);

  btRigidBody* body = new btRigidBody(rbInfo);

  world->addRigidBody(body);

  return body;
}

void BulletState::removeRigidBody(RigidBodyHandle handle)
{
  /* delete ((btRigidBody*)handle)->getMotionState(); */

  /* world->removeRigidBody((btRigidBody*)rb); */
  /* delete (btRigidBody*)rb; */
}


void BulletState::updateRigidBody(RigidBodyHandle handle, RigidBody *rb)
{
  btVector3 inertiaVec = btVector3(0, 0, 0);
  ((btRigidBody*)handle)->setMassProps(rb->mass, inertiaVec);

  if(rb->collisionShape != ((btRigidBody*)handle)->getCollisionShape())
  {
    ((btRigidBody*)handle)->setCollisionShape((btCollisionShape*)rb->collisionShape);
  }

  btTransform newTransform;
  newTransform.setIdentity();
  newTransform.setOrigin(ev2btVector3(rb->position));
  btQuaternion rot;
  /* rot.setEulerZYX( */
  /*     btScalar(ANG2RAD(rb->rotation.z)), */ 
  /*     btScalar(ANG2RAD(rb->rotation.y)), */ 
  /*     btScalar(ANG2RAD(rb->rotation.x)) */
  /* ); */

  newTransform.setRotation(rot);

  ((btRigidBody*)handle)->setWorldTransform(newTransform);
  /* printf("new transform: %f, %f, %f\n", rb->position.x, rb->position.y, rb->position.z); */
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

