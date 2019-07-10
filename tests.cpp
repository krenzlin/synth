#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "helper.h"


TEST_CASE( "[zero_one_to_minus_plus] transform [0..1] to [-1..+1]") {
    CHECK( zero_one_to_minus_plus(0) == -1 );
    CHECK( zero_one_to_minus_plus(1) == +1 );
    CHECK( zero_one_to_minus_plus(0.5) == 0.0 );
}
