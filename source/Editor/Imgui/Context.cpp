// By Thomas Steinke

#include <GL/includes.h>

#include "Context.h"

namespace CubeWorld
{

namespace Editor
{

static const char* GetClipboardText(void* user_data)
{
   return glfwGetClipboardString((GLFWwindow*)user_data);
}

static void SetClipboardText(void* user_data, const char* text)
{
   glfwSetClipboardString((GLFWwindow*)user_data, text);
}

ImguiContext::ImguiContext(Engine::Window& window)
   : mWindow(window)
   , mVBO(Engine::Graphics::VBO::Vertices)
   , mElements(Engine::Graphics::VBO::Indices)
{
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGui::StyleColorsDark();

   // Setup back-end capabilities flags
   ImGuiIO& io = ImGui::GetIO();
   io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
   io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
   io.BackendPlatformName = "imgui_impl_glfw";

   // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
   io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
   io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
   io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
   io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
   io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
   io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
   io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
   io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
   io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
   io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
   io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
   io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
   io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
   io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
   io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
   io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
   io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
   io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
   io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
   io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
   io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
   io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

   io.SetClipboardTextFn = SetClipboardText;
   io.GetClipboardTextFn = GetClipboardText;
   io.ClipboardUserData = mWindow.get();

   mCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
   mCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
   mCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);   // FIXME: GLFW doesn't have this.
   mCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
   mCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
   mCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);  // FIXME: GLFW doesn't have this.
   mCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);  // FIXME: GLFW doesn't have this.
   mCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

   // Prepare graphics resources.
   auto maybeProgram = Engine::Graphics::Program::Load(
      "Shaders/Imgui.vert",
      "Shaders/Imgui.frag"
   );

   if (!maybeProgram)
   {
      maybeProgram.Failure().WithContext("Failed loading Imgui program").Log();
   }
   else
   {
      mProgram = std::move(*maybeProgram);
   }

   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small)
   // because it is more likely to be compatible with user's existing shaders. If your
   // ImTextureId represent a higher-level concept than just a GL texture id, consider
   // calling GetTexDataAsAlpha8() instead to save on GPU memory.
   unsigned char* pixels;
   int width, height;
   io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

   auto maybeTexture = Engine::Graphics::Texture::FromBytes(pixels, (uint32_t)width, (uint32_t)height);
   if (!maybeTexture)
   {
      maybeTexture.Failure().WithContext("Failed loading Imgui font").Log();
   }
   else
   {
      mFontTexture = std::move(*maybeTexture);
   }

   // Store our identifier
   io.Fonts->TexID = (ImTextureID)(intptr_t)mFontTexture->GetTexture();
}

ImguiContext::~ImguiContext()
{
   for (ImGuiMouseCursor i = 0; i < ImGuiMouseCursor_COUNT; ++i)
   {
      glfwDestroyCursor(mCursors[i]);
      mCursors[i] = nullptr;
   }
   ImGui::DestroyContext();
}

