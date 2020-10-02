#include "stdio.h"
#include "BulletState.h"

ATTRIBUTE_ALIGNED16(struct)
EvMotionState : public btMotionState
{
  unsigned int entt_id;

  BT_DECLARE_ALIGNED_ALLOCATOR();

  EvMotionState(btVector3* graphicsVec = 0, const btTransform& startTransform = btTransform::getIdentity(), const btTransform& centerOfMassOffset = btTransform::getIdentity())
  {
  }

  void getWorldTransform(btTransform & centerOfMassWorldTrans) const;
  void setWorldTransform(const btTransform & centerOfMassWorldTrans);

};
