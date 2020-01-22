#include <stdio.h>

#include <GL/includes.h>
#include <GLFW/glfw3native.h>

#include <reactphysics3d.h>

// #include <RGBNetworking/JSONSerializer.h>
#include <RGBLogger/DebugLogger.h>
#include <RGBLogger/StdoutLogger.h>
#include <Engine/Core/Window.h>

using namespace CubeWorld;
using namespace CubeWorld::Engine;

int main(int, char**)
{
   Logger::StdoutLogger::Instance();
   Logger::DebugLogger::Instance();

   LOG_ALWAYS("Everything worked perfectly");

   rp3d::Vector3 gravity(0, rp3d::decimal(-9.81), 0);
   rp3d::DynamicsWorld world(gravity);

   rp3d::BoxShape box(rp3d::Vector3(1, 1, 1));
   rp3d::RigidBody* body = world.createRigidBody(rp3d::Transform::identity());
   //rp3d::ProxyShape* proxyShape = 
      body->addCollisionShape(&box, rp3d::Transform::identity(), 1);

   auto isGravityEnabled = world.isGravityEnabled();
   rp3d::Vector3 gravityVector = world.getGravity();
   auto isSleepingEnabled = world.isSleepingEnabled();
   auto sleepLinearVelocity = world.getSleepLinearVelocity();
   auto sleepAngularVelocity = world.getSleepAngularVelocity();
   auto nbPositionSolverIterations = world.getNbIterationsPositionSolver();
   auto nbVelocitySolverIterations = world.getNbIterationsVelocitySolver();
   auto timeBeforeSleep = world.getTimeBeforeSleep();

   LOG_ALWAYS("isGravityEnabled: %1", isGravityEnabled);
   LOG_ALWAYS("isSleepingEnabled: %1", isSleepingEnabled);
   LOG_ALWAYS("sleepLinearVelocity: %1", sleepLinearVelocity);
   LOG_ALWAYS("sleepAngularVelocity: %1", sleepAngularVelocity);
   LOG_ALWAYS("nbPositionSolverIterations: %1", nbPositionSolverIterations);
   LOG_ALWAYS("nbVelocitySolverIterations: %1", nbVelocitySolverIterations);
   LOG_ALWAYS("timeBeforeSleep: %1", timeBeforeSleep);

   auto t = body->getTransform().getPosition();
   LOG_ALWAYS("transform: %1 %2 %3", t.x, t.y, t.z);

   world.update(0.16);

   t = body->getTransform().getPosition();
   LOG_ALWAYS("transform: %1 %2 %3", t.x, t.y, t.z);

   /*
   // // Setup main window
   Window::Options windowOptions;
   windowOptions.title = "NCW Editor";
   windowOptions.fullscreen = false;
   windowOptions.width = 1280;
   windowOptions.height = 760;
   windowOptions.lockCursor = false;
   Window& window = Window::Instance();
   if (auto result = window.Initialize(windowOptions); !result)
   {
      return 1;
   }

   auto _ = window.AddCallback(GLFW_KEY_ESCAPE, [&](int, int, int) {
      window.SetShouldClose(true);
   });

   do {
      // Swap buffers
      window.SwapBuffers();
      glfwPollEvents();
   } // Check if the ESC key was pressed or the window was closed
   while (!window.ShouldClose());
   */

   return 0;
}