// By Thomas Steinke

#pragma once

#include <GL/includes.h>
#include <glm/glm.hpp>

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

   class Camera
   {
   public:
      virtual ~Camera() {}
      
      virtual glm::mat4 GetPerspective() const = 0;
      virtual glm::mat4 GetView() const = 0;
      virtual glm::vec3 GetPosition() const = 0;
   };

   //
   // Convenience handle around a Camera, for changing cameras without having to update
   // potentially numerous references to it.
   //
   class CameraHandle : public Camera
   {
   public:
      CameraHandle(Camera* camera = nullptr) : mCamera(camera) {}

      glm::mat4 GetPerspective() const override { return mCamera->GetPerspective(); }
      glm::mat4 GetView() const override { return mCamera->GetView(); }
      glm::vec3 GetPosition() const override { return mCamera->GetPosition(); }
   
      void Set(Camera *camera) { mCamera = camera; }
      Camera* Get() { return mCamera; }

   private:
      Camera *mCamera;
   };
   
}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
