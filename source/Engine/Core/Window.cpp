// By Thomas Steinke

#include "../Core/Input.h"
#include "../Logger/Logger.h"
#include "../Graphics/VAO.h"
#include "Window.h"
#include "Scope.h"
#include "Singleton.h"

namespace CubeWorld
{

namespace Engine
{

Window::Window() : mGLFW(nullptr)
{
}

Window::~Window()
{
   if (mGLFW != nullptr)
   {
      glfwDestroyWindow(mGLFW);
   }

   glfwTerminate();
}

Maybe<Window*> Window::Initialize(const Options& options)
{
   mOptions = options;
   if (!glfwInit()) {
      return Failure{"Failed to initialize GLFW"};
   }

   // Get monitor dimensions.
   const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
   int monitorWidth = mode->width;
   int monitorHeight = mode->height;

   // Use the monitor size as a default
   if (mOptions.width < 0 || mOptions.height < 0)
   {
      mOptions.width = monitorWidth;
      mOptions.height = monitorHeight;
   }

   // Window settings. We don't wanna change these.
   glfwWindowHint(GLFW_SAMPLES, 4);
   glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

   // Create the window itself
   GLFWmonitor *monitor = mOptions.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
   GLFWwindow *newWindow = glfwCreateWindow(mOptions.width, mOptions.height, mOptions.title.c_str(), monitor, mGLFW);
   if (newWindow == nullptr) {
      return Failure{"Failed to initialize GLFW"};
   }

   // Clean up any old window that might have existed.
   if (mGLFW != nullptr) {
      glfwDestroyWindow(mGLFW);
   }
   
   // Resize and reposition!
   mGLFW = newWindow;
   glfwGetWindowSize(mGLFW, &mOptions.width, &mOptions.height);
   glfwSetWindowPos(mGLFW, mOptions.x + monitorWidth / 2 - mOptions.width / 2, mOptions.y + monitorHeight / 2 - mOptions.height / 2);
   glfwMakeContextCurrent(mGLFW);

   // Initialize OpenGL context
   if (!gladLoadGL()) {
      return Failure{"Failed to initialize glad!"};
   }

   // Clear any errors that might be lying around.
   glGetError();

   // Configure GL
   {
      //glEnable(GL_MULTISAMPLE);
      //glEnable(GL_CULL_FACE);
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glClearColor(mOptions.r, mOptions.g, mOptions.b, mOptions.a);
   }

   // Get some info about the situation
   {
      const GLubyte *vendor = glGetString(GL_VENDOR);
      const GLubyte *renderer = glGetString(GL_RENDERER);
      LOG_INFO("Vendor: %s", vendor);
      LOG_INFO("Renderer: %s", renderer);
   }

   mVAO.Bind();

   // Initialize input.
   mInput.Initialize(this);
   mInput.SetMouseLock(mOptions.lockCursor);

   return this;
}

void Window::Clear()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::SwapBuffers()
{
   glfwSwapBuffers(mGLFW);
}

void Window::SetShouldClose(bool close)
{
   glfwSetWindowShouldClose(mGLFW, close);
}

bool Window::ShouldClose()
{
   return glfwWindowShouldClose(mGLFW) != 0;
}

void Window::Focus()
{
   glfwFocusWindow(mGLFW);
}

}; // namespace Engine

}; // namespace CubeWorld
