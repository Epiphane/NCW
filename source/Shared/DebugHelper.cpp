// By Thomas Steinke

#include <GL/glew.h>

#include <Engine/Logger/Logger.h>
#include <Engine/Graphics/Program.h>

#include "Helpers/Asset.h"
#include "DebugHelper.h"

namespace CubeWorld
{

namespace Game
{

   REGISTER_GLUINT(DebugHelper, program);
   REGISTER_GLUINT(DebugHelper, aPosition);
   REGISTER_GLUINT(DebugHelper, aUV);
   REGISTER_GLUINT(DebugHelper, uTexture);
   REGISTER_GLUINT(DebugHelper, uWindowSize);

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
      mFont = std::move(*maybeFont);

      program = Engine::Graphics::LoadProgram("Shaders/DebugText.vert", "Shaders/DebugText.geom", "Shaders/DebugText.frag");

      if (program == 0)
      {
         LOG_ERROR("Could not load DebugText shader");
         return;
      }

      DISCOVER_ATTRIBUTE(aPosition);
      DISCOVER_ATTRIBUTE(aUV);
      DISCOVER_UNIFORM(uTexture);
      DISCOVER_UNIFORM(uWindowSize);
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
      if (mMetricsText != text)
      {
         mMetricsText = text;
         std::vector<Engine::Graphics::Font::CharacterVertexUV> metricsText = mFont->Write(0, 0, 1, text);

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
            std::string ms = Format::FormatString("%1ms", std::round(system.second * 10000.0) / 10.0);
            ms.insert(ms.begin(), 7 - ms.size(), ' ');
            rightText += "\n" + std::move(ms);
         }
         std::vector<Engine::Graphics::Font::CharacterVertexUV> systemsText = mFont->Write(mBounds->Width() - 400, 0, 1, leftText);
         std::vector<Engine::Graphics::Font::CharacterVertexUV> rightUVs = mFont->Write(mBounds->Width() - 105, 0, 1, rightText);
         systemsText.insert(systemsText.end(), rightUVs.begin(), rightUVs.end());

         mSystemsCount = static_cast<GLint>(systemsText.size());
         mSystemsBenchmarkVBO.BufferData(sizeof(Engine::Graphics::Font::CharacterVertexUV) * mSystemsCount, &systemsText[0], GL_STATIC_DRAW);
      }
#endif
   }

   void DebugHelper::Render()
   {
      assert(mBounds);

      glUseProgram(program);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, mFont->GetTexture());
      glUniform1i(uTexture, 0);
      glUniform2f(uWindowSize, static_cast<GLfloat>(mBounds->Width()), static_cast<GLfloat>(mBounds->Height()));

      mMetricsTextVBO.AttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)0);
      mMetricsTextVBO.AttribPointer(aUV, 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)(sizeof(float) * 2));

      glDrawArrays(GL_LINES, 0, mMetricsCount);

      mSystemsBenchmarkVBO.AttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)0);
      mSystemsBenchmarkVBO.AttribPointer(aUV, 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)(sizeof(float) * 2));

      glDrawArrays(GL_LINES, 0, mSystemsCount);

      // Cleanup.
      glDisableVertexAttribArray(aPosition);
      glDisableVertexAttribArray(aUV);
      glUseProgram(0);
   }

}; // namespace Game

}; // namespace CubeWorld