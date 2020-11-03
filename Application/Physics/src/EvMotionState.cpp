#include "EvMotionState.h"
#include "cglm/affine.h"
#include "cglm/cglm.h"

void EvMotionState::getWorldTransform(btTransform & centerOfMassWorldTrans) const
{
  const ev_Matrix4 *ev_TransformMatrix = entity_getWorldTransform(entt_id);
  centerOfMassWorldTrans.setFromOpenGLMatrix(reinterpret_cast<const btScalar*>(*ev_TransformMatrix));
}


void EvMotionState::setWorldTransform(const btTransform & centerOfMassWorldTrans)
{
  ev_Matrix4 ev_TransformMatrix;
  centerOfMassWorldTrans.getOpenGLMatrix(reinterpret_cast<btScalar*>(*ev_TransformMatrix));
  entity_setWorldTransform(entt_id, ev_TransformMatrix);
}
