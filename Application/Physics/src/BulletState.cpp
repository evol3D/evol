#include "stdio.h"
#include "BulletState.h"
#include "PhysicsDebugWindow.h"

#define ev2btVector3(vec_p) \
  (btVector3(vec_p->x, vec_p->y, vec_p->z))

ATTRIBUTE_ALIGNED16(struct)
EvMotionState : public btMotionState
{
  unsigned int entt_id;
  BT_DECLARE_ALIGNED_ALLOCATOR();

  EvMotionState(btVector3* graphicsVec = 0, const btTransform& startTransform = btTransform::getIdentity(), const btTransform& centerOfMassOffset = btTransform::getIdentity())
  {
  }

  void getWorldTransform(btTransform & centerOfMassWorldTrans) const
  {
    const ev_Vector3 *ev_PositionVector = entity_get_position(entt_id);
    const ev_Vector3 *ev_RotationVector = entity_get_rotation(entt_id);
    if(ev_PositionVector)
    {
      centerOfMassWorldTrans.setOrigin(ev2btVector3(ev_PositionVector));
    }
    if(ev_RotationVector)
    {
      btQuaternion rot;
      rot.setEulerZYX(
          ANG2RAD(ev_RotationVector->z),
          ANG2RAD(ev_RotationVector->y),
          ANG2RAD(ev_RotationVector->x)
          );
      centerOfMassWorldTrans.setRotation(rot);
    }
  }


  void setWorldTransform(const btTransform & centerOfMassWorldTrans)
  {
    ev_Vector3 *ev_PositionVector = entity_get_position_mut(entt_id);
    ev_Vector3 *ev_RotationVector = entity_get_rotation_mut(entt_id);
    if(ev_PositionVector)
    {
      const btVector3 pos = centerOfMassWorldTrans.getOrigin();
      ev_PositionVector->x = pos.getX();
      ev_PositionVector->y = pos.getY();
      ev_PositionVector->z = pos.getZ();
    }
    if(ev_RotationVector)
    {
      btScalar x, y, z;
      centerOfMassWorldTrans.getRotation().getEulerZYX(
          z,
          y,
          x
      );
      ev_RotationVector->x = RAD2ANG(x);
      ev_RotationVector->y = RAD2ANG(y);
      ev_RotationVector->z = RAD2ANG(z);
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
  bool isDynamic = rb->mass;

  btVector3 localInertia(0, 0, 0);

  if(isDynamic)
  {
    ((btCollisionShape*)rb->collisionShape)->calculateLocalInertia(rb->mass, localInertia);
  }

  EvMotionState *motionState = new EvMotionState();

  motionState->entt_id = (unsigned int)rb->entt_id;

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
  printf("updating\n");
  btVector3 inertiaVec = btVector3(0, 0, 0);
  ((btRigidBody*)handle)->setMassProps(rb->mass, inertiaVec);


  if(rb->collisionShape != ((btRigidBody*)handle)->getCollisionShape())
  {
    ((btRigidBody*)handle)->setCollisionShape((btCollisionShape*)rb->collisionShape);
  }

  unsigned int entt_id = ((EvMotionState *)((btRigidBody*)handle)->getMotionState())->entt_id;

  const ev_Vector3 *ev_PositionVector = entity_get_position(entt_id);
  const ev_Vector3 *ev_RotationVector = entity_get_rotation(entt_id);

  btTransform newTransform;
  newTransform.setIdentity();
  newTransform.setOrigin(ev2btVector3(ev_PositionVector));
  btQuaternion rot;

  rot.setEulerZYX(
      btScalar(ANG2RAD(ev_RotationVector->z)), 
      btScalar(ANG2RAD(ev_RotationVector->y)), 
      btScalar(ANG2RAD(ev_RotationVector->x))
  );

  newTransform.setRotation(rot);

  ((btRigidBody*)handle)->setWorldTransform(newTransform);
}

void BulletState::setGravity(real x, real y, real z)
{
  world->setGravity(btVector3(btScalar(x), btScalar(y), btScalar(z)));
}

void BulletState::step()
{
  world->stepSimulation(0.0167f, 10);
}

void BulletState::step_dt(real dt)
{
  world->stepSimulation(dt, 10);
}

