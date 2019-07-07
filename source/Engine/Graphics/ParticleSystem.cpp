// By Thomas Steinke

#include <RGBLogger/Logger.h>
#include <RGBDesignPatterns/Scope.h>

#include "ParticleSystem.h"

namespace CubeWorld
{

namespace Engine
{

namespace Graphics
{

// std::unique_ptr<Engine::Graphics::Program> ParticleSystem::updater = nullptr;

// ParticleSystem::ParticleSystem(
//    const std::string& vertexShaderPath,
//    const std::string& geometryShaderPath,
//    const std::string& fragmentShaderPath
// )
// {
//    if (!updater)
//    {
//       auto maybeUpdater = Engine::Graphics::Program::Load("Shaders/ParticleSystem.vert", "Shaders/ParticleSystem.geom", "Shaders/ParticleSystem.frag");
//       if (!maybeUpdater)
//       {
//          LOG_ERROR(maybeUpdater.Failure().WithContext("Failed loading 2D Texture shader").GetMessage());
//       }
//       else
//       {
//          updater = std::move(*maybeUpdater);

//          updater->Attrib("aType");
//          updater->Attrib("aPosition");
//          updater->Attrib("aVelocity");
//          updater->Attrib("aAge");
//          updater->Uniform("uWindowSize");
//          updater->Uniform("uDeltaTimeMillis");
//          updater->Uniform("uTime");
//          updater->Uniform("uRandomTexture");
//          updater->Uniform("uLauncherLifetime");
//          updater->Uniform("uShellLifetime");
//          updater->Uniform("uPlayerSpeed");
//       }
//    }
// }

}; // namespace Graphics

}; // namespace Engine

}; // namespace CubeWorld
