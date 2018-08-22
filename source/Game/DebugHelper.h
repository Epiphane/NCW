//
// DebugHelper - Provides the ability to render debug text, info, geometry, etc.
//
// By Thomas Steinke
//

#pragma once

#include <functional>
#include <utility>
#include <vector>

#include <Engine/Core/Singleton.h>
#include <Engine/Core/Window.h>
#include <Engine/Graphics/FontManager.h>
#include <Engine/Graphics/VBO.h>

// Included so that we get the CUBEWORLD_BENCHMARK_SYSTEMS define
#include <Engine/System/SystemManager.h>

namespace CubeWorld
{

namespace Game
{

   class DebugHelper : public Engine::Singleton<DebugHelper> {
   public:
      DebugHelper();
      ~DebugHelper();

      //
      // Syntactic sugar for the below. This allows using a lambda, instead of predefining
      // a std::function when calling Register([&]() { ... });
      //
      //template <typename T> struct identity { typedef T type; };

      // Register a debug line.
      // Ex: DebugHelper::Instance()->RegisterMetric("FPS", []() { return "Undefined"; })
      void RegisterMetric(const std::string& name, const std::function<std::string(void)>& fn);

      void Update();
      void Render();

      void SetWindow(Engine::Window* window) { mWindow = window; }

#if CUBEWORLD_BENCHMARK_SYSTEMS
      void SetSystemManager(Engine::SystemManager* manager) { mSystemManager = manager; }
#endif

   private:
      Engine::Window* mWindow;

      Engine::Graphics::Font* mFont;

      std::vector<std::pair<std::string, std::function<std::string(void)>>> mMetrics;
      std::vector<std::pair<std::string, std::string>> mMetricsState;
      std::string mMetricsText;

      Engine::Graphics::VBO mMetricsTextVBO;
      GLint mMetricsCount;

#if CUBEWORLD_BENCHMARK_SYSTEMS
      Engine::Graphics::VBO mSystemsBenchmarkVBO;
      GLint mSystemsCount;

      Engine::SystemManager* mSystemManager;
#endif

   private:
      static GLuint program;
      static GLuint aPosition, aUV;
      static GLuint uTexture, uWindowSize;
   };

}; // namespace Game

}; // namespace CubeWorld
