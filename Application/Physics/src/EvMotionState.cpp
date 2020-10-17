#include "EvMotionState.h"

void EvMotionState::getWorldTransform(btTransform & centerOfMassWorldTrans) const
{
  const ev_Vector4 *ev_PositionVector = entity_get_position(entt_id);
  const ev_Vector4 *ev_RotationVector = entity_get_rotation(entt_id);

  if(ev_PositionVector)
  {
    centerOfMassWorldTrans.setOrigin(ev2btVector3(ev_PositionVector));
  }
  if(ev_RotationVector)
  {
    centerOfMassWorldTrans.setRotation(
      btQuaternion(
        ev_RotationVector->x,
        ev_RotationVector->y,
        ev_RotationVector->z,
        ev_RotationVector->w
      )
    );
  }
}


void EvMotionState::setWorldTransform(const btTransform & centerOfMassWorldTrans)
{
  ev_Vector4 *ev_PositionVector = entity_get_position_mut(entt_id);
  ev_Vector4 *ev_RotationVector = entity_get_rotation_mut(entt_id);
  if(ev_PositionVector)
  {
    const btVector3 pos = centerOfMassWorldTrans.getOrigin();
    ev_PositionVector->x = pos.getX();
    ev_PositionVector->y = pos.getY();
    ev_PositionVector->z = pos.getZ();
  }
  if(ev_RotationVector)
  {
    btQuaternion rot_quat = centerOfMassWorldTrans.getRotation();
    *ev_RotationVector = {
      rot_quat.x(),
      rot_quat.y(),
      rot_quat.z(),
      rot_quat.w()
    };
  }
}
