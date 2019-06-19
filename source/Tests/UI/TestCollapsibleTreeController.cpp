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

struct TestStringItem {
   std::string title;
   std::vector<TestStringItem> children;
};

typedef std::vector<std::string> StringVector;

SCENARIO( "CollapsibleTreeItems have correct layout and respond to user input correctly" ) {
   
   GIVEN( "a CollapsibleTreeItem" ) {
      std::unique_ptr<UIRoot> dummyRoot = CreateDummyUIRoot();
      UIRoot* root = dummyRoot.get();
      std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();
      std::vector<bool> selections;
      
      CollapsibleTreeItem* item = dummyRoot->Add<CollapsibleTreeItem>("Test Title", "DummyItem");
      
      item->GetSelectionObservable() >>
         ToContainer(selections, myBag);
      
      // solve constraints
      dummyRoot->UpdateRoot();
      
      WHEN( "the item is clicked" ) {
         dummyRoot->UpdateRoot();
         UIElement* hilite = FindChildByName(item, "DummyItemHighlight");
         MockClick(root, hilite);
         
         THEN( "the item is selected" ) {
            CHECK( selections.size() == 1 );
         }
      }
      
      WHEN( "the item's toggle is clicked" ) {
         UIElement* toggleBoy = FindChildByName(item, "DummyItemToggle");
         MockClick(root, toggleBoy);
         
         THEN( "the item is NOT selected, since the toggle consumed the event" ) {
            CHECK( selections.size() == 0 );
         }
      }
      
      WHEN( "the item is provided with some new children" ) {
         StringVector dummyChildren = {"Test 1", "Test 2", "Test 3"};
         item->GetChildDataObservable().SendMessage(&dummyChildren);
         
         UIElement* itemStackView = FindChildByName(item, "DummyItemSubItemParent");
         const auto& subItems = itemStackView->GetChildren();
         
         THEN( "the item adds children with the correct names" ) { 
            for (size_t ndx = 0; ndx < subItems.size(); ndx++) {
               const std::string& childName = subItems[ndx]->GetName();
               Text* childLabel = (Text*) FindChildByName(itemStackView, childName + "." + childName + "Label");
               CHECK( dummyChildren[ndx] == childLabel->GetText() );
            }
         }
         
         AND_WHEN( "the item is collapsed" ) {
            UIElement* toggleBoy = FindChildByName(item, "DummyItemToggle");
            MockClick(root, toggleBoy); // expand
            dummyRoot->UpdateRoot();    // solve constraints
            
            float fullHeight = item->GetHeight();
            float subItemHeight = subItems[0].get()->GetHeight();
            
            MockClick(root, toggleBoy); // unexpand
            dummyRoot->UpdateRoot();    // solve constraints
            
            THEN( "the item's height goes down by the total height of all its children" ) {
               CHECK( fullHeight - subItemHeight * 3 == Approx(item->GetHeight()) );
            }
         }
      }
      
   } // GIVEN
} // SCENARIO

//SCENARIO( "The CollapsibleTreeController reacts appropriately to data changes and user interaction" ) {
//   
//   GIVEN( "A CollapsibleTreeController" ) {
//      std::unique_ptr<CubeWorld::Engine::UIRoot> dummyRoot = CreateDummyUIRoot();
////      CollapsibleTreeVC* collapsible = dummyRoot->Add<CollapsibleTreeVC>("TestCollapsibleTreeController");
//      
//      WHEN( "the controller is provided with a tree of data" ) {
//         TestStringItem blah{
//            "lol",
//            { 
//               {"haha", {}},
//               {"cool", {}},
//               {"neat", {}}
//            }
//         };
//         
//         
//         
//         
//      }
//      
//   } // GIVEN
//} // SCENARIO
