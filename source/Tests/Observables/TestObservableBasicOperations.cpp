#include "../catch.h"

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld::Observables;

SCENARIO( "Basic Observable operations should properly manipulate messages streams" ) {

   GIVEN( "A DisposeBag" ) {
      std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();

      WHEN( "A Map is attached to an Observable emitting messages" ) {
         ObservableInternal<int> testObservable;
         std::vector<int> results;

         testObservable.MessageProducer() >>
            Map<int, int>([](int test) -> int {
               return test + 2;
            }) >>
            ToContainer(results, myBag);

         testObservable.SendMessage(3);
         testObservable.SendMessage(4);
         testObservable.SendMessage(5);
         testObservable.SendMessage(-1);

         THEN( "the results should be the original messages but ran through the mapping function" ) {
            std::vector<int> expected{ 5, 6, 7, 1 };
            CHECK( results == expected );
         }
      }

      WHEN( "A Filter for only even numbers is attached to an Observable emitting messages" ) {
         ObservableInternal<int> testObservable;
         std::vector<int> results;

         testObservable.MessageProducer() >>
            Filter<int>([](int test) -> bool {
               return test % 2 == 0;
            }) >>
            ToContainer(results, myBag);

         testObservable.SendMessage(3);
         testObservable.SendMessage(4);
         testObservable.SendMessage(5);
         testObservable.SendMessage(6);
         testObservable.SendMessage(0);
         testObservable.SendMessage(-1);

         THEN( "the results should only contain even numbers" ) {
            std::vector<int> expected{ 4, 6, 0 };
            CHECK( results == expected );
         }
      }

      WHEN( "Two Observables are combined into a new one with Merge" ) {
         ObservableInternal<int> obsA;
         ObservableInternal<int> obsB;
         std::vector<int> results;

         Merge(obsA.MessageProducer(), obsB.MessageProducer()) >>
            ToContainer(results, myBag);

         obsA.SendMessage(1);

         obsB.SendMessage(5);
         obsB.SendMessage(5);
         obsB.SendMessage(10);

         obsA.SendMessage(-1);
         obsA.SendMessage(700);

         THEN( "the results should be the combination of both Observables' messages" ) {
            std::vector<int> expected{ 1, 5, 5, 10, -1, 700 };
            CHECK( results == expected );
         }
      }
      
      WHEN( "An Observable sends its messages through RemoveDuplicates()" ) {
         ObservableInternal<int> testObservable;
         std::vector<bool> results;
         
         testObservable.MessageProducer() >>
            RemoveDuplicates() >>
            ToContainer(results, myBag);
         
         testObservable.SendMessage(true);
         testObservable.SendMessage(true);
         testObservable.SendMessage(false);
         testObservable.SendMessage(true);
         testObservable.SendMessage(false);
         testObservable.SendMessage(false);
         testObservable.SendMessage(false);
         testObservable.SendMessage(true);
         
         THEN( "only non-duplicate values should come through" ) {
            std::vector<bool> expected{ true, false, true, false, true };
            CHECK( results == expected );
         }
      }
      
      WHEN( "An Observable is modified by StartWith()" ) {
         ObservableInternal<int> testObservable;
         std::vector<int> results;
         
         testObservable.MessageProducer() >>
            StartWith(11) >>
            ToContainer(results, myBag);
         
         THEN( "it should immediately have the passed-in message at the beginning of its output message stream") {
            std::vector<int> expected { 11 };
            CHECK( results == expected );
            
            AND_THEN( "it should continue passing the rest of its messages through normally" ) {
               testObservable.SendMessage(37);
               testObservable.SendMessage(-1);
               testObservable.SendMessage(22);
               
               std::vector<int> expected { 11, 37, -1, 22 };
               CHECK( results == expected );
            }
         }
      }

   } // GIVEN
} // SCENARIO
