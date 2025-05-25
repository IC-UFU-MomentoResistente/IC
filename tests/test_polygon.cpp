#include "catch.hpp"  
#include "polygon.h" 

TEST_CASE("Should create a polygon", "[polygon]")
{
    Polygon p(1, "Triangle", { {0,0}, {10, 0}, {5,5} });

    REQUIRE(p.id == 1);
    REQUIRE(p.name == "Triangle");
    REQUIRE(p.getNumVertices() == 3);
    REQUIRE(p.isValid() == true);
}

TEST_CASE("Should add vertice to polygon", "[polygon]")
{
    Polygon p;
    p.id = 2;
    p.name = "Triangle";
    p.addVertice(0,0);
    p.addVertice(10,0);
    p.addVertice(5,5);

    REQUIRE(p.getNumVertices() == 3);
    REQUIRE(p.isValid() == true);
}

TEST_CASE("Should not be a valid polygon", "[polygon]")
{
    Polygon p(3, "Line", { {0,0}, {10, 0} });

    REQUIRE_FALSE(p.isValid());
}