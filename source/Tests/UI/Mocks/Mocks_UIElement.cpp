//
// Mocks_UIElement.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "Mocks_UIElement.h"

#include <sstream>
#include <iostream>

#include <Engine/UI/UIElement.h>

#include <Engine/Core/Window.h>
#include <Engine/UI/UIRoot.h>

using namespace CubeWorld;
using Engine::UIRoot;
using Engine::UIElement;

namespace CubeWorld
{
   
glm::vec3 ElementCenter(UIElement* element) {
   return (element->GetFrame().GetTopRight() + element->GetFrame().GetBottomLeft()) / 2.0f;
}
   
void MockClick(UIRoot* root, UIElement* victim, double fakeX, double fakeY) {
   glm::vec3 center = ElementCenter(victim);
   
   MouseDownEvent down(0, center.x + fakeX, center.y + fakeX);
   MouseUpEvent     up(0, center.x + fakeX, center.y + fakeX);
   
   root->Receive(down);
   root->Receive(up);
}

void MockMouseDown(UIRoot* root, UIElement* victim, double fakeX, double fakeY) {
   glm::vec3 center = ElementCenter(victim);
   
   MouseDownEvent down(0, center.x + fakeX, center.y + fakeX);
   root->Receive(down);
}

void MockMouseUp(UIRoot* root, UIElement* victim, double fakeX, double fakeY) {
   glm::vec3 center = ElementCenter(victim);
   
   MouseUpEvent up(0, center.x + fakeX, center.y + fakeX);
   root->Receive(up);
}

void MockMouseMove(UIRoot* root, UIElement* victim, double fakeX, double fakeY) {
   glm::vec3 center = ElementCenter(victim);
   
   MouseMoveEvent move(center.x + fakeX, center.y + fakeX);
   root->Receive(move);
}

std::unique_ptr<UIRoot> CreateDummyUIRoot() {
   Engine::Window::Options windowOptions;
   Engine::Window& dummyWindow = Engine::Window::Instance();
   dummyWindow.Initialize(windowOptions);
   return std::make_unique<UIRoot>(&dummyWindow);
}

UIElement* FindChildByName(UIElement* baseElement, const std::string& name) {
   std::istringstream nameStream(name);
   std::string token;
   
   UIElement* currElement = baseElement;
   
   while (std::getline(nameStream, token, '.')) {
      bool foundElementForName = false;
      for (auto it = currElement->BeginChildren(); it != currElement->EndChildren(); it++) {
         if (it->GetName() == token) {
            currElement = it.GetPointer();
            foundElementForName = true;
            break;
         }
      }
      
      if (!foundElementForName) {
         assert(false && "Invalid name for UIElement :(");
      }
   }
   
   return currElement;
}
   
} // namespace CubeWorld

