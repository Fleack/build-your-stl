#include <iterator>
#include <sstream>
#include <type_traits>

#include <bys/count.hpp>
#include <bys/test/test.hpp>

namespace
{
using StreamIterator = std::istream_iterator<int, char, std::char_traits<char>, short>;

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

STATIC_CHECK(std::is_same_v<decltype(bys::count(StreamIterator{}, StreamIterator{}, 0)), short>,
    "bys::count must return the iterator's difference_type");

STATIC_CHECK([] {
    int items[]{1, 2, 1};
    return bys::count(items, items + 3, 1) == 2;
}(),
    "bys::count must work in a constant expression");

TEST_CASE("count.empty")
{
    int comparisons = 0;
    CountedElement items[]{{7, &comparisons}};

    CHECK_EQ(bys::count(items, items, 7), 0);
    CHECK_EQ(comparisons, 0);
}

TEST_CASE("count.missing")
{
    int items[]{1, 3, 5};
    CHECK_EQ(bys::count(items, items + 3, 7), 0);
}

TEST_CASE("count.all_match")
{
    int items[]{7, 7, 7};
    CHECK_EQ(bys::count(items, items + 3, 7), 3);
}

TEST_CASE("count.respects_bounds")
{
    int items[]{7, 1, 7, 7, 2, 7};
    CHECK_EQ(bys::count(items + 1, items + 5, 7), 2);
}

TEST_CASE("count.const_elements")
{
    int const items[]{1, 2};
    CHECK_EQ(bys::count(items, items + 2, 2), 1);
}

TEST_CASE("count.single_pass")
{
    std::istringstream input("4 8 15 8");
    CHECK_EQ(bys::count(StreamIterator{input}, StreamIterator{}, 8), 2);
}

TEST_CASE("count.different_value_type")
{
    Element items[]{{2}, {1}, {2}};
    CHECK_EQ(bys::count(items, items + 3, Needle{2}), 2);
}

TEST_CASE("count.comparison_count")
{
    int comparisons = 0;
    CountedElement items[]{{7, &comparisons}, {1, &comparisons}, {7, &comparisons}};

    CHECK_EQ(bys::count(items, items + 3, 7), 2);
    CHECK_EQ(comparisons, 3);
}