void ImguiContext::StartFrame(TIMEDELTA dt)
{
   ImGuiIO& io = ImGui::GetIO();

   // Setup display size (every frame to accommodate for window resizing)
   int w, h;
   int display_w, display_h;
   glfwGetWindowSize(mWindow.get(), &w, &h);
   glfwGetFramebufferSize(mWindow.get(), &display_w, &display_h);
   io.DisplaySize = ImVec2((float)w, (float)h);
   if (w > 0 && h > 0)
   {
      io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);
   }

   // Setup time step
   io.DeltaTime = (float)dt;

   // Update mouse position.
   auto mouse = mWindow.GetRawMousePosition();
   io.MousePos = ImVec2((float)mouse.x, (float)mWindow.GetHeight() - mouse.y);

   // Update mouse click state
   for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
   {
      // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
      io.MouseDown[i] = mWindow.IsMouseDown(i);
   }

   // Update mouse cursor.
   if (
      (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0 &&
      glfwGetInputMode(mWindow.get(), GLFW_CURSOR) != GLFW_CURSOR_DISABLED
   )
   {
      ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
      if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
      {
         // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
         glfwSetInputMode(mWindow.get(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
      }
      else
      {
         // Show OS mouse cursor
         // FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
         glfwSetCursor(mWindow.get(), mCursors[imgui_cursor] ? mCursors[imgui_cursor] : mCursors[ImGuiMouseCursor_Arrow]);
         glfwSetInputMode(mWindow.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      }
   }

   // Start recording!
   ImGui::NewFrame();
}

void ImguiContext::ResetRenderState(ImDrawData* drawData, int fbWidth, int fbHeight)
{
   glDisable(GL_CULL_FACE);
   glDisable(GL_DEPTH_TEST);
   glEnable(GL_SCISSOR_TEST);

   // Setup viewport, orthographic projection matrix
   // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
   glViewport(0, 0, (GLsizei)fbWidth, (GLsizei)fbHeight);
   double L = drawData->DisplayPos.x;
   double R = drawData->DisplayPos.x + drawData->DisplaySize.x;
   double T = drawData->DisplayPos.y;
   double B = drawData->DisplayPos.y + drawData->DisplaySize.y;
   glm::mat4 projection =
   {
       { 2.0 / (R - L),      0.0,              0.0,   0.0 },
       { 0.0,                2.0 / (T - B),    0.0,   0.0 },
       { 0.0,                0.0,             -1.0,   0.0 },
       { (R + L) / (L - R),   (T + B) / (B - T), 0.0,   1.0 },
   };
   mProgram->Uniform1i("uTexture", 0);
   mProgram->UniformMatrix4f("uProjMatrix", projection);

   // Bind vertex/index buffers and setup attributes for ImDrawVert
   mVBO.Bind();
   mElements.Bind();
   mVBO.AttribPointer(mProgram->Attrib("aPosition"), 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
   mVBO.AttribPointer(mProgram->Attrib("aUV"), 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
   mVBO.AttribPointer(mProgram->Attrib("aColor"), 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, col));
}

void ImguiContext::Render()
{
   ImGui::Render();

   ImDrawData* drawData = ImGui::GetDrawData();

   int fbWidth = (int)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
   int fbHeight = (int)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
   if (fbWidth <= 0 || fbHeight <= 0)
   {
      return;
   }
   
   // Setup GL state
   BIND_PROGRAM_IN_SCOPE(mProgram);
   ResetRenderState(drawData, fbWidth, fbHeight);
   CUBEWORLD_SCOPE_EXIT([] {
      glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);
      glDisable(GL_SCISSOR_TEST);
   });

   // Will project scissor/clipping rectangles into framebuffer space
   ImVec2 clip_off = drawData->DisplayPos;         // (0,0) unless using multi-viewports
   ImVec2 clip_scale = drawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)
   bool clip_origin_lower_left = true;
#if defined(GL_CLIP_ORIGIN) && !defined(__APPLE__)
   GLenum last_clip_origin = 0; glGetIntegerv(GL_CLIP_ORIGIN, (GLint*)& last_clip_origin); // Support for GL 4.5's glClipControl(GL_UPPER_LEFT)
   if (last_clip_origin == GL_UPPER_LEFT)
      clip_origin_lower_left = false;
#endif

   // Render command lists
   for (int n = 0; n < drawData->CmdListsCount; n++)
   {
      const ImDrawList* cmdList = drawData->CmdLists[n];

      // Upload vertex/index buffers
      glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(cmdList->VtxBuffer.Size * sizeof(ImDrawVert)), (const GLvoid*)cmdList->VtxBuffer.Data, GL_STREAM_DRAW);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(cmdList->IdxBuffer.Size * sizeof(ImDrawIdx)), (const GLvoid*)cmdList->IdxBuffer.Data, GL_STREAM_DRAW);

      for (int cmd_i = 0; cmd_i < cmdList->CmdBuffer.Size; cmd_i++)
      {
         const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmd_i];
         if (pcmd->UserCallback != nullptr)
         {
            // User callback, registered via ImDrawList::AddCallback()
            // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
            if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
            {
               ResetRenderState(drawData, fbWidth, fbHeight);
            }
            else
            {
               pcmd->UserCallback(cmdList, pcmd);
            }
         }
         else
         {
            // Project scissor/clipping rectangles into framebuffer space
            ImVec4 clip;
            clip.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
            clip.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
            clip.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
            clip.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

            if (clip.x < fbWidth && clip.y < fbHeight && clip.z >= 0.0f && clip.w >= 0.0f)
            {
               // Apply scissor/clipping rectangle
               if (clip_origin_lower_left)
               {
                  glScissor((int)clip.x, (int)(fbHeight - clip.w), (int)(clip.z - clip.x), (int)(clip.w - clip.y));
               }
               else
               {
                  glScissor((int)clip.x, (int)clip.y, (int)clip.z, (int)clip.w); // Support for GL 4.5 rarely used glClipControl(GL_UPPER_LEFT)
               }

               // Bind texture and draw
               glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
               glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)));
               CHECK_GL_ERRORS();
            }
         }
      }
   }
}

}; // namespace Editor

}; // namespace CubeWorld
