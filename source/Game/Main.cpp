//
// NotCubeWorld - An attempt to make something that looks like CubeWorld
//

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cmath>
#include <stdio.h>

#include <Engine/Logger/Logger.h>
#include <Engine/Logger/StdoutLogger.h>
#include <Engine/Logger/DebugLogger.h>
#include <Engine/Core/Input.h>
#include <Engine/Core/StateManager.h>
#include <Engine/Core/Timer.h>
#include <Engine/Core/Window.h>
#include <Engine/Graphics/Program.h>

#include "Renderers/StupidRenderer.h"
#include "States/StupidState.h"
#include "Main.h"

using namespace CubeWorld;

void setWindowSize(int /*width*/, int /*height*/) {
   //aspect_ratio = float(width) / height;
   // Only aspect ratios 16/10 and 16/9 supported 
   //const float midrange = ((16.0f / 9.0f) + 1.6f) / 2.0f;
   //if (aspect_ratio < midrange) {
      // Window too tall!
      //aspect_ratio = 1.6f;
      //height = (int) (width / aspect_ratio);
   //}
   //else {
      // Window too wide!
      //aspect_ratio = 16.0f / 9.0f;
      //width = (int) (height * aspect_ratio);
   //}

   //w_width = width;
   //w_height = height;

   //if (window != nullptr) {
   //   glfwSetWindowSize(window, w_width, w_height);
   //}
}

const double FRAMES_PER_SEC = 60.0;
const double SEC_PER_FRAME = (1 / FRAMES_PER_SEC);

void setWindowSizeDefault() {
   const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
   setWindowSize(mode->width, mode->height);
}

int main(int /* argc */, char ** /* argv */) {
   using namespace Engine;

   // Initialize and register loggers to VS debugger and stdout
   Logger::StdoutLogger::Instance();
   Logger::DebugLogger::Instance();

   Window::Options windowOptions;
   windowOptions.title = "Not Cube World";
   windowOptions.fullscreen = false;
   windowOptions.width = 1280;
   windowOptions.height = 800;
   windowOptions.b = 0.4f;
   std::unique_ptr<Window> window = std::make_unique<Window>(windowOptions);
   
   Input::InputManager* input = Input::InputManager::Instance();
   input->Clear();

   std::unique_ptr<Engine::State> initialState = std::make_unique<Game::StupidState>();
   Engine::StateManager* stateManager = Engine::StateManager::Instance();

   stateManager->SetState(initialState.get());

   //nextState = new LoadingScreen();

   //RendererDebug::instance()->log("Hey there handsome \2", true);

   Timer<100> clock(SEC_PER_FRAME);
   do {
      /*if (nextState != NULL) {
         if (currentState != nullptr)
            currentState->pause();
         
         currentState = nextState;
         if (!currentState->initialized)
            currentState->start();
         
         currentState->unpause();
         
         nextState = NULL;
      }
      assert(currentState != NULL);*/

      double elapsed = clock.Elapsed();
      if (elapsed > 0)
      {
         window->Clear();
         input->Update();

         stateManager->Update(elapsed);

         GLenum error = glGetError();
         assert(error == 0);

         // Swap buffers
         window->SwapBuffers();
         glfwPollEvents();
      }

      /*double nextTime = glfwGetTime();
      if (nextTime - clock > SEC_PER_FRAME) {
         // Compute FPS
         const int fps_sample_rate = 100;
         static float samples[fps_sample_rate] = {1};
         static int pos = 0;
         samples[pos] = (float) (nextTime - clock);
         pos = (pos + 1) % fps_sample_rate;
         float elapsed = 0;
         for (int i = 0; i < fps_sample_rate; i ++)
            elapsed += samples[i];
         elapsed = elapsed / fps_sample_rate;
         
         float fps = 1 / elapsed;
         if (fps >= FRAMES_PER_SEC * 29.0f / 30.0f)
            ;
            //RendererDebug::instance()->log("FPS: " + std::to_string((int) FRAMES_PER_SEC) + " \2", false);
         else {
            std::string msg = "FPS: " + std::to_string((int) fps);
            //RendererDebug::instance()->log(msg, false);
            //RendererDebug::instance()->log("Time since last frame: " + std::to_string(elapsed), false);
         }
         
         //input_update();
         //audio_update();

         // Update and render the game
         // Use fixed time updating
         if (!PAUSED || moveOneFrame) {
            float dt = nextTime - clock;
            if (dt > 0.5f) {
               LOG_WARNING("Game hung. dt=" + std::to_string(dt));
               dt = 0.5f;
            }

            stateManager->Update(input, dt);
         }
         else {
            // Game is paused.
            stateManager->Update(input, 0);
         }
         moveOneFrame = false;
         
         //COMPUTE_BENCHMARK(25, "Game Update: ", true)

         currentState->render(glfwGetTime() - clock);*/
         
         //COMPUTE_BENCHMARK(25, "Game Render: ", true)
         
         //if (showDebugLog)
         //   RendererDebug::instance()->renderLog();
         //else
         //   RendererDebug::instance()->clearLog();
         
         //clock = nextTime;
      //}
   } // Check if the ESC key was pressed or the window was closed
   while (!window->ShouldClose() && !input->IsKeyDown(GLFW_KEY_ESCAPE));

   stateManager->Shutdown();

   return 0;
}

