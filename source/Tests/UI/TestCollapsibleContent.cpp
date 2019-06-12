#include "../catch.h"

#include "Mocks/Mocks_UIElement.h"

#include <Engine/UI/CollapsibleContent.h>

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld::Observables;
using namespace CubeWorld;
using Engine::UIRoot;
using Engine::UIElement;

using Engine::CollapsibleContent;

SCENARIO( "CollapsibleContent should hide its content view when its toggle button is toggled" ) {
   
   GIVEN( "A CollapsibleContent element" ) {
      std::unique_ptr<UIRoot> dummyRoot = CreateDummyUIRoot();
      CollapsibleContent* collapsible = dummyRoot->Add<CollapsibleContent>("TestCollapsibleContent");
      
      WHEN( "the element gets children, its height expands appropriately" ) {
         std::unique_ptr<UIElement> dummyContent = std::make_unique<UIElement>(dummyRoot.get(), dummyRoot.get(), "DummyContent");
         dummyContent->ConstrainHeight(50);
         dummyRoot->UpdateRoot();   // Update constraints
         uint32_t normalHeight = collapsible->GetHeight();
         
         collapsible->SetContent(std::move(dummyContent));
         
         dummyRoot->UpdateRoot();   // Update constraints
         CHECK( collapsible->GetHeight() == normalHeight + 50 );
      }
      
   } // GIVEN
} // SCENARIO
