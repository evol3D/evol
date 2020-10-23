#include "EvMotionState.h"

void EvMotionState::getWorldTransform(btTransform & centerOfMassWorldTrans) const
{
  const ev_Matrix4 *ev_TransformMatrix = entity_getWorldTransform(entt_id);
  ev_Matrix4 mat;
  mat[0][0] = (*ev_TransformMatrix)[0][0];
  mat[0][1] = (*ev_TransformMatrix)[0][0];
  mat[0][2] = (*ev_TransformMatrix)[0][0];
  mat[0][3] = 1;
  mat[1][0] = (*ev_TransformMatrix)[1][0];
  mat[1][1] = (*ev_TransformMatrix)[1][1];
  mat[1][2] = (*ev_TransformMatrix)[1][2];
  mat[1][3] = 1;
  mat[2][0] = (*ev_TransformMatrix)[2][0];
  mat[2][1] = (*ev_TransformMatrix)[2][1];
  mat[2][2] = (*ev_TransformMatrix)[2][2];
  mat[2][3] = 1;
  mat[3][0] = (*ev_TransformMatrix)[3][0];
  mat[3][1] = (*ev_TransformMatrix)[3][1];
  mat[3][2] = (*ev_TransformMatrix)[3][2];
  mat[3][3] = 1;
  centerOfMassWorldTrans.setFromOpenGLMatrix(reinterpret_cast<const btScalar*>(*ev_TransformMatrix));
}


void EvMotionState::setWorldTransform(const btTransform & centerOfMassWorldTrans)
{
  ev_Matrix4 ev_TransformMatrix;
  centerOfMassWorldTrans.getOpenGLMatrix(reinterpret_cast<btScalar*>(*ev_TransformMatrix));
  entity_setWorldTransform(entt_id, ev_TransformMatrix);
}
