#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "createPolygonUseCase.h"
#include "interfacePolygonRepository.h"
#include "polygonDto.h"

using ::testing::_;
using ::testing::Return;
using ::testing::Throw;

class MockPolygonRepository : public InterfacePolygonRepository
{
public:
    MOCK_METHOD(void, save, (Polygon& polygon), (override));
    MOCK_METHOD(std::optional<Polygon>, loadByName, (const std::string& name), (override));
    MOCK_METHOD(std::optional<Polygon>, loadById, (uint64_t id), (override));
    MOCK_METHOD(std::vector<std::string>, listSavedSectionNames, (), (override));
};

TEST(CreatePolygonUseCaseTest, CreatePolygon_Success)
{
    MockPolygonRepository mockRepo;
    CreatePolygonUseCase useCase(mockRepo);

    PolygonDto polygonDto;
    polygonDto.name = "TestePoligono";
    polygonDto.vertices = {{0, 0}, {10, 0}, {5, 5}};

    EXPECT_CALL(mockRepo, save(testing::Truly([](const Polygon& p) 
    { return p.name == "TestePoligono" && p.vertices.size() == 3; }))).Times(1);

    Polygon createdPolygon = useCase.execute(polygonDto);

    ASSERT_EQ(createdPolygon.name, "TestePoligono");
    ASSERT_EQ(createdPolygon.vertices.size(), 3);
    ASSERT_TRUE(createdPolygon.id != 0);
}

TEST(CreatePolygonUseCaseTest, NameEmpty_ThrowsException)
{
    MockPolygonRepository mockRepo;
    CreatePolygonUseCase useCase(mockRepo);

    PolygonDto polygonDto;
    polygonDto.name = "";
    polygonDto.vertices = {{0, 0}, {10, 0}, {5, 5}};

    ASSERT_THROW(useCase.execute(polygonDto), std::invalid_argument);
}

TEST(CreatePolygonUseCaseTest, Under3Vertices_ThrowsException)
{
    MockPolygonRepository mockRepo;
    CreatePolygonUseCase useCase(mockRepo);

    PolygonDto polygonDto;
    polygonDto.name = "Invalida";
    polygonDto.vertices = {{0, 0}, {10, 0}};

    ASSERT_THROW(useCase.execute(polygonDto), std::invalid_argument);
}