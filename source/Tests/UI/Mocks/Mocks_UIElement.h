//
// Mocks_UIElement.h
//
// Some functions useful to mess around with UIElements during testing
//
// This file created by the ELLIOT FISKE gang
//

#pragma once

#include <string>

#include <Engine/UI/UIRoot.h>

namespace CubeWorld
{
   
// Fake various mouse events on a given element. NOTE: fakeX and fakeY are offset from the CENTER of the victim.
//    Also note: This mocks a click that is sent TO the actual UIRoot. If there is another element in the
//                way for instance, the click won't come through to the 'victim' element. 
void MockClick    (Engine::UIRoot* root, Engine::UIElement* victim, double fakeX = 0, double fakeY = 0);
void MockMouseDown(Engine::UIRoot* root, Engine::UIElement* victim, double fakeX = 0, double fakeY = 0);
void MockMouseUp  (Engine::UIRoot* root, Engine::UIElement* victim, double fakeX = 0, double fakeY = 0);
void MockMouseMove(Engine::UIRoot* root, Engine::UIElement* victim, double fakeX = 0, double fakeY = 0);

// Create a dummy UIRoot.
std::unique_ptr<Engine::UIRoot> CreateDummyUIRoot();
   
// Get a reference to a UIElement by name. To go deeper into the 
//    hierarchy, chain names with a period.
//
// This function asserts if it can't find the element.
Engine::UIElement* FindChildByName(Engine::UIElement* baseElement, const std::string& name);
   
} // namespace CubeWorld
