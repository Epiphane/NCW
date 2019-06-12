#include "../catch.h"

#include "Mocks/Mocks_UIElement.h"

#include <Editor/Constrainer/CollapsibleTreeVC.h>
#include <Editor/Constrainer/CollapsibleTreeItem.h>

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld::Observables;
using namespace CubeWorld;
using Engine::UIRoot;
using Engine::UIElement;

using Editor::Constrainer::CollapsibleTreeVC;
using Editor::Constrainer::CollapsibleTreeItem;

struct TestStringItem {
   std::string title;
   std::vector<TestStringItem> children;
};

SCENARIO( "CollapsibleTreeItems have correct layout and respond to user input correctly" ) {
   
   GIVEN( "a CollapsibleTreeItem" ) {
      std::unique_ptr<UIRoot> dummyRoot = CreateDummyUIRoot();
      std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();
      std::vector<Engine::UIGestureRecognizer::Message_GestureState> selections;
      
      CollapsibleTreeItem* item = dummyRoot->Add<CollapsibleTreeItem>("Title", "DummyItem");
      
      item->OnSelected() >>
         ToContainer(selections, myBag);
      
      WHEN( "the item's label is clicked, the item is selected" ) {
         
      }
      
   } // GIVEN
} // SCENARIO

SCENARIO( "The CollapsibleTreeController reacts appropriately to data changes and user interaction" ) {
   
   GIVEN( "A CollapsibleTreeController" ) {
      std::unique_ptr<UIRoot> dummyRoot = CreateDummyUIRoot();
      CollapsibleTreeVC* collapsible = dummyRoot->Add<CollapsibleTreeVC>("TestCollapsibleTreeController");
      
      WHEN( "the controller is provided with a tree of data" ) {
         TestStringItem blah{
            "lol",
            { 
               {"haha", {}},
               {"cool", {}},
               {"neat", {}}
            }
         };
         
         
         
         
      }
      
   } // GIVEN
} // SCENARIO
