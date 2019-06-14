#include "../catch.h"

#include "Mocks/Mocks_UIElement.h"

#include <Engine/UI/CollapsibleTreeItem.h>

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld;
using namespace CubeWorld::Observables;

using CubeWorld::Engine::UIElement;
using CubeWorld::Engine::UIRoot;
using CubeWorld::Engine::UIGestureRecognizer;
using CubeWorld::Engine::CollapsibleTreeItem;

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
      std::vector<UIGestureRecognizer::Message_GestureState> selections;
      
      CollapsibleTreeItem* item = dummyRoot->Add<CollapsibleTreeItem>("Test Title", "DummyItem");
      
      item->OnSelected() >>
         ToContainer(selections, myBag);
      
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
         
         ObservableInternal<StringVector*> childrenObservable;
         
//         item->ProvideChildrenObservable(childrenObservable.MessageProducer());
         
         THEN( "the item grows to the appropriate size" ) {
            
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
