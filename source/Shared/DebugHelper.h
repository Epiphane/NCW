//
// DebugHelper - Provides the ability to render debug text, info, geometry, etc.
//
// By Thomas Steinke
//

#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include <RGBDesignPatterns/Singleton.h>
#include <Engine/Core/Window.h>
#include <Engine/Graphics/FontManager.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>

// Included so that we get the CUBEWORLD_BENCHMARK_SYSTEMS define
#include <Engine/System/SystemManager.h>

namespace CubeWorld
{

class DebugHelper : public Singleton<DebugHelper> {
public:
   // Metrics are stored as a doubly-linked list, to allow for easy insertion and removal.
   struct MetricLink {
      using Callback = std::function<std::string(void)>;

      std::string name;
      Callback callback;
      MetricLink(DebugHelper* debug, const std::string& name, const Callback& callback)
         : name(name)
         , callback(callback)
         , debug(debug)
         , next(nullptr)
         , prev(nullptr)
      {};
      ~MetricLink()
      {
         debug->RemoveLink(this);
      }

   private:
      DebugHelper* debug;

      friend class DebugHelper;
      MetricLink* next;
      MetricLink* prev;
   };

public:
   DebugHelper();
   ~DebugHelper();

   //
   // Syntactic sugar for the below. This allows using a lambda, instead of predefining
   // a std::function when calling Register([&]() { ... });
   //
   //template <typename T> struct identity { typedef T type; };

   // Register a debug line.
   // Ex: DebugHelper::Instance().RegisterMetric("FPS", []() { return "Unimplemented"; })
   std::unique_ptr<MetricLink> RegisterMetric(const std::string& name, const std::function<std::string(void)>& fn);
   void DeregisterMetric(std::unique_ptr<MetricLink> metric);

   void SetMetric(const std::string& name, const std::string& value);

   template <typename T>
   void SetMetric(const std::string& name, T value)
   {
      SetMetric(name, FormatString("%1", value));
   }

   void Update();
   void Render();

   void SetBounds(Bounded* bounds) { mBounds = bounds; }

#if CUBEWORLD_BENCHMARK_SYSTEMS
   void SetSystemManager(Engine::SystemManager* manager) { mSystemManager = manager; }
#endif

private:
   Bounded* mBounds;

   Engine::Graphics::Font* mFont;

private:
   std::unique_ptr<MetricLink> mMetrics;
   void RemoveLink(MetricLink* link);

   std::vector<std::unique_ptr<MetricLink>> mGlobalMetricLinks;
   std::unordered_map<std::string, std::string> mGlobalMetrics;

private:
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
   static std::unique_ptr<Engine::Graphics::Program> program;
};

}; // namespace CubeWorld
