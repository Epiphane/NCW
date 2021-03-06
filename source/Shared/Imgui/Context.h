// By Thomas Steinke

#pragma once

#include <memory>
#include <imgui.h>

#include <Engine/Core/Window.h>
#include <Engine/Core/Config.h>
#include <Engine/Graphics/VBO.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/TextureManager.h>

namespace CubeWorld
{

namespace Editor
{

class ImguiContext
{
public:
   ImguiContext(Engine::Window& window);
   ~ImguiContext();

   void StartFrame(TIMEDELTA dt);
   void Render();

private:
   void OnKey(int key, int action, int mods);
   void OnChar(unsigned int character);
   void ResetRenderState(ImDrawData* drawData, int fbWidth, int fbHeight);

private:
   // Parameters.
   Engine::Window& mWindow;
   std::unique_ptr<Engine::Input::KeyCallbackLink> mKeyCallback;
   std::unique_ptr<Engine::Input::CharCallbackLink> mCharCallback;

   // Graphics resources.
   Engine::Graphics::VBO mVBO;
   Engine::Graphics::VBO mElements;
   std::unique_ptr<Engine::Graphics::Program> mProgram;
   std::unique_ptr<Engine::Graphics::Texture> mFontTexture;

   // State.
   GLFWcursor* mCursors[ImGuiMouseCursor_COUNT] = {0};
};

}; // namespace Editor

}; // namespace CubeWorld