std::ostream &operator<< (std::ostream &out, const glm::vec2 &vec) {
   out << "{" << vec.x << ", " << vec.y << "}";
   return out;
}

std::ostream &operator<< (std::ostream &out, const glm::vec3 &vec) {
    out << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
    return out;
}

std::ostream &operator<< (std::ostream &out, const glm::vec4 &vec) {
    out << "{" << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w << "}";
    return out;
}

std::ostream &operator<< (std::ostream &out, const glm::mat4 &mat) {
    out << "\n{\t";
    for(int i = 0; i < 4; i ++) {
        for(int j = 0; j < 4; j ++) {
            if (mat[j][i] == 0)
                out << "0\t";
            else
                out << mat[j][i] << "\t";
        }
        if(i < 3)
            out << std::endl << "\t";
    }
    out << "}";
    return out;
}

// Given a vector of shapes which has already been read from an obj file
// resize all vertices to the range [-1, 1]
/*void resize_obj(std::vector<tinyobj::shape_t> &shapes){
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    float scaleX, scaleY, scaleZ;
    float shiftX, shiftY, shiftZ;
    float epsilon = 0.001f;
    
    minX = minY = minZ = 1.1754E+38F;
    maxX = maxY = maxZ = -1.1754E+38F;
    
    //Go through all vertices to determine min and max of each dimension
    for (size_t i = 0; i < shapes.size(); i++) {
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            if(shapes[i].mesh.positions[3*v+0] < minX) minX = shapes[i].mesh.positions[3*v+0];
            if(shapes[i].mesh.positions[3*v+0] > maxX) maxX = shapes[i].mesh.positions[3*v+0];
            
            if(shapes[i].mesh.positions[3*v+1] < minY) minY = shapes[i].mesh.positions[3*v+1];
            if(shapes[i].mesh.positions[3*v+1] > maxY) maxY = shapes[i].mesh.positions[3*v+1];
            
            if(shapes[i].mesh.positions[3*v+2] < minZ) minZ = shapes[i].mesh.positions[3*v+2];
            if(shapes[i].mesh.positions[3*v+2] > maxZ) maxZ = shapes[i].mesh.positions[3*v+2];
        }
    }
    //From min and max compute necessary scale and shift for each dimension
    float maxExtent = 0, xExtent, yExtent, zExtent;
    xExtent = maxX-minX;
    yExtent = maxY-minY;
    zExtent = maxZ-minZ;
    if (xExtent >= yExtent && xExtent >= zExtent) {
        maxExtent = xExtent;
    }
    if (yExtent >= xExtent && yExtent >= zExtent) {
        maxExtent = yExtent;
    }
    if (zExtent >= xExtent && zExtent >= yExtent) {
        maxExtent = zExtent;
    }
    scaleX = 2.0f / maxExtent;
    shiftX = minX + (xExtent / 2.0f);
    scaleY = 2.0f / maxExtent;
    shiftY = minY + (yExtent / 2.0f);
    scaleZ = 2.0f / maxExtent;
    shiftZ = minZ + (zExtent / 2.0f);
    
    //Go through all verticies shift and scale them
    for (size_t i = 0; i < shapes.size(); i++) {
        for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
            shapes[i].mesh.positions[3*v+0] = (shapes[i].mesh.positions[3*v+0] - shiftX) * scaleX;
            assert(shapes[i].mesh.positions[3*v+0] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3*v+0] <= 1.0 + epsilon);
            shapes[i].mesh.positions[3*v+1] = (shapes[i].mesh.positions[3*v+1] - shiftY) * scaleY;
            assert(shapes[i].mesh.positions[3*v+1] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3*v+1] <= 1.0 + epsilon);
            shapes[i].mesh.positions[3*v+2] = (shapes[i].mesh.positions[3*v+2] - shiftZ) * scaleZ;
            assert(shapes[i].mesh.positions[3*v+2] >= -1.0 - epsilon);
            assert(shapes[i].mesh.positions[3*v+2] <= 1.0 + epsilon);
        }
    }
}*/
