#include <iterator>
#include <sstream>

#include <bys/find.hpp>
#include <bys/test/test.hpp>

namespace
{
struct Needle
{
    int value;
};

struct Element
{
    int value;

    bool operator==(Needle const& needle) const
    {
        return value == needle.value;
    }
};

struct CountedElement
{
    int value;
    int* comparisons;

    bool operator==(int needle) const
    {
        ++*comparisons;
        return value == needle;
    }
};
} // namespace

STATIC_CHECK([] {
    int items[]{1, 2, 3};
    return bys::find(items, items + 3, 2) == items + 1;
}(),
    "bys::find must work in a constant expression");

TEST_CASE("find.empty")
{
    int items[]{1};
    CHECK(bys::find(items, items, 1) == items);
}

TEST_CASE("find.missing")
{
    int items[]{1, 3, 5};
    CHECK(bys::find(items, items + 3, 9) == items + 3);
}

TEST_CASE("find.first_match")
{
    int items[]{1, 7, 7};
    CHECK(bys::find(items, items + 3, 7) == items + 1);
}

TEST_CASE("find.respects_end")
{
    int items[]{1, 2, 3};
    CHECK(bys::find(items, items + 2, 3) == items + 2);
}

TEST_CASE("find.const_elements")
{
    int const items[]{1, 2};
    CHECK(bys::find(items, items + 2, 2) == items + 1);
}

TEST_CASE("find.single_pass")
{
    std::istringstream input("4 8 15");
    using Iterator = std::istream_iterator<int>;
    auto const match = bys::find(Iterator{input}, Iterator{}, 8);
    REQUIRE(match != Iterator{});
    CHECK_EQ(*match, 8);
}

TEST_CASE("find.different_value_type")
{
    Element items[]{{1}, {2}, {3}};
    CHECK(bys::find(items, items + 3, Needle{2}) == items + 1);
}

TEST_CASE("find.comparison_limit")
{
    int comparisons = 0;
    CountedElement items[]{{1, &comparisons}, {2, &comparisons}, {3, &comparisons}};

    CHECK(bys::find(items, items + 3, 9) == items + 3);
    CHECK(comparisons <= 3);
}
