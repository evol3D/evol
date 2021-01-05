#include "BulletCollision/CollisionDispatch/btCollisionObject.h"
#include "BulletCollision/CollisionShapes/btCapsuleShape.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "BulletDynamics/Vehicle/btRaycastVehicle.h"
#include "BulletDynamics/Vehicle/btVehicleRaycaster.h"
#include "physics_types.h"
#include "stdio.h"
#include "BulletState.h"
#include "PhysicsDebugWindow.h"
#include "EvMotionState.h"
#include "utils.h"

BulletState::BulletState()
{
  collisionConfiguration = new btDefaultCollisionConfiguration();
  collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
  broadphase = new btDbvtBroadphase();
  constraintSolver = new btSequentialImpulseConstraintSolver();
  world = new btDiscreteDynamicsWorld(collisionDispatcher, broadphase, constraintSolver, collisionConfiguration);
	world->getSolverInfo().m_numIterations = 100;
}

BulletState::~BulletState()
{
  PhysicsDebugWindow::deinit();

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

  PhysicsDebugWindow::init(world);
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

CollisionShape BulletState::createCapsule(real radius, real height)
{
  btCollisionShape *capsule = new btCapsuleShape(radius, height);
  collision_shapes_mutex.lock();
  collisionShapes.push_back(capsule);
  collision_shapes_mutex.unlock();
  return capsule;
}

CollisionShape BulletState::createCylinderX(real width, real radiusX, real radiusY)
{
  btCollisionShape *cylinderX = new btCylinderShapeX(btVector3(width, radiusX, radiusY)); 
  collision_shapes_mutex.lock();
  collisionShapes.push_back(cylinderX);
  collision_shapes_mutex.unlock();
  return cylinderX;
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
  btBvhTriangleMeshShape* mesh = new btBvhTriangleMeshShape(buffer_interface, true);
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

  btVector3 localInertia(0,0,0);
  btCompoundShape *compound = new btCompoundShape(); 

  if(isDynamic) {
    reinterpret_cast<btCollisionShape*>(rb->collisionShape)->calculateLocalInertia(rb->mass, localInertia);

    btTransform localTransform;
    localTransform.setIdentity();
    localTransform.setOrigin(btVector3(0, -3, 0));
    compound->addChildShape(localTransform, (btCollisionShape *)rb->collisionShape);
  }

  EvMotionState *motionState = new EvMotionState();

  motionState->entt_id = (unsigned int)rb->entt_id;

  btRigidBody::btRigidBodyConstructionInfo rbInfo(rb->mass, motionState, (isDynamic)?compound:(btCollisionShape*)rb->collisionShape, localInertia);
  /* btRigidBody::btRigidBodyConstructionInfo rbInfo(rb->mass, motionState, (btCollisionShape*)rb->collisionShape, localInertia); */
  rbInfo.m_restitution = rb->restitution;

  btRigidBody* body = new btRigidBody(rbInfo);
  if(rb->type == EV_RIGIDBODY_KINEMATIC) {
    body->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
  }

  if(rb->type == EV_RIGIDBODY_STATIC) {
    body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
  }

  if(rb->type == EV_RIGIDBODY_DYNAMIC) {
    body->setCollisionFlags(btCollisionObject::CF_DYNAMIC_OBJECT);
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

  btVector3 inertiaVec;;
  collisionShape->calculateLocalInertia(rb->mass, inertiaVec);
  rbHandle->setMassProps(rb->mass, inertiaVec);


  if(collisionShape != rbHandle->getCollisionShape())
  {
    rbHandle->setCollisionShape(collisionShape);
  }

  unsigned int entt_id = ((EvMotionState *)((btRigidBody*)handle)->getMotionState())->entt_id;

  const ev_Matrix4 *ev_TransformMatrix = entity_getWorldTransform(entt_id);

  btTransform newTransform;
  newTransform.setFromOpenGLMatrix(reinterpret_cast<const btScalar*>(*ev_TransformMatrix));

  rbHandle->setWorldTransform(newTransform);
}

void BulletState::setGravity(real x, real y, real z)
{
  world->setGravity(btVector3(btScalar(x), btScalar(y), btScalar(z)));
}

void BulletState::step()
{
  world_mutex.lock();
  world->stepSimulation(0.01667f, 10);
  world_mutex.unlock();
}

void BulletState::step_dt(real dt)
{
  world_mutex.lock();
  world->stepSimulation(dt, 2);
  world_mutex.unlock();
}

void BulletState::setLinearVelocity(RigidBodyHandle handle, ev_Vector3 *velocity)
{
  btRigidBody *rbHandle = reinterpret_cast<btRigidBody*>(handle);
  rbHandle->setLinearVelocity(btVector3(velocity->x, velocity->y, velocity->z));
}

void BulletState::getLinearVelocity(RigidBodyHandle handle, ev_Vector3 *velocity)
{
  btRigidBody *rbHandle = reinterpret_cast<btRigidBody*>(handle);
  btVector3 linearVelocity = rbHandle->getLinearVelocity();
  velocity->x = linearVelocity.x();
  velocity->y = linearVelocity.y();
  velocity->z = linearVelocity.z();
}

void BulletState::setAngularVelocity(RigidBodyHandle handle, ev_Vector3 *velocity)
{
  btRigidBody *rbHandle = reinterpret_cast<btRigidBody*>(handle);
  rbHandle->setAngularVelocity(btVector3(velocity->x, velocity->y, velocity->z));
}

void BulletState::getAngularVelocity(RigidBodyHandle handle, ev_Vector3 *velocity)
{
  btRigidBody *rbHandle = reinterpret_cast<btRigidBody*>(handle);
  btVector3 angularVelocity = rbHandle->getAngularVelocity();
  velocity->x = angularVelocity.x();
  velocity->y = angularVelocity.y();
  velocity->z = angularVelocity.z();
}

void BulletState::applyForce(RigidBodyHandle handle, ev_Vector3 *force)
{
  btRigidBody *rbHandle = reinterpret_cast<btRigidBody*>(handle);
  rbHandle->applyCentralForce(btVector3(force->x, force->y, force->z));
}

void BulletState::setFriction(RigidBodyHandle handle, real frict)
{
  btRigidBody *rbHandle = reinterpret_cast<btRigidBody*>(handle);
  rbHandle->setFriction(frict);
}

void BulletState::setDamping(RigidBodyHandle handle, real linearDamping, real angularDamping)
{
  btRigidBody *rbHandle = reinterpret_cast<btRigidBody*>(handle);
  rbHandle->setDamping(linearDamping, angularDamping);
}

void BulletState::addHingeConstraint(RigidBodyHandle parentHandle, RigidBodyHandle childHandle, ev_Vector3 *anchor, ev_Vector3 *parentAxis, ev_Vector3 *childAxis)
{
  btRigidBody *parentBody = reinterpret_cast<btRigidBody *>(parentHandle);
  btRigidBody *childBody  = reinterpret_cast<btRigidBody *>(childHandle);
  btVector3 anchorVec3(anchor->x, anchor->y, anchor->z);
  btVector3 parentAxisVec3(parentAxis->x, parentAxis->y, parentAxis->z); 
  btVector3 childAxisVec3(childAxis->x, childAxis->y, childAxis->z);
  btHinge2Constraint* pHinge2 = new btHinge2Constraint(
      *parentBody, *childBody, 
      anchorVec3,
      parentAxisVec3, childAxisVec3
  );

  world->addConstraint(pHinge2, true);

  pHinge2->setDamping( 2, 4.0 );
  pHinge2->setStiffness( 2, 40.0);
}

void BulletState::setRotation(RigidBodyHandle handle, ev_Vector4 *rotationQuat)
{
  btRigidBody *body  = reinterpret_cast<btRigidBody *>(handle);
  btTransform bodyTransform = body->getWorldTransform();
  bodyTransform.setRotation(
      btQuaternion(
        rotationQuat->x,
        rotationQuat->y,
        rotationQuat->z,
        rotationQuat->w
        )
      );
  body->setWorldTransform(bodyTransform);
}

PhysicsVehicle BulletState::createRaycastVehicle(RigidBodyHandle handle)
{
  btRigidBody *chassisRb  = reinterpret_cast<btRigidBody *>(handle);

  btVehicleRaycaster *vehicleRaycaster = new btDefaultVehicleRaycaster(world);
  btRaycastVehicle::btVehicleTuning tuning;

  btRaycastVehicle *vehicle = new btRaycastVehicle(tuning, chassisRb, vehicleRaycaster);
  world->addVehicle(vehicle);

  btVector3 halfExtents(1.5, btScalar(0.5), 2);

  btVector3 raycastDirection(0, -1, 0);
  btVector3 wheelAxle(1, 0, 0);

  btScalar suspensionRestLength(0.5);

  btScalar wheelWidth(0.2);

  btScalar wheelRadius(0.2);

  //The height where the wheels are connected to the chassis
  btScalar connectionHeight(0.1);

  //All the wheel configuration assumes the vehicle is centered at the origin and a right handed coordinate system is used
  btVector3 wheelConnectionPoint(halfExtents.x() - wheelWidth, connectionHeight, halfExtents.z() - wheelRadius);

  //Adds the front wheels
  vehicle->addWheel(wheelConnectionPoint, raycastDirection, wheelAxle, suspensionRestLength, wheelRadius, tuning, true);
  vehicle->addWheel(wheelConnectionPoint * btVector3(-1, 1, 1), raycastDirection, wheelAxle, suspensionRestLength, wheelRadius, tuning, true);

  //Adds the rear wheels
  vehicle->addWheel(wheelConnectionPoint* btVector3(1, 1, -1), raycastDirection, wheelAxle, suspensionRestLength, wheelRadius, tuning, false);
  vehicle->addWheel(wheelConnectionPoint * btVector3(-1, 1, -1), raycastDirection, wheelAxle, suspensionRestLength, wheelRadius, tuning, false);

  //Configures each wheel of our vehicle, setting its friction, damping compression, etc.
  //For more details on what each parameter does, refer to the docs
  for (int i = 0; i < vehicle->getNumWheels(); i++)
  {
    btWheelInfo& wheel = vehicle->getWheelInfo(i);
    wheel.m_suspensionStiffness = 50;
    wheel.m_wheelsDampingCompression = btScalar(0.3) * 2 * btSqrt(wheel.m_suspensionStiffness);//btScalar(0.8);
    wheel.m_wheelsDampingRelaxation = btScalar(0.7) * 2 * btSqrt(wheel.m_suspensionStiffness);//1;
    //Larger friction slips will result in better handling
    wheel.m_frictionSlip = btScalar(1.2);
    wheel.m_rollInfluence = 1;
  }

  return vehicle;
}

void BulletState::applyEngineForce(PhysicsVehicle physVehicle, real force)
{
  btRaycastVehicle *vehicle = reinterpret_cast<btRaycastVehicle*>(physVehicle);
  /* if(force > 0) { */
  vehicle->applyEngineForce(force, 2);
  vehicle->applyEngineForce(force, 3);
  /* } else { */
  /*   vehicle->applyEngineForce(-force, 2); */
  /*   vehicle->applyEngineForce(-force, 3); */
  /* } */
}

void BulletState::setVehicleSteering(PhysicsVehicle physVehicle, real steeringVal)
{
  btRaycastVehicle *vehicle = reinterpret_cast<btRaycastVehicle*>(physVehicle);
  vehicle->setSteeringValue(-steeringVal, 2);
  vehicle->setSteeringValue(-steeringVal, 3);
}
