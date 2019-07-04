#include "../catch.h"

#include "Mocks/Mocks_UIElement.h"

#include <Engine/UI/CollapsibleTreeVC.h>

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld;
using namespace CubeWorld::Observables;

using CubeWorld::Engine::UI::CollapsibleTreeVC; 
using TreeData = CubeWorld::Engine::CollapsibleTreeItem::Data;

SCENARIO( "The CollapsibleTreeController reacts appropriately to data changes" ) {
   
GIVEN( "A CollapsibleTreeController" ) {
      std::unique_ptr<CubeWorld::Engine::UIRoot> dummyRoot = CreateDummyUIRoot();
      CollapsibleTreeVC* collapsible = dummyRoot->Add<CollapsibleTreeVC>("TestCollapsibleTreeController");
      
      WHEN( "the controller is provided with a tree of data" ) {
         TreeData topElement1{
            "topElement1",
            { 
               {"coolItem1"}, {"coolItem2"}, {"coolItem3"}
            }
         };
         TreeData topElement2{
            "topElement2",
            { 
               {"level1", {{"level2", {{"level3", {{"level4"}}}}}}}
            }
         };
         
         std::vector<TreeData> items = { topElement1, topElement2 };
         collapsible->GetDataSink().SendMessage(items);
         
         THEN( "the controller should add items with the provided titles" ) {
            std::string title1 = collapsible->GetRootItems()[0]->GetSubItems()[0]->GetTitle();
            CHECK( title1 == "coolItem1" );
            
            std::string title3 = collapsible->GetRootItems()[0]->GetSubItems()[2]->GetTitle();
            CHECK( title3 == "coolItem3" );
            
            std::string deepTitle = collapsible->GetRootItems()[1]->GetSubItems()[0]->GetSubItems()[0]->GetSubItems()[0]->GetSubItems()[0]->GetTitle();
            CHECK( deepTitle == "level4" );
            
            AND_WHEN( "the controller is provided with new data" ) {
               collapsible->GetDataSink().SendMessage({ {"newFella"} });
               
               THEN( "the old data is removed, and the new data replaces it" ) {
                  dummyRoot->UpdateRoot(); // delete old UIElements
                  
                  CHECK( collapsible->GetRootItems().size() == 1 );
                  CHECK( collapsible->GetRootItems()[0]->GetSubItems().size() == 0 );
               }
            }
         }
      }
   
} // GIVEN
} // SCENARIO
