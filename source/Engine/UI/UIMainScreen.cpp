// By Thomas Steinke

#include "UIMainScreen.h"
#include "UIRectFilled.h"
#include "UILabel.h"

namespace CubeWorld
{

namespace Engine
{
 
UIMainScreen::UIMainScreen(const Bounded& bounds)
   : UIRoot(bounds)
{
   UIRectFilled *testRect = Add<UIRectFilled>();
   testRect->SetColor({1.0f, 0.0f, 0.0f, 0.5f});
   UIFrame funFrame = testRect->GetFrame();
   
   UIRectFilled *testRect2 = Add<UIRectFilled>();
   testRect2->SetColor({0.0f, 1.0f, 1.0f, 0.5f});
   UIFrame funnerFrame = testRect2->GetFrame();
   
   UILabel *testLabel = Add<UILabel>();
   testLabel->SetText("farts");
   UIFrame textFrame = testLabel->GetFrame();

   mSolver.add_constraints({
      funFrame.centerX == mFrame.centerX,
      funFrame.centerY == mFrame.centerY,
         
      funFrame.width  == 200,
      funFrame.height == 300,
         
      funnerFrame.height == funFrame.height / 2,
      funnerFrame.width  == funFrame.width  / 2,
      funnerFrame.top    == funFrame.top,
      funnerFrame.left   == funFrame.right,
      
      textFrame.left == funFrame.left,
      textFrame.centerY == funFrame.centerY,
      textFrame.width == funFrame.width,
      textFrame.height == 30,
   });
}
   
}; // namespace Engine

}; // namespace CubeWorld
