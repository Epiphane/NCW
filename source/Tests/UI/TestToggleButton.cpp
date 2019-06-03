#include "../catch.h"

#include "Mocks/Mocks_UIElement.h"

#include <Engine/UI/ToggleButtonVC.h>

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld;
using namespace CubeWorld::Observables;

using CubeWorld::Engine::ToggleButtonVC;
using UI::Image;
using Engine::UIRoot;

SCENARIO( "Toggle buttons send the correct messages when clicked or force-toggled" ) {
   
   GIVEN( "A toggle button with Observers attached" ) {
      std::unique_ptr<UIRoot> dummyRoot = CreateDummyUIRoot();
      std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();
      
      ToggleButtonVC* button = new ToggleButtonVC(dummyRoot.get(), dummyRoot.get(), Image::Options(), Image::Options(), "ToggleButtonDummy");
      
      std::vector<bool> clickToggles;
      std::vector<bool> allToggles;
      
      button->OnToggled() >>
         ToContainer(clickToggles, myBag);
      button->OnToggleStateSet() >>
         ToContainer(allToggles, myBag);
      
      WHEN( "The toggle button receives mouse events" ) {
         THEN ( "the toggle button should send messages from its OnToggled observable." ) {
            MockClick(button);
            
            // ToggleButton defaults to false, so toggling once should give us one true
            std::vector<bool> expected = { true };
            CHECK( expected == clickToggles );
            
            MockClick(button);
            MockClick(button);
            MockClick(button);
            
            expected = { true, false, true, false };
            CHECK( expected == clickToggles );
            CHECK( expected == allToggles );
         }
      }
      
      AND_WHEN( "the toggle button is set programmatically" ) {
         ObservableInternal<bool> toggleMeister;
         button->ProvideToggleSetter(toggleMeister.OnChanged());
         
         toggleMeister.SendMessage(false);
         toggleMeister.SendMessage(false);
         toggleMeister.SendMessage(true);
         
         THEN( "only the 'OnToggleStateSet' Observable should send messages." ) {            
            std::vector<bool> expectedClickToggles = { };
            std::vector<bool> expectedAllToggles   = { false, false, true };
            
            CHECK( expectedClickToggles == clickToggles );
            CHECK( expectedAllToggles == allToggles );
         }
      }
      
      AND_WHEN( "programmatic setting is combined with mouse events" ) {
         ObservableInternal<bool> toggleMeister;
         button->ProvideToggleSetter(toggleMeister.OnChanged());
         
         toggleMeister.SendMessage(false);
         MockClick(button);
         toggleMeister.SendMessage(false);
         toggleMeister.SendMessage(true);
         MockClick(button);
         MockClick(button);
         MockClick(button);
         toggleMeister.SendMessage(false);
         toggleMeister.SendMessage(false);
         
         THEN("allToggles will receive all messages while clickToggles will only receive click-based messages." ) {
            std::vector<bool> expectedClickToggles = { true, false, true, false };
            std::vector<bool> expectedAllToggles   = { false, true, false, true, false, true, false, false, false };
            
            CHECK( expectedClickToggles == clickToggles );
            CHECK( expectedAllToggles == allToggles );
         }
      }
   }
}
