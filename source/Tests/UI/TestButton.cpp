#include "../catch.h"

#include "Mocks/Mocks_UIElement.h"

#include <Engine/UI/ButtonVC.h>
#include <Engine/UI/UIGestureRecognizer.h>

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld;
using namespace Observables;

using Engine::ButtonVC;
using Engine::UIRoot;

SCENARIO( "Buttons respond correctly to mouse moves and clicks" ) {
   
   GIVEN( "A button with an Observable attached to it" ) {
      std::unique_ptr<UIRoot> dummyRoot = CreateDummyUIRoot();
      std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();
      std::vector<Engine::UIGestureRecognizer::Message_GestureState> clicks;
      
      ButtonVC* button = dummyRoot->Add<ButtonVC>("ButtonDummy");
      button->ConstrainHeight(50);
      button->ConstrainWidth(50);
      button->OnClick() >>
         ToContainer(clicks, myBag);
      
      // Solve constraints
      dummyRoot->UpdateRoot();
      
      auto button_center = (button->GetFrame().GetTopRight() + button->GetFrame().GetBottomLeft()) / 2.0f;
      
      WHEN( "The button receives a click" ) {
         MockClick(button);
         THEN ( "the button should send a message from its OnClick observable." ) {
            CHECK( clicks.size() == 1 );
         }
      }
      
      WHEN( "the button receives a MouseDown, the mouse moves OFF the button, then back on, then MouseUps" ) {
         MockMouseDown(button, button_center.x, button_center.y);
         MockMouseMove(button, 1000, 1000);
         MockMouseMove(button, button_center.x, button_center.y);
         MockMouseUp  (button, button_center.x, button_center.y);

         THEN ( "the button should send a message from its OnClick observable." ) {
            CHECK( clicks.size() == 1 );
         }
      }
      
      WHEN( "MouseDown ON the button, drags off, MouseUps, MouseDown and drags back ON, MouseUp" ) {
         MockMouseDown(button, button_center.x, button_center.y);
         MockMouseMove(button, 1000, 1000);
         MockMouseUp  (button, 1000, 1000);
         MockMouseDown(dummyRoot.get(), 1000, 1000);
         MockMouseMove(button, button_center.x, button_center.y);
         MockMouseUp  (button, button_center.x, button_center.y);
         
         THEN( "the button should NOT send a message from its OnClick observable." ) {
            CHECK( clicks.size() == 0 );
         }
      }

   } // GIVEN
} // SCENARIO
