// By Thomas Steinke

#include "UIMainScreen.h"
#include "UIRectFilled.h"

namespace CubeWorld
{

namespace Engine
{
 
UIMainScreen::UIMainScreen(const Bounded& bounds)
   : UIRoot(bounds)
{
   UIRectFilled *testRect = AddChild<UIRectFilled>();
   testRect->SetColor({1.0f, 0.0f, 0.0f, 0.5f});
   UIFrame funFrame = testRect->GetFrame();

   UIRectFilled *testRect2 = AddChild<UIRectFilled>();
   testRect2->SetColor({0.0f, 1.0f, 1.0f, 0.5f});
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
   
}; // namespace Engine

}; // namespace CubeWorld
