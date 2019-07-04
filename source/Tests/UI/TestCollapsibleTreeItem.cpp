#include "../catch.h"

#include "Mocks/Mocks_UIElement.h"

#include <Engine/UI/CollapsibleTreeItem.h>
#include <Shared/UI/Text.h>

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld;
using namespace CubeWorld::Observables;

using CubeWorld::Engine::UIElement;
using CubeWorld::Engine::UIRoot;
using CubeWorld::Engine::UIGestureRecognizer;
using CubeWorld::Engine::CollapsibleTreeItem;

using CubeWorld::UI::Text;

typedef std::vector<std::string> StringVector;

SCENARIO( "CollapsibleTreeItems have correct layout and respond to user input correctly" ) {
   
GIVEN( "a CollapsibleTreeItem" ) {
   std::unique_ptr<UIRoot> dummyRoot = CreateDummyUIRoot();
   UIRoot* root = dummyRoot.get();
   std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();

   CollapsibleTreeItem* item = dummyRoot->Add<CollapsibleTreeItem>("DummyItem");

   // solve constraints
   dummyRoot->UpdateRoot();

   WHEN( "the item is clicked" ) {
      dummyRoot->UpdateRoot();
      UIElement* hilite = FindChildByName(item, "DummyItemHighlight");
      MockClick(root, hilite);

      THEN( "the item is highlighted and selected" ) {
         CHECK( hilite->IsActive() );
      }
   }
   
   WHEN( "the item has no subItems" ) {
      THEN( "the item's ToggleButton has active == false" ) {
         UIElement* toggleBoy = FindChildByName(item, "DummyItemToggle");
         CHECK( !toggleBoy->IsActive() );
      }
   }

   WHEN( "the item is provided with some new subItems" ) {
      CollapsibleTreeItem::Data dummyData = {"Test Title", {{"Test 1"}, {"Test 2"}, {"Test 3"}}};
      item->GetDataSink().SendMessage(dummyData);

      UIElement* itemStackView = FindChildByName(item, "DummyItemSubItemParent");
      const auto& subItems = itemStackView->GetChildren();
      
      THEN( "the item sets its label to the given title" ) {
         Text* label = (Text*) FindChildByName(item, "DummyItemLabel");
         CHECK( label->GetText() == "Test Title" );
      }
      
      THEN( "the item adds children with the correct labels" ) {
         for (size_t ndx = 0; ndx < subItems.size(); ndx++) {
            const std::string& childName = subItems[ndx]->GetName();
            Text* childLabel = (Text*) FindChildByName(itemStackView, childName + "." + childName + "Label");
            CHECK( dummyData.children[ndx].title == childLabel->GetText() );
         }
      }
      
      AND_THEN( "the item's ToggleButton has active == true" ) {
         UIElement* toggleBoy = FindChildByName(item, "DummyItemToggle");
         CHECK( toggleBoy->IsActive() );
      }
      
      AND_WHEN( "the item's toggle is clicked" ) {
         UIElement* toggleBoy = FindChildByName(item, "DummyItemToggle");
         MockClick(root, toggleBoy);
         
         UIElement* hilite = FindChildByName(item, "DummyItemHighlight");
         
         THEN( "the item is NOT selected, since the toggle consumed the event" ) {
            CHECK( !hilite->IsActive() );
         }
      }

      AND_WHEN( "the item is collapsed" ) {
         UIElement* toggleBoy = FindChildByName(item, "DummyItemToggle");
         MockClick(root, toggleBoy); // expand
         dummyRoot->UpdateRoot();    // solve constraints

         uint32_t fullHeight = item->GetHeight();
         uint32_t subItemHeight = subItems[0].get()->GetHeight();

         MockClick(root, toggleBoy); // unexpand
         dummyRoot->UpdateRoot();    // solve constraints

         THEN( "the item's height goes down by the total height of all its subItems" ) {
            CHECK( fullHeight - subItemHeight * 3 == Approx(item->GetHeight()) );
         }
      }
   }

} // GIVEN
} // SCENARIO
