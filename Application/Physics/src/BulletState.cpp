#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "physics_types.h"
#include "stdio.h"
#include "BulletState.h"
/* #include "PhysicsDebugWindow.h" */
#include "EvMotionState.h"
#include "utils.h"

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
  /* PhysicsDebugWindow::deinit(); */

  clearCollisionObjects();
  clearCollisionShapes();

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

void BulletState::clearCollisionShapes()
{
  for(int i = 0; i < collisionShapes.size(); ++i)
  {
    btCollisionShape *shape = collisionShapes[i];
    collisionShapes[i] = 0;
    delete shape;
  }
}

void BulletState::visualize()
{
  if(visualDebugging)
    return;

  /* PhysicsDebugWindow::init(world); */
  visualDebugging = true;
}

CollisionShape BulletState::createBox(real x, real y, real z)
{
  btCollisionShape* box = new btBoxShape(btVector3(btScalar(x), btScalar(y), btScalar(z)));
  collision_shapes_mutex.lock();
  collisionShapes.push_back(box);
  collision_shapes_mutex.unlock();
  return box;
}

CollisionShape BulletState::createSphere(real r)
{
  btCollisionShape* sphere = new btSphereShape(btScalar(r));
  collision_shapes_mutex.lock();
  collisionShapes.push_back(sphere);
  collision_shapes_mutex.unlock();
  return sphere;
}

CollisionShape BulletState::createStaticFromTriangleIndexVertex(int numTriangles, int *triangleIndexBase, int triangleIndexStride, int numVertices, real *vertexBase, int vertexStride)
{
  btStridingMeshInterface* buffer_interface = new btTriangleIndexVertexArray(numTriangles, triangleIndexBase, triangleIndexStride, numVertices, vertexBase, vertexStride);
  btCollisionShape* mesh = new btBvhTriangleMeshShape(buffer_interface, true);
  collision_shapes_mutex.lock();
  collisionShapes.push_back(mesh);
  collision_shapes_mutex.unlock();

  return mesh;
}

CollisionShape BulletState::generateConvexHull(int vertexCount, ev_Vector3* vertices)
{
  btCollisionShape* hull = new btConvexHullShape(reinterpret_cast<real*>(vertices), vertexCount, sizeof(ev_Vector3));
  collision_shapes_mutex.lock();
  collisionShapes.push_back(hull);
  collision_shapes_mutex.unlock();

  return hull;
}


RigidBodyHandle BulletState::addRigidBody(RigidBody *rb)
{
  bool isDynamic = rb->mass;

  btVector3 localInertia(0, 0, 0);

  if(rb->type == EV_RIGIDBODY_DYNAMIC)
  {
    reinterpret_cast<btCollisionShape*>(rb->collisionShape)->calculateLocalInertia(rb->mass, localInertia);
  }

  EvMotionState *motionState = new EvMotionState();

  motionState->entt_id = (unsigned int)rb->entt_id;

  btRigidBody::btRigidBodyConstructionInfo rbInfo(rb->mass, motionState, (btCollisionShape*)rb->collisionShape, localInertia);
  rbInfo.m_restitution = rb->restitution;

  btRigidBody* body = new btRigidBody(rbInfo);
  if(rb->type == EV_RIGIDBODY_KINEMATIC)
  {
    body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
    body->setActivationState(DISABLE_DEACTIVATION);
  }

  world_mutex.lock();
  world->addRigidBody(body);
  world_mutex.unlock();

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
  btRigidBody *rbHandle = reinterpret_cast<btRigidBody*>(handle);
  btCollisionShape *collisionShape = reinterpret_cast<btCollisionShape*>(rb->collisionShape);

  btVector3 inertiaVec = btVector3(0, 0, 0);
  rbHandle->setMassProps(rb->mass, inertiaVec);


  if(collisionShape != rbHandle->getCollisionShape())
  {
    rbHandle->setCollisionShape(collisionShape);
  }

  unsigned int entt_id = ((EvMotionState *)((btRigidBody*)handle)->getMotionState())->entt_id;

  const ev_Matrix4 *ev_TransformMatrix = entity_getWorldTransform(entt_id);

  btTransform newTransform;
  newTransform.setFromOpenGLMatrix(reinterpret_cast<const btScalar*>(*ev_TransformMatrix));

  rbHandle->setWorldTransform(newTransform);//->setWorldTransform(newTransform);
}

void BulletState::setGravity(real x, real y, real z)
{
  world->setGravity(btVector3(btScalar(x), btScalar(y), btScalar(z)));
}

void BulletState::step()
{
  world_mutex.lock();
  world->stepSimulation(0.0167f, 10);
  world_mutex.unlock();
}

void BulletState::step_dt(real dt)
{
  world_mutex.lock();
  world->stepSimulation(dt, 10);
  world_mutex.unlock();
}
