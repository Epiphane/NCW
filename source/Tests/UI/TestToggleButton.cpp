#include "../catch.h"

#include <Engine/Core/Window.h>

#include <Engine/UI/UIRoot.h>

#include <Engine/UI/ToggleButtonVC.h>

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld::Observables;
using CubeWorld::Engine::ToggleButtonVC;
using namespace CubeWorld;
using UI::Image;
using Engine::UIRoot;

SCENARIO( "Toggle buttons send the correct messages when clicked or force-toggled" ) {
   
   GIVEN( "A dummy window and UIRoot" ) {
      Engine::Window::Options windowOptions;
      Engine::Window& dumb = Engine::Window::Instance();
      dumb.Initialize(windowOptions);
      std::unique_ptr<UIRoot> dummyUIRoot = std::make_unique<UIRoot>(&dumb);
      
      std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();
      std::vector<bool> toggles;
      
      WHEN( "The toggle button receives mouse events" ) {
         ToggleButtonVC* button = new ToggleButtonVC(dummyUIRoot.get(), dummyUIRoot.get(), Image::Options(), Image::Options(), "ToggleButtonDummy");
         
         button->OnUserToggled() >>
            ToContainer(toggles, myBag);
         
         THEN ( "the toggle button should send messages only from its OnUserToggled observable." ) {
            MouseDownEvent down(0, 11, 37);
            MouseUpEvent up(0, 11, 37);
            button->MouseDown(down);
            button->MouseUp(up);
            
            // ToggleButton defaults to false, so toggling once should give us one true
            std::vector<bool> expected = { true };
            CHECK( expected == toggles );
            
            button->MouseDown(down);
            button->MouseUp(up);
            button->MouseDown(down);
            button->MouseUp(up);
            button->MouseDown(down);
            button->MouseUp(up);
            
            expected = { true, false, true, false };
            CHECK( expected == toggles );
         }
      }
      
      WHEN( "The toggle button is forcibly changed to another value" ) {
         
      }
   }
}

//TEST_CASE( "Setting the toggle button's toggle programmatically" ) {
//   ToggleButtonVC* button = new ToggleButtonVC(dummyUIRoot.get(), dummyUIRoot.get(), Image::Options(), Image::Options(), "ToggleButtonDummy");
//   std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();
//   ObservableInternal<bool> toggleMeister;
//
//   std::vector<bool> toggles;
//
//   button->ProvideToggler(toggleMeister.OnChanged());
//
//   button->OnToggleValueChanged() >>
//      ToContainer(toggles, myBag);
//
//   toggleMeister.SendMessage(false);
//   toggleMeister.SendMessage(false);
//   toggleMeister.SendMessage(true);
//
//   std::vector<bool> expected = { false, false, true };
//   CHECK( expected == toggles );
//}

