#include "../catch.h"

#include "Mocks/Mocks_UIElement.h"

#include <Engine/UI/ButtonVC.h>
#include <Engine/UI/UIGestureRecognizer.h>

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld;
using namespace Observables;

using Engine::ButtonVC;

SCENARIO( "Buttons respond correctly to mouse moves and clicks", GL_TEST_FLAG) {

   GIVEN( "A button with an Observable attached to it" ) {
      Test::MockInput input;
      std::unique_ptr<Engine::UIRootDep> dummyRoot = CreateDummyUIRoot(input);
      Engine::UIRootDep* root = dummyRoot.get();
      std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();
      std::vector<Engine::UIGestureRecognizer::Message_GestureState> clicks;

      ButtonVC* button = dummyRoot->Add<ButtonVC>("ButtonDummy");
      button->OnClick() >>
         ToContainer<std::vector<Engine::UIGestureRecognizer::Message_GestureState>>(clicks, myBag);

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
