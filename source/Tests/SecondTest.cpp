#include "catch.h"


unsigned int Factorial2( unsigned int number ) {
   return number <= 1 ? number : Factorial2(number-1)*number;
}

TEST_CASE( "Factorials are computed lol", "[factorials yo]" ) {
   REQUIRE( Factorial2(1) == 1 );
   REQUIRE( Factorial2(2) == 3 );
   REQUIRE( Factorial2(3) == 6 );
   REQUIRE( Factorial2(10) == 3628800 );
}
