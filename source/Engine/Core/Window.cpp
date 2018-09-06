// By Thomas Steinke

#include "../Core/Input.h"
#include "../Logger/Logger.h"
#include "../Graphics/VAO.h"
#include "Window.h"
#include "Singleton.h"

namespace CubeWorld
{

namespace Engine
{

Window* Window::root = nullptr;

Window::Window(Window::Options options)
   : window(nullptr)
   , mOptions(options)
{
   if (!glfwInit()) {
      LOG_ERROR("Failed to initialize GLFW");
      return;
   }

   const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
   int monitorWidth = mode->width;
   int monitorHeight = mode->height;

   if (options.width < 0 || options.height < 0)
   {
      options.width = monitorWidth;
      options.height = monitorHeight;
   }

   glfwWindowHint(GLFW_SAMPLES, 4);
   glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

   // Add to the list of windows
   if (root == nullptr)
   {
      root = this;
   }
   next = root;
   prev = root->prev;
   root->prev = this;
   prev->next = this;

   // Get the "origin context"
   GLFWwindow* glContext = root->window;

   GLFWmonitor *monitor = options.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
   GLFWwindow *newWindow = glfwCreateWindow(options.width, options.height, options.title.c_str(), monitor, glContext);
   if (newWindow == nullptr) {
      LOG_ERROR("Failed to initialize GLFW");
      return;
   }

   if (window != nullptr) {
      glfwDestroyWindow(window);
   }
   window = newWindow;

   glfwMakeContextCurrent(window);

   glfwSetWindowPos(window, options.x + monitorWidth / 2 - options.width / 2, options.y + monitorHeight / 2 - options.height / 2);// + (monitorWidth - options.width) / 2, options.y + (monitorHeight - options.height) / 2);

   if (glContext == nullptr)
   {
      glewExperimental = true;

      if (GLenum result = glewInit(); result != GLEW_OK) {
         LOG_ERROR("Failed to initialize GLEW: %s", glewGetErrorString(result));
         return;
      }

      // Clear any errors that might be lying around.
      glGetError();

      //glEnable(GL_MULTISAMPLE);
      glEnable(GL_DEPTH_TEST);
      //glEnable(GL_CULL_FACE);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      glClearColor(options.r, options.g, options.b, options.a);

      // Get some info about the situation
      const GLubyte *vendor = glGetString(GL_VENDOR);
      const GLubyte *renderer = glGetString(GL_RENDERER);
      LOG_INFO("Vendor: %s", vendor);
      LOG_INFO("Renderer: %s", renderer);
   }

   mVAO.Bind();

   if (options.lockCursor)
   {
      LockCursor();
   }
}

Window::~Window()
{
   if (window != nullptr)
   {
      glfwDestroyWindow(window);
   }

   glfwTerminate();
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
