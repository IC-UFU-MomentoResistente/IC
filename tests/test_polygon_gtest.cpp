#include "gtest/gtest.h"
#include "polygon.h"

TEST(PolygonTest, CreatePolygon)
{
    Polygon p(1, "Retangulo", {{0,0}, {10, 0}, {10, 5}, {0, 5} });

    ASSERT_EQ(p.id, 1);
    ASSERT_EQ(p.name, "Retangulo");
    ASSERT_EQ(p.getNumVertices(), 4);
    ASSERT_TRUE(p.isValid());
}

TEST(PolygonTest, AddVertice)
{
    Polygon p;
    p.id = 2;
    p.name = "Triangulo";
    p.addVertice(0, 0);
    p.addVertice(10, 0);
    p.addVertice(5, 5);

    ASSERT_EQ(p.getNumVertices(), 3);
    ASSERT_TRUE(p.isValid());
}

TEST(PolygonTest, PolygonIsValid)
{
    Polygon p(3, "Linha", {{0, 0}, {10, 0}});
    ASSERT_FALSE(p.isValid());
}