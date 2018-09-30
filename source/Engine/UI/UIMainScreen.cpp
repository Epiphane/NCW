// By Thomas Steinke

#include "UIMainScreen.h"

namespace CubeWorld
{

namespace Engine
{
 
   UIMainScreen::UIMainScreen(Window* pWindow)
      : UIRoot(pWindow)
   {
      UIRectFilled *testRect = new UIRectFilled();
      AddChild(*testRect);
      UIFrame funFrame = testRect->GetFrame();
      
      mSolver.add_constraints({
         funFrame.centerX == mFrame.centerX,
         funFrame.centerY == mFrame.centerY,
         funFrame.width == 200,
         funFrame.height == 300,
      });
   }
   
}; // namespace Engine

}; // namespace CubeWorld
