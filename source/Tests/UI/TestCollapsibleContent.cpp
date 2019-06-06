#include "../catch.h"

#include "Mocks/Mocks_UIElement.h"

#include <Engine/UI/CollapsibleContentVC.h>

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld::Observables;
using namespace CubeWorld;
using Engine::UIRoot;
using Engine::UIElement;

using Engine::CollapsibleContentVC;

SCENARIO( "CollapsibleContent should hide its content view when its toggle button is toggled" ) {
   
   GIVEN( "A CollapsibleContent element" ) {
      std::unique_ptr<UIRoot> dummyRoot = CreateDummyUIRoot();
      
      CollapsibleContentVC collapsible(dummyRoot.get(), dummyRoot.get(), "TestCollapsibleContent");
      uint32_t normalHeight = collapsible.GetHeight();
      
      WHEN( "the element gets children, its height expands appropriately" ) {
         UIElement* dummyContent = collapsible.Add<UIElement>("DummyContent");
         dummyContent->ConstrainHeight(50);
         
         CHECK( collapsible.GetHeight() == normalHeight + 50 );
      }
   }
//      Engine::Window::Options windowOptions;
//      windowOptions.fullscreen = false;
//      windowOptions.width = 1000;
//      windowOptions.height = 1000;
//
//      Engine::Window& dummyWindow = Engine::Window::Instance();
//      dummyWindow.Initialize(windowOptions);
//      std::unique_ptr<UIRoot> dummyUIRoot = std::make_unique<UIRoot>(&dummyWindow);
//      
//      std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();
//
//      WHEN( "The CollapsibleContent is toggled" ) {
//         CollapsibleContentVC collapser(dummyUIRoot.get(), dummyUIRoot.get(), "CollapsibleContentTest");
//         std::vector<bool> clickToggles;
//         std::vector<bool> forcedToggles;
//         
//         button->OnUserToggled() >>
//            ToContainer(clickToggles, myBag);
//         button->OnToggleStateSet() >>
//            ToContainer(forcedToggles, myBag);
//         
//         THEN ( "the content should be set to inactive" ) {
//            MockClick(button);
//            
//            // ToggleButton defaults to false, so toggling once should give us one true
//            std::vector<bool> expected = { true };
//            CHECK( expected == clickToggles );
//            
//            MockClick(button);
//            MockClick(button);
//            MockClick(button);
//            
//            expected = { true, false, true, false };
//            CHECK( expected == clickToggles );
//            CHECK( forcedToggles.empty() );
//         }
//      }
//      
//      WHEN( "The toggle button is forcibly changed to another value" ) {
//         ToggleButtonVC* button = new ToggleButtonVC(dummyUIRoot.get(), dummyUIRoot.get(), Image::Options(), Image::Options(), "ToggleButtonDummy");
//         std::vector<bool> clickToggles;
//         std::vector<bool> forcedToggles;
//         
//         button->OnUserToggled() >>
//            ToContainer(clickToggles, myBag);
//         button->OnToggleStateSet() >>
//            ToContainer(forcedToggles, myBag);
//         
//         ObservableInternal<bool> toggleMeister;
//         
//         THEN ( "the toggle button should send messages only from its OnToggleForced observable." ) {
//            button->ProvideToggleSetter(toggleMeister.MessageProducer());
//            
//            toggleMeister.SendMessage(false);
//            toggleMeister.SendMessage(false);
//            toggleMeister.SendMessage(true);
//            
//            std::vector<bool> expected = { false, false, true };
//            CHECK( expected == forcedToggles );
//            CHECK( clickToggles.empty() );
//         }
//      }
//   }
}
