#include "EvMotionState.h"

void EvMotionState::getWorldTransform(btTransform & centerOfMassWorldTrans) const
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


void EvMotionState::setWorldTransform(const btTransform & centerOfMassWorldTrans)
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
