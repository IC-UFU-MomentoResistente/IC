#define CATCH_CONFIG_MAIN
#include "../lib/catch2/catch.hpp"

int factorial (int n)
{
    return (n <= 1) ? 1 : n * factorial (n - 1); 
}

TEST_CASE("Fatoral funcional corretamente", "[fatorial]")
{
    REQUIRE(factorial(0) == 1);
    REQUIRE(factorial(1) == 1);
    REQUIRE(factorial(5) == 120);
}   