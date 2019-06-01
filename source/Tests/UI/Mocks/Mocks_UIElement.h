//
// Mocks_UIElement.h
//
// Some functions useful to mess around with UIElements during testing
//
// This file created by the ELLIOT FISKE gang
//

#include <Engine/UI/UIElement.h>

namespace CubeWorld
{
   
// Fake a click on the given element.
void MockClick(Engine::UIElement* victim, double fakeX = 0, double fakeY = 0) {
   MouseDownEvent down(0, fakeX, fakeY);
   MouseUpEvent up(0, fakeX, fakeY);
   
   victim->MouseDown(down);
   victim->MouseUp(up);
}
   
} // namespace CubeWorld
