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
      UIRoot* root = dummyRoot.get();
      std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();
      std::vector<Engine::UIGestureRecognizer::Message_GestureState> clicks;
      
      ButtonVC* button = dummyRoot->Add<ButtonVC>("ButtonDummy");
      button->ConstrainHeight(50);
      button->ConstrainWidth(50);
      button->OnClick() >>
         ToContainer(clicks, myBag);
      
      // Solve constraints
      dummyRoot->UpdateRoot();
      
      WHEN( "The button receives a click" ) {
         MockClick(root, button);
         THEN ( "the button should send a message from its OnClick observable." ) {
            CHECK( clicks.size() == 1 );
         }
      }
      
      WHEN( "the button receives a MouseDown, the mouse moves OFF the button, then back on, then MouseUps" ) {
         MockMouseDown(root, button);
         MockMouseMove(root, button, 1000, 1000);
         MockMouseMove(root, button);
         MockMouseUp  (root, button);

         THEN ( "the button should send a message from its OnClick observable." ) {
            CHECK( clicks.size() == 1 );
         }
      }
      
      WHEN( "MouseDown ON the button, drags off, MouseUps, MouseDown and drags back ON, MouseUp" ) {
         MockMouseDown(root, button);
         MockMouseMove(root, button, 1000, 1000);
         MockMouseUp  (root, button, 1000, 1000);
         MockMouseDown(root, root, 1000, 1000);
         MockMouseMove(root, button);
         MockMouseUp  (root, button);
         
         THEN( "the button should NOT send a message from its OnClick observable." ) {
            CHECK( clicks.size() == 0 );
         }
      }

   } // GIVEN
} // SCENARIO
