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
   
void MockClick(UIElement* victim, double fakeX, double fakeY) {
   MouseDownEvent down(0, fakeX, fakeY);
   MouseUpEvent up(0, fakeX, fakeY);
   
   victim->MouseDown(down);
   victim->MouseUp(up);
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
         }
      }
      
      if (!foundElementForName) {
         assert(false && "Invalid name for UIElement :(");
      }
   }
   
   return currElement;
}
   
} // namespace CubeWorld

