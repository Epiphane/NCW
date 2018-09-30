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
      testRect->SetColor(1.0f, 0.0f, 0.0f, 0.5f);
      UIFrame funFrame = testRect->GetFrame();
      
      UIRectFilled *testRect2 = new UIRectFilled();
      AddChild(*testRect2);
      testRect2->SetColor(0.0f, 1.0f, 1.0f, 0.5f);
      UIFrame funnerFrame = testRect2->GetFrame();
      
      mSolver.add_constraints({
         funFrame.centerX == mFrame.centerX,
         funFrame.centerY == mFrame.centerY,
         funFrame.width == 200,
         funFrame.height == 300,
         
         funnerFrame.height == funFrame.height / 2,
         funnerFrame.width  == funFrame.width  / 2,
         funnerFrame.top    == funFrame.top,
         funnerFrame.left   == funFrame.right
      });
   }
   
}; // namespace Engine

}; // namespace CubeWorld
