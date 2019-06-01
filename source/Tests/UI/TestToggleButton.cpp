#include "../catch.h"

#include "Mocks/Mocks_UIElement.h"

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
      
      WHEN( "The toggle button receives mouse events" ) {
         ToggleButtonVC* button = new ToggleButtonVC(dummyUIRoot.get(), dummyUIRoot.get(), Image::Options(), Image::Options(), "ToggleButtonDummy");
         std::vector<bool> clickToggles;
         std::vector<bool> forcedToggles;
         
         button->OnUserToggled() >>
            ToContainer(clickToggles, myBag);
         button->OnToggleForciblyChanged() >>
            ToContainer(forcedToggles, myBag);
         
         THEN ( "the toggle button should send messages only from its OnUserToggled observable." ) {
            MockClick(button);
            
            // ToggleButton defaults to false, so toggling once should give us one true
            std::vector<bool> expected = { true };
            CHECK( expected == clickToggles );
            
            MockClick(button);
            MockClick(button);
            MockClick(button);
            
            expected = { true, false, true, false };
            CHECK( expected == clickToggles );
            CHECK( forcedToggles.empty() );
         }
      }
      
      WHEN( "The toggle button is forcibly changed to another value" ) {
         ToggleButtonVC* button = new ToggleButtonVC(dummyUIRoot.get(), dummyUIRoot.get(), Image::Options(), Image::Options(), "ToggleButtonDummy");
         std::vector<bool> clickToggles;
         std::vector<bool> forcedToggles;
         
         button->OnUserToggled() >>
            ToContainer(clickToggles, myBag);
         button->OnToggleForciblyChanged() >>
            ToContainer(forcedToggles, myBag);
         
         ObservableInternal<bool> toggleMeister;
         
         THEN ( "the toggle button should send messages only from its OnToggleForced observable." ) {
            button->ProvideToggleForcer(toggleMeister.OnChanged());
            
            toggleMeister.SendMessage(false);
            toggleMeister.SendMessage(false);
            toggleMeister.SendMessage(true);
            
            std::vector<bool> expected = { false, false, true };
            CHECK( expected == forcedToggles );
            CHECK( clickToggles.empty() );
         }
      }
   }
}
