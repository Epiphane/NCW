//
// NotCubeWorld - An attempt to make something that looks like CubeWorld
//

#include <algorithm>
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
#include <Engine/Graphics/Framebuffer.h>
#include <Engine/Graphics/Program.h>
#include <Engine/Graphics/VBO.h>

#include <Shared/DebugHelper.h>
#include "Helpers/Controls.h"
#include "Helpers/SubWindow.h"
#include "States/AnimationStation.h"

#include "Main.h"

using namespace CubeWorld;
using namespace Engine;

const double FRAMES_PER_SEC = 60.0;
const double SEC_PER_FRAME = (1 / FRAMES_PER_SEC);

int main(int /* argc */, char ** /* argv */) {
   // Initialize and register loggers to VS debugger and stdout
   Logger::StdoutLogger::Instance();
   Logger::DebugLogger::Instance();

   // Setup main window
   Window::Options windowOptions;
   windowOptions.title = "NCW Editor";
   windowOptions.fullscreen = false;
   windowOptions.width = 1280;
   windowOptions.height = 760;
   windowOptions.b = 0.4f;
   Window* window = Window::Instance();
   if (auto result = window->Initialize(windowOptions); !result)
   {
      LOG_ERROR("Failed creating window: %s", result.Failure().GetMessage());
      return 1;
   }
   
   Input::InputManager::Initialize(window);
   Input::InputManager* input = Input::InputManager::Instance();
   input->Clear();

   std::unique_ptr<Editor::Controls> controls = std::make_unique<Editor::Controls>(window, input);

   Editor::SubWindow::Options gameWindowOptions;
   gameWindowOptions.x = 250;
   gameWindowOptions.y = 250;
   gameWindowOptions.w = 1280 - 250;
   gameWindowOptions.h = 760 - 250;
   Editor::SubWindow gameWindow(window, input, gameWindowOptions);

   // Configure Debug helper
   Game::DebugHelper* debug = Game::DebugHelper::Instance();
   debug->SetBounds(&gameWindow);

   Timer<100> clock(SEC_PER_FRAME);
   auto fps = debug->RegisterMetric("FPS", [&clock]() -> std::string {
      return Format::FormatString("%1", std::round(1.0 / clock.Average()));
   });

   // Start with AnimationStation
   Engine::StateManager* stateManager = Engine::StateManager::Instance();
   stateManager->SetState(std::make_unique<Editor::AnimationStation>(float(gameWindowOptions.w) / gameWindowOptions.h, controls.get()));

   do {
      double elapsed = clock.Elapsed();
      if (elapsed > 0)
      {
         GLenum error;

         // Basic prep
         window->Clear();
         input->Update();

         gameWindow.Bind();

         // Render game state
         {
            stateManager->Update(std::min(elapsed, SEC_PER_FRAME));

            error = glGetError();
            assert(error == 0);
         }

         // Render debug stuff
         {
            debug->Update();
            debug->Render();

            error = glGetError();
            assert(error == 0);
         }

         // Pop game/debug into game section
         gameWindow.Unbind();
         gameWindow.Render();

         // Render controls
         {
            controls->Update();

            error = glGetError();
            assert(error == 0);
         }

         // Swap buffers
         {
            window->SwapBuffers();
            glfwPollEvents();
         }
      }
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
