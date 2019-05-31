#include "../catch.h"

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld::Observables;

TEST_CASE( "Simple Observable Map" ) {
   std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();
   ObservableInternal<int> testObservable;
   
   std::vector<int> results;
   
   testObservable.OnChanged() >>
      Map<int, int>([](int test) -> int {
         return test + 2;
      }) >> 
      ToContainer(results, myBag);
   
   testObservable.SendMessage(3);
   testObservable.SendMessage(4);
   testObservable.SendMessage(5);
   testObservable.SendMessage(-1);
   
   std::vector<int> expected{ 5, 6, 7, 1 };
   
   CHECK( results == expected );
}
