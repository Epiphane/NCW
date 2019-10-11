//
// Mocks_UIElement.cpp
//
// This file created by the ELLIOT FISKE gang
//

#include "Mocks_UIElement.h"

#include <sstream>
#include <iostream>

#include <Engine/UI/UIElement.h>

#include "../../Mocks/MockInput.h"
#include <Engine/UI/UIRoot.h>

using namespace CubeWorld;
using Engine::UIElement;
using Test::MockInput;

namespace CubeWorld
{

glm::vec3 ElementCenter(UIElement* element) {
   return (element->GetFrame().GetTopRight() + element->GetFrame().GetBottomLeft()) / 2.0f;
}

void MockClick(Engine::UIRoot* root, Engine::UIElement* victim, double fakeX, double fakeY) {
   glm::vec3 center = ElementCenter(victim);

   MouseDownEvent down(0, center.x + fakeX, center.y + fakeY);
   MouseUpEvent     up(0, center.x + fakeX, center.y + fakeY);

   root->Receive(down);
   root->Receive(up);
}

void MockMouseDown(Engine::UIRoot* root, Engine::UIElement* victim, double fakeX, double fakeY) {
   glm::vec3 center = ElementCenter(victim);

   MouseDownEvent down(0, center.x + fakeX, center.y + fakeY);
   root->Receive(down);
}

void MockMouseUp(Engine::UIRoot* root, Engine::UIElement* victim, double fakeX, double fakeY) {
   glm::vec3 center = ElementCenter(victim);

   MouseUpEvent up(0, center.x + fakeX, center.y + fakeY);
   root->Receive(up);
}

void MockMouseMove(Engine::UIRoot* root, Engine::UIElement* victim, double fakeX, double fakeY) {
   glm::vec3 center = ElementCenter(victim);

   MouseMoveEvent move(center.x + fakeX, center.y + fakeY);
   root->Receive(move);
}

std::unique_ptr<Engine::UIRoot> CreateDummyUIRoot(Engine::Input& input) {
   return std::make_unique<Engine::UIRoot>(&input);
}

UIElement* FindChildByName(Engine::UIElement* baseElement, const std::string& name) {
   std::istringstream nameStream(name);
   std::string token;

   UIElement* currElement = baseElement;

   while (std::getline(nameStream, token, '.')) {
      bool foundElementForName = false;
      for (auto& child : currElement->GetChildren()) {
         if (child->GetName() == token) {
            currElement = child.get();
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
