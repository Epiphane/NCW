#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.h"

#include <RGBBinding/Observable.h>
#include <RGBBinding/ObservableBasicOperations.h>

using namespace CubeWorld;

unsigned int Factorial( unsigned int number ) {
   return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
   REQUIRE( Factorial(1) == 1 );
   REQUIRE( Factorial(2) == 2 );
   REQUIRE( Factorial(3) == 6 );
   REQUIRE( Factorial(10) == 3628800 );
   
   std::shared_ptr<DisposeBag> myBag = std::make_shared<DisposeBag>();
   ObservableInternal<int> testObservable;
   
   testObservable.OnChanged() >>
   Map<int, int>([](int test) -> int {
      return test + 2;
   }) >> 
   OnMessage<int>([](int test) {
      std::cout << "OH yeAH BABY" << test << std::endl;
      REQUIRE( true );
   }, myBag);
   
   testObservable.SendMessage(3);
   testObservable.SendMessage(4);
   testObservable.SendMessage(5);
   testObservable.SendMessage(-1);
}
