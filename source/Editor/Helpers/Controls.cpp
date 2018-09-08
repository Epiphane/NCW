// By Thomas Steinke

#include <cassert>

#include <Engine/Graphics/Program.h>
#include <Engine/Logger/Logger.h>

#include <Shared/DebugHelper.h>
#include <Shared/Helpers/Asset.h>
#include "Controls.h"

namespace CubeWorld
{

namespace Editor
{

REGISTER_GLUINT(Controls, program);
REGISTER_GLUINT(Controls, aPosition);
REGISTER_GLUINT(Controls, aUV);
REGISTER_GLUINT(Controls, uTexture);
REGISTER_GLUINT(Controls, uWindowSize);

Controls::Controls(Bounded* bounds)
   : mBounds(bounds)
   , mLayout{{}}
   , mControlsTextVBO(Engine::Graphics::VBO::Vertices)
   , mControlsTextCount(0)
{
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

Controls::~Controls()
{
}

void Controls::Update()
{
   glUseProgram(program);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mFont->GetTexture());
   glUniform1i(uTexture, 0);
   glUniform2f(uWindowSize, static_cast<GLfloat>(mBounds->Width()), static_cast<GLfloat>(mBounds->Height()));

   mControlsTextVBO.AttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)0);
   mControlsTextVBO.AttribPointer(aUV, 2, GL_FLOAT, GL_FALSE, sizeof(Engine::Graphics::Font::CharacterVertexUV), (void*)(sizeof(float) * 2));

   glDrawArrays(GL_LINES, 0, mControlsTextCount);

   {
      GLenum err = glGetError();
      assert(err == 0);
   }

   // Cleanup.
   glDisableVertexAttribArray(aPosition);
   glDisableVertexAttribArray(aUV);
   glUseProgram(0);
}

void Controls::SetLayout(const Layout& layout)
{
   mLayout = layout;

   Rebuild();
}

void Controls::Rebuild()
{
   std::string controls;
   std::vector<Engine::Graphics::Font::CharacterVertexUV> uvs;
   for (size_t i = 0; i < mLayout.elements.size(); ++i)
   {
      std::string label = mLayout.elements[i].label;
      if (i == mHoveredItem)
      {
         label = "> " + label;
      }
      std::vector<Engine::Graphics::Font::CharacterVertexUV> item = mFont->Write(10, i * 35 + 10, 1, label);
      uvs.insert(uvs.end(), item.begin(), item.end());
   }
      
   mControlsTextCount = static_cast<GLint>(uvs.size());
   mControlsTextVBO.BufferData(sizeof(Engine::Graphics::Font::CharacterVertexUV) * mControlsTextCount, &uvs[0], GL_STATIC_DRAW);
}

void Controls::MouseDown(int button, double x, double y)
{
}

void Controls::MouseUp(int button, double x, double y)
{
}

void Controls::MouseClick(int button, double x, double y)
{
   double pixelY = y * mBounds->Height();
   uint32_t item = (pixelY - 10) / 35;

   if (item < mLayout.elements.size())
   {
      if (mLayout.elements[item].callback)
      {
         mLayout.elements[item].callback();
      }
      else
      {
         LOG_ALWAYS("Clicked \"%1\", but it has no callback.", mLayout.elements[item].label);
      }
   }
}

void Controls::MouseDrag(int button, double x, double y)
{
}

void Controls::MouseMove(double x, double y)
{
   double pixelY = y * mBounds->Height();
   uint32_t item = (pixelY - 10) / 35;
   if (item >= mLayout.elements.size())
   {
      item = -1;
   }

   if (item != mHoveredItem)
   {
      mHoveredItem = item;
      Rebuild();
   }
}

}; // namespace Editor

}; // namespace CubeWorld
