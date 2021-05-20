// By Thomas Steinke

#pragma once

#include <GL/includes.h>
#include <glm/glm.hpp>

#include <Engine/Graphics/Camera.h>
#include <Engine/Entity/Component.h>
#include <Engine/Entity/ComponentHandle.h>
#include <Engine/Entity/Transform.h>

namespace CubeWorld
{

//
// RenderCamera implements the Camera class in a unique way.
//
// Perspective is pretty straightforward, just a set of options, but it depends on
// a direct link to an entity's Transform component as well.
//
class ArmCamera : public Engine::Component<ArmCamera>, public Engine::Graphics::Camera {
public:
   //
   // Options for initializing a RenderCamera. Most of them define the perspective matrix.
   //
   struct Options
   {
      double aspect = 1.0;
      double fov = 45.0;
      double near = 0.1;
      double far = 100.0;
      float distance = 5;
      float minDistance = 2;
      float maxDistance = 100;
   };

   ArmCamera(const Engine::ComponentHandle<Engine::Transform>& transform, const Options& options);
   ~ArmCamera() {};

   glm::mat4 GetPerspective() const override;
   glm::mat4 GetView() const override;
   glm::vec3 GetPosition() const override;

   Engine::ComponentHandle<Engine::Transform> transform;

   double aspect;
   double fov;
   double near;
   double far;
   float distance;
   float minDistance;
   float maxDistance;
};

struct MouseControlledCameraArm : public Engine::Component<MouseControlledCameraArm> {
   MouseControlledCameraArm(double sensitivity = 1.0)
      : sensitivity(sensitivity)
   {};

   double sensitivity;
};

struct KeyControlledCameraArm : public Engine::Component<KeyControlledCameraArm> {
   KeyControlledCameraArm(
      int zoomInKey = GLFW_KEY_E,
      int zoomOutKey = GLFW_KEY_Q,
      double sensitivity = 8.0
   )
      : zoomIn(zoomInKey)
      , zoomOut(zoomOutKey)
      , sensitivity(sensitivity)
   {};

   int zoomIn, zoomOut;
   double sensitivity;
};

}; // namespace CubeWorld
