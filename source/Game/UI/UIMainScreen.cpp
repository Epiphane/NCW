// By Thomas Steinke

#include "UIMainScreen.h"
#include <Shared/UI/RectFilled.h>

namespace CubeWorld
{

namespace Game
{

using Engine::UIFrame;
using UI::RectFilled;
 
UIMainScreen::UIMainScreen(const Bounded& bounds)
   : UIRoot(bounds)
{
   RectFilled *testRect = Add<RectFilled>(glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
   UIFrame funFrame = testRect->GetFrame();

   RectFilled *testRect2 = Add<RectFilled>(glm::vec4(0.0f, 1.0f, 1.0f, 0.5f));
   UIFrame funnerFrame = testRect2->GetFrame();

   mSolver.add_constraints({
      funFrame.centerX == mFrame.centerX,
      funFrame.centerY == mFrame.centerY,
         
      funFrame.width  == 200,
      funFrame.height == 300,
         
      funnerFrame.height == funFrame.height / 2,
      funnerFrame.width  == funFrame.width  / 2,
      funnerFrame.top    == funFrame.top,
      funnerFrame.left   == funFrame.right
   });
}
   
}; // namespace Game

}; // namespace CubeWorld
