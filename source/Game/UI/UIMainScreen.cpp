// By Thomas Steinke

#include "UIMainScreen.h"
#include <Shared/UI/RectFilled.h>
#include <Shared/UI/Text.h>

namespace CubeWorld
{

namespace Game
{

using Engine::UIFrame;
using UI::RectFilled;
using UI::Text;
 
UIMainScreen::UIMainScreen(Engine::Input* input) : UIRoot(input)
{
}
   
}; // namespace Game

}; // namespace CubeWorld
