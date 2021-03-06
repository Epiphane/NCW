// By Thomas Steinke

#include <glad/glad.h>
#include <imgui.h>

#include <RGBDesignPatterns/Scope.h>
#include <Engine/Core/Window.h>
#include <RGBLogger/Logger.h>

#include "Helpers/Asset.h"
#include "DebugHelper.h"

namespace CubeWorld
{

std::unique_ptr<Engine::Graphics::Program> DebugHelper::program = nullptr;

DebugHelper::DebugHelper()
   : mBounds(nullptr)
   , mFont(nullptr)
   , mSystemManager(nullptr)
{
   mMetrics = std::make_unique<MetricLink>(this, "None", nullptr);
   mMetrics->next = mMetrics->prev = mMetrics.get();

   auto maybeFont = Engine::Graphics::FontManager::Instance().GetFont(Asset::Font("debug"));
   assert(maybeFont);
   mFont = *maybeFont;

   if (!program)
   {
      auto maybeProgram = Engine::Graphics::Program::Load(
          Asset::Shader("DebugText.vert"),
          Asset::Shader("DebugText.geom"),
          Asset::Shader("DebugText.frag")
      );
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

   return link;
}

void DebugHelper::SetMetric(const std::string& name, const std::string& value)
{
    if (mGlobalMetrics.size() == 0)
    {
        return;
    }

    if (mGlobalMetrics.count(name) == 0)
    {
        mGlobalMetricLinks.push_back(RegisterMetric(name, [this, name] {
            return mGlobalMetrics.at(name);
        }));
    }

    mGlobalMetrics[name] = value;
}

void DebugHelper::DeregisterMetric(std::unique_ptr<MetricLink> /*metric*/)
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

void DebugHelper::Update(bool imgui)
{
   if (mBounds == nullptr)
   {
      return;
   }

   float left = 0, top = 0, right = 0;
   if (imgui)
   {
       ImGui::Begin("Debug Metrics");

       MetricLink* metric = mMetrics->next;
       while (metric != mMetrics.get())
       {
           if (metric->callback)
           {
               std::pair<std::string, std::string> result = std::make_pair(metric->name, metric->callback());
               ImGui::Text("%s: %s", result.first.c_str(), result.second.c_str());
           }
           metric = metric->next;
       }

       ImGui::End();
   }
   else
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

       left = GLfloat(mBounds->GetX());
       top = GLfloat(mBounds->GetY() + mBounds->GetHeight()) - 40.0f;
       right = left + GLfloat(mBounds->GetWidth());
       if (mMetricsText != text)
       {
           mMetricsText = text;
           std::vector<Engine::Graphics::Font::CharacterVertexUV> metricsText = mFont->Write(left, top, 0, 1, text, Engine::Graphics::Font::Left);

           mMetricsCount = static_cast<GLint>(metricsText.size());
           mMetricsTextVBO.BufferData(sizeof(Engine::Graphics::Font::CharacterVertexUV) * mMetricsCount, &metricsText[0], GL_STATIC_DRAW);
       }
   }

#if CUBEWORLD_BENCHMARK_SYSTEMS
   if (mSystemManager != nullptr)
   {
       if (imgui)
       {
           ImGui::Begin("System Performance");
           for (auto system : mSystemManager->GetBenchmarks())
           {
               ImVec4 color(1, 1, 1, 1);
               if (system.second < 0.0001)
               {
                   continue;
               }
               if (system.second > 0.001)
               {
                   color.z = 0;
               }
               if (system.second > 0.01)
               {
                   color.y = 0;
               }
               ImGui::TextColored(color, "%7s: %.1fms", system.first.c_str(), std::floor(system.second * 100000.0) / 100.0);
           }
           ImGui::End();
       }
       else
       {
           std::string leftText = "-------- Systems ------";
           std::string rightText = "------";
           for (auto system : mSystemManager->GetBenchmarks())
           {
               leftText += "\n" + system.first;
               std::string ms = FormatString("%.1fms", system.second * 1000.0);
               if (ms.size() < 7)
               {
                   ms.insert(ms.begin(), 7 - ms.size(), ' ');
               }
               rightText += "\n" + std::move(ms);
           }
           std::vector<Engine::Graphics::Font::CharacterVertexUV> systemsText = mFont->Write(right - 400, top, 0, 1, leftText, Engine::Graphics::Font::Left);
           std::vector<Engine::Graphics::Font::CharacterVertexUV> rightUVs = mFont->Write(right - 105, top, 0, 1, rightText, Engine::Graphics::Font::Left);
           systemsText.insert(systemsText.end(), rightUVs.begin(), rightUVs.end());

           mSystemsCount = static_cast<GLint>(systemsText.size());
           mSystemsBenchmarkVBO.BufferData(sizeof(Engine::Graphics::Font::CharacterVertexUV) * mSystemsCount, &systemsText[0], GL_STATIC_DRAW);
       }
   }
#endif
}

void DebugHelper::Render(bool imgui)
{
   if (mBounds == nullptr || imgui)
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
   program->Uniform2f("uWindowSize", static_cast<GLfloat>(Engine::Window::Instance().GetWidth()), static_cast<GLfloat>(Engine::Window::Instance().GetHeight()));

   mMetricsTextVBO.AttribPointer(program->Attrib("aPosition"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)0);
   mMetricsTextVBO.AttribPointer(program->Attrib("aUV"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)(sizeof(float) * 2));

   glDrawArrays(GL_LINES, 0, mMetricsCount);

   mSystemsBenchmarkVBO.AttribPointer(program->Attrib("aPosition"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)0);
   mSystemsBenchmarkVBO.AttribPointer(program->Attrib("aUV"), 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)(sizeof(float) * 2));

   glDrawArrays(GL_LINES, 0, mSystemsCount);
}

}; // namespace CubeWorld
