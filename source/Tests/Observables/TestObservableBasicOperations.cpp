#include "../catch.h"

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld::Observables;

SCENARIO( "Basic Observable operations should properly manipulate messages streams" ) {

   GIVEN( "A DisposeBag" ) {
      std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();

      WHEN( "A Map is attached to an Observable emitting messages" ) {
         Observable<int> testObservable;
         std::vector<int> results;

         testObservable >>
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
         Observable<int> testObservable;
         std::vector<int> results;

         testObservable >>
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
         Observable<int> obsA;
         Observable<int> obsB;
         std::vector<int> results;

         Merge(obsA, obsB) >>
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
      
      WHEN( "Two Observables are combined with CombineLatest()" ) {
         Observable<int> obsA;
         Observable<bool> obsB;
         std::vector<std::tuple<int, bool>> results;
         
         CombineLatest(obsA, obsB) >>
            ToContainer(results, myBag);
         
         THEN( "CombineLatest does not emit until both Observables emit a message" ) {
            obsA.SendMessage(5);
            
            CHECK( results.empty() );
         }
         
         AND_THEN( "CombineLatest returns the most recent two messages sent from either Observable " ) {
            obsA.SendMessage(3);
            obsB.SendMessage(false);
            obsB.SendMessage(true);
            obsA.SendMessage(-10);
            
            std::vector<std::tuple<int, bool>> expected = { {3, false}, {3, true}, {-10, true} };
            CHECK( results == expected );
         }
      }
      
      WHEN( "An Observable sends its messages through RemoveDuplicates()" ) {
         Observable<int> testObservable;
         std::vector<bool> results;
         
         testObservable >>
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
         Observable<int> testObservable;
         std::vector<int> results;
         
         testObservable >>
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
      
      WHEN( "An Observable gets messages piped into it" ) {
         Observable<int> testObservableAccepter;
         Observable<int> testObservableProducer;
         std::vector<int> results; 
         
         testObservableProducer >> testObservableAccepter;
         
         testObservableAccepter >>
            ToContainer(results, myBag);
         
         testObservableProducer.SendMessage(3);
         testObservableProducer.SendMessage(100);
         testObservableProducer.SendMessage(-37);
         testObservableProducer.SendMessage(11);
         
         THEN ( "All messages sent by the producer are forwarded through the accepter" ) {
            std::vector<int> expected { 3, 100, -37, 11 };
            CHECK( results == expected );
         }
      }

   } // GIVEN
} // SCENARIO
