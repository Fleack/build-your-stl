#include <bys/add.hpp>
#include <bys/test/test.hpp>

TEST_CASE("add.sum")
{
    CHECK_EQ(bys::add(2, 3), 5);
    CHECK_EQ(bys::add(7, 0), 7);
    CHECK_EQ(bys::add(3, -5), -2);
}
