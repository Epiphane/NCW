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
 
UIMainScreen::UIMainScreen()
{
   RectFilled *testRect = Add<RectFilled>("DumbTestRect", glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
   UIFrame funFrame = testRect->GetFrame();

   RectFilled *testRect2 = Add<RectFilled>("DumberTestRect", glm::vec4(0.0f, 1.0f, 1.0f, 0.5f));
   UIFrame funnerFrame = testRect2->GetFrame();
   
   Text *text1 = Add<Text>(Text::Options{"Test Center :P"});
   text1->SetAlignment(Engine::Graphics::Font::Alignment::Center);
   UIFrame textFrame = text1->GetFrame();
   
   Text *text2 = Add<Text>(Text::Options{"I'm right!"});
   text2->SetAlignment(Engine::Graphics::Font::Alignment::Right);
   UIFrame textFrame2 = text2->GetFrame();
   
   mSolver.add_constraints({
      funFrame.centerX == mFrame.centerX,
      funFrame.centerY == mFrame.centerY,
         
      funFrame.width  == 200,
      funFrame.height == 300,
         
      funnerFrame.height == funFrame.height / 2,
      funnerFrame.width  == funFrame.width  / 2,
      funnerFrame.top    == funFrame.top,
      funnerFrame.left   == funFrame.right,
      
      textFrame.left    == funFrame.left,
      textFrame.right   == funFrame.right,
      textFrame.centerY == funFrame.centerY,
      textFrame.height  == 30,
      
      textFrame2.right  == funFrame.right,
      textFrame2.width  == funFrame.width,
      textFrame2.height == 30,
      textFrame2.top    == funFrame.top,
   });
}
   
}; // namespace Game

}; // namespace CubeWorld
