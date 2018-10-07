// By Thomas Steinke

#include <glad/glad.h>

#include <Engine/Core/Scope.h>
#include <Engine/Core/Window.h>
#include <Engine/Logger/Logger.h>

#include "Helpers/Asset.h"
#include "DebugHelper.h"

namespace CubeWorld
{

namespace Game
{

std::unique_ptr<Engine::Graphics::Program> DebugHelper::program = nullptr;

DebugHelper::DebugHelper()
   : mBounds(nullptr)
   , mFont(nullptr)
   , mMetricsTextVBO(Engine::Graphics::VBO::Vertices)
   , mSystemsBenchmarkVBO(Engine::Graphics::VBO::Vertices)
   , mSystemManager(nullptr)
{
   mMetrics = std::make_unique<MetricLink>(this, "None", nullptr);
   mMetrics->next = mMetrics->prev = mMetrics.get();

   auto maybeFont = Engine::Graphics::FontManager::Instance()->GetFont(Asset::Font("debug"));
   assert(maybeFont);
   mFont = *maybeFont;

   if (!program)
   {
      auto maybeProgram = Engine::Graphics::Program::Load("Shaders/DebugText.vert", "Shaders/DebugText.geom", "Shaders/DebugText.frag");
      if (!maybeProgram)
      {
         LOG_ERROR(maybeProgram.Failure().WithContext("Failed loading DebugText shader").GetMessage());
         return;
      }

      program = std::move(*maybeProgram);
      program->Attrib("aPosition");
      program->Attrib("aUV");
      program->Uniform("uTexture");
      program->Uniform("uWindowSize");
   }
}

DebugHelper::~DebugHelper()
{
}

std::unique_ptr<DebugHelper::MetricLink> DebugHelper::RegisterMetric(const std::string& name, const std::function<std::string(void)>& fn)
{
   std::unique_ptr<MetricLink> link = std::make_unique<MetricLink>(this, name, fn);
      
   link->next = mMetrics.get();
   link->prev = mMetrics->prev;
   link->prev->next = link.get();
   link->next->prev = link.get();

   return std::move(link);
}

void DebugHelper::DeregisterMetric(std::unique_ptr<MetricLink> metric)
{
   // metric will be deconstructed at the end of this function, which is now the owner
}

void DebugHelper::RemoveLink(MetricLink* link)
{
   if (link->next != nullptr)
   {
      link->next->prev = link->prev;
   }

   if (link->prev != nullptr)
   {
      link->prev->next = link->next;
   }
      
   link->next = nullptr;
   link->prev = nullptr;
}

void DebugHelper::Update()
{
   mMetricsState.clear();
   std::string text = "DEBUG:";
   MetricLink* metric = mMetrics->next;
   while (metric != mMetrics.get())
   {
      if (metric->callback)
      {
         std::pair<std::string, std::string> result = std::make_pair(metric->name, metric->callback());
         mMetricsState.push_back(result);

         text += "\n" + result.first + ": " + result.second;
      }
      metric = metric->next;
   }

   float left = GLfloat(mBounds->GetX());
   float top = GLfloat(mBounds->GetY() + mBounds->GetHeight()) - 40.0f;
   float right = left + mBounds->GetWidth();
   if (mMetricsText != text)
   {
      mMetricsText = text;
      std::vector<Engine::Graphics::Font::CharacterVertexUV> metricsText = mFont->Write(left, top, 1, text);

      mMetricsCount = static_cast<GLint>(metricsText.size());
      mMetricsTextVBO.BufferData(sizeof(Engine::Graphics::Font::CharacterVertexUV) * mMetricsCount, &metricsText[0], GL_STATIC_DRAW);
   }

#if CUBEWORLD_BENCHMARK_SYSTEMS
   if (mSystemManager != nullptr)
   {
      std::string leftText = "-------- Systems ------";
      std::string rightText = "------";
      for (auto system : mSystemManager->GetBenchmarks())
      {
         leftText += "\n" + system.first;
         std::string ms = Format::FormatString("%.1fms", system.second * 1000.0);
         ms.insert(ms.begin(), 7 - ms.size(), ' ');
         rightText += "\n" + std::move(ms);
      }
      std::vector<Engine::Graphics::Font::CharacterVertexUV> systemsText = mFont->Write(right - 400, top, 1, leftText);
      std::vector<Engine::Graphics::Font::CharacterVertexUV> rightUVs = mFont->Write(right - 105, top, 1, rightText);
      systemsText.insert(systemsText.end(), rightUVs.begin(), rightUVs.end());

      mSystemsCount = static_cast<GLint>(systemsText.size());
      mSystemsBenchmarkVBO.BufferData(sizeof(Engine::Graphics::Font::CharacterVertexUV) * mSystemsCount, &systemsText[0], GL_STATIC_DRAW);
   }
#endif
}

void DebugHelper::Render()
{
   if (mBounds == nullptr)
   {
      return;
   }

   // TODO tech debt? It's getting covered by the stuff the same draws,
   // but I mean we always want debug text on top soooo can't hurt.
   glClear(GL_DEPTH_BUFFER_BIT);

   BIND_PROGRAM_IN_SCOPE(program);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mFont->GetTexture());
   program->Uniform1i("uTexture", 0);
   program->Uniform2f("uWindowSize", static_cast<GLfloat>(Engine::Window::Instance()->GetWidth()), static_cast<GLfloat>(Engine::Window::Instance()->GetHeight()));

   mMetricsTextVBO.AttribPointer(program->Attrib("aPosition"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)0);
   mMetricsTextVBO.AttribPointer(program->Attrib("aUV"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)(sizeof(float) * 2));

   glDrawArrays(GL_LINES, 0, mMetricsCount);

   mSystemsBenchmarkVBO.AttribPointer(program->Attrib("aPosition"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)0);
   mSystemsBenchmarkVBO.AttribPointer(program->Attrib("aUV"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)(sizeof(float) * 2));

   glDrawArrays(GL_LINES, 0, mSystemsCount);
}

}; // namespace Game

}; // namespace CubeWorld
