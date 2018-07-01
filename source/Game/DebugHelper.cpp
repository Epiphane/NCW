// By Thomas Steinke

#include <Engine/Logger/Logger.h>
#include <Engine/Graphics/Program.h>

#include "DebugHelper.h"

namespace CubeWorld
{

namespace Game
{

   REGISTER_GLUINT(DebugHelper, program)
   REGISTER_GLUINT(DebugHelper, aPosition)
   REGISTER_GLUINT(DebugHelper, aUV)
   REGISTER_GLUINT(DebugHelper, uTexture)
   REGISTER_GLUINT(DebugHelper, uWindowSize)

   DebugHelper::DebugHelper()
      : mWindow(nullptr)
      , mFont(nullptr)
      , mMetrics{}
      , mMetricsTextVBO(Engine::Graphics::Vertices)
   {
      auto maybeResult = Engine::Graphics::FontManager::Instance()->GetFont("Fonts/debug.ttf");
      assert(maybeResult);
      mFont = maybeResult.Left();

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

   void DebugHelper::RegisterMetric(const std::string& name, const std::function<std::string(void)>& fn)
   {
      mMetrics.push_back(std::make_pair(name, fn));
   }

   void DebugHelper::Update()
   {
      mMetricsState.clear();
      std::string text = "DEBUG:";
      for (auto metric : mMetrics)
      {
         std::pair<std::string, std::string> result = std::make_pair(metric.first, metric.second());
         mMetricsState.push_back(result);

         text += "\n" + result.first + ": " + result.second;
      }
      if (mMetricsText != text)
      {
         mMetricsText = text;
         std::vector<Engine::Graphics::Font::CharacterVertexUV> metricsText = mFont->Write(0, 0, 1, text);

         mMetricsCount = metricsText.size();
         mMetricsTextVBO.BufferData(sizeof(Engine::Graphics::Font::CharacterVertexUV) * mMetricsCount, &metricsText[0], GL_STATIC_DRAW);
      }
   }

   void DebugHelper::Render()
   {
      assert(mWindow);

      glUseProgram(program);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, mFont->GetTexture());
      glUniform1i(uTexture, 0);
      glUniform2f(uWindowSize, mWindow->Width(), mWindow->Height());

      mMetricsTextVBO.AttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)0);
      mMetricsTextVBO.AttribPointer(aUV, 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)(sizeof(float) * 2));

      glDrawArrays(GL_LINES, 0, mMetricsCount);

      // Cleanup.
      glDisableVertexAttribArray(aPosition);
      glDisableVertexAttribArray(aUV);
      glUseProgram(0);
   }

}; // namespace Game

}; // namespace CubeWorld