// By Thomas Steinke

#pragma once

#include <glm/ext.hpp>

#include "Component.h"
#include "ComponentHandle.h"
#include "Entity.h"

namespace CubeWorld
{

namespace Engine
{

class Transform : public Component<Transform> {
public:
   using val = float;

   Transform(
      glm::vec3 position,
      glm::vec3 direction = glm::vec3(0, 0, 1),
      val roll = 0,
      glm::vec3 scale = glm::vec3(1, 1, 1)
   );

public:
   // Computations that take into account parent transformation.
   glm::vec3 GetAbsolutePosition() const;
   glm::vec3 GetAbsoluteDirection() const;
   glm::vec3 GetAbsoluteScale() const;

   // Traits relative to one's parent
   glm::vec3 GetLocalPosition() const { return mPosition; }
   glm::vec3 GetLocalScale() const { return mScale; }
   glm::vec3 GetLocalDirection() const { return mDirection; }
   glm::vec3 GetFlatDirection() const { return mFlatDirection; }
   val GetPitch() const { return mPitch; }
   val GetYaw() const { return mYaw; }
   val GetRoll() const { return mRoll; }
   ComponentHandle<Transform> GetParent() const { return mParent; }
   glm::mat4 GetMatrix() const;

   void SetLocalPosition(glm::vec3 position);
   void SetLocalScale(glm::vec3 scale);
   void SetLocalDirection(glm::vec3 direction);
   void SetPitch(val pitch);
   void SetYaw(val yaw);
   void SetRoll(val r) { mRoll = r; }
   void SetParent(const ComponentHandle<Transform>& parent) { mParent = parent; }
   void SetParent(const Entity& parent) { mParent = parent.Get<Transform>(); }

   // Break all the normal conventions of a Transform and set the matrix directly.
   // Any extra changes will cause previous state to be lost.
   void SetMatrix(const glm::mat4 matrix) { mMatrix = matrix; }

private:
   void ComputeMatrix();
   glm::mat4 mMatrix;

   glm::vec3 mPosition;
   glm::vec3 mUp;
   glm::vec3 mScale;

   glm::vec3 mDirection, mFlatDirection;
   val mPitch, mYaw, mRoll;

   ComponentHandle<Transform> mParent;
};

}; // namespace Engine

}; // namespace CubeWorld
