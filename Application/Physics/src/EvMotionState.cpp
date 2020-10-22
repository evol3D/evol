#include "EvMotionState.h"

void EvMotionState::getWorldTransform(btTransform & centerOfMassWorldTrans) const
{
  const ev_Matrix4 *ev_TransformMatrix = entity_getWorldTransform(entt_id);
  centerOfMassWorldTrans.setFromOpenGLMatrix(reinterpret_cast<const btScalar*>(*ev_TransformMatrix));
}


void EvMotionState::setWorldTransform(const btTransform & centerOfMassWorldTrans)
{
  ev_Matrix4 *ev_TransformMatrix = entity_getWorldTransformMut(entt_id);
  centerOfMassWorldTrans.getOpenGLMatrix(reinterpret_cast<btScalar*>(*ev_TransformMatrix));
}
