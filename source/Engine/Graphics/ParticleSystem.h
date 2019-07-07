// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <GL/includes.h>
#include <glm/glm.hpp>

#include <RGBDesignPatterns/Maybe.h>
#include <RGBDesignPatterns/Scope.h>

#include "../Core/Config.h"
#include "Program.h"

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

//
// Describes a single particle system.
// 
// One particle system has the following traits:
// - A method of emitting particles (emitter)
// - Characteristic description of a single particle
// - Associated rendering shader
//
// All particle systems share the same shader for emitting
// and updating particles, but rendering shaders must be
// provided when constructing the ParticleSystem.
//
struct ParticleSystem {
public:
   ParticleSystem(
      const std::string& vertexShaderPath,
      const std::string& geometryShaderPath,
      const std::string& fragmentShaderPath
   );

   void Update(TIMEDELTA dt);
   void Render();

private:
   static std::unique_ptr<Program> updater;

   std::unique_ptr<Program> renderer;
};

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
