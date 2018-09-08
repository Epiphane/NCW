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

Window::Window() : window(nullptr)
{
}

Window::~Window()
{
   if (window != nullptr)
   {
      glfwDestroyWindow(window);
   }

   glfwTerminate();
}

Maybe<void> Window::Initialize(const Options& options)
{
   mOptions = options;
   if (!glfwInit()) {
      return Failure{"Failed to initialize GLFW"};
   }

   const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
   int monitorWidth = mode->width;
   int monitorHeight = mode->height;

   if (mOptions.width < 0 || mOptions.height < 0)
   {
      mOptions.width = monitorWidth;
      mOptions.height = monitorHeight;
   }

   glfwWindowHint(GLFW_SAMPLES, 4);
   glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

   GLFWmonitor *monitor = mOptions.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
   GLFWwindow *newWindow = glfwCreateWindow(mOptions.width, mOptions.height, mOptions.title.c_str(), monitor, window);
   if (newWindow == nullptr) {
      return Failure{"Failed to initialize GLFW"};
   }

   if (window != nullptr) {
      glfwDestroyWindow(window);
   }
   window = newWindow;
   glfwGetWindowSize(window, &mOptions.width, &mOptions.height);

   glfwMakeContextCurrent(window);

   glfwSetWindowPos(window, mOptions.x + monitorWidth / 2 - mOptions.width / 2, mOptions.y + monitorHeight / 2 - mOptions.height / 2);

   if (!gladLoadGL()) {
      return Failure{"Failed to initialize glad!"};
   }

   // Clear any errors that might be lying around.
   glGetError();

   //glEnable(GL_MULTISAMPLE);
   glEnable(GL_DEPTH_TEST);
   //glEnable(GL_CULL_FACE);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glClearColor(mOptions.r, mOptions.g, mOptions.b, mOptions.a);

   // Get some info about the situation
   const GLubyte *vendor = glGetString(GL_VENDOR);
   const GLubyte *renderer = glGetString(GL_RENDERER);
   LOG_INFO("Vendor: %s", vendor);
   LOG_INFO("Renderer: %s", renderer);

   mVAO.Bind();

   if (mOptions.lockCursor)
   {
      LockCursor();
   }

   return Success;
}

void Window::LockCursor()
{
   glfwSetCursorPos(window, mOptions.width / 2, mOptions.height / 2);
}

void Window::UnlockCursor()
{
}

void Window::Use()
{
   glfwMakeContextCurrent(window);
   mVAO.Bind();
}

}; // namespace Engine

}; // namespace CubeWorld
