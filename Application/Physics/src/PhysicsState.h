#pragma once
class PhysicsState {
  public:
    PhysicsState() {};
    virtual ~PhysicsState() {};
    virtual void visualize() = 0;
};
