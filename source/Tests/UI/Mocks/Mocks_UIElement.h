//
// Mocks_UIElement.h
//
// Some functions useful to mess around with UIElements during testing
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <Engine/UI/UIElement.h>

#include <Engine/Core/Window.h>
#include <Engine/UI/UIRoot.h>

using namespace CubeWorld;
using Engine::UIRoot;

namespace CubeWorld
{
   
// Fake a click on the given element.
void MockClick(Engine::UIElement* victim, double fakeX = 0, double fakeY = 0) {
   MouseDownEvent down(0, fakeX, fakeY);
   MouseUpEvent up(0, fakeX, fakeY);
   
   victim->MouseDown(down);
   victim->MouseUp(up);
}
   
// Create a dummy UIRoot.
std::unique_ptr<UIRoot> CreateDummyUIRoot() {
   Engine::Window::Options windowOptions;
   Engine::Window& dummyWindow = Engine::Window::Instance();
   dummyWindow.Initialize(windowOptions);
   return std::make_unique<UIRoot>(&dummyWindow);
}
   
} // namespace CubeWorld
