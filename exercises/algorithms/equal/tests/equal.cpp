#include <iterator>
#include <sstream>
#include <type_traits>

#include <bys/equal.hpp>
#include <bys/test/test.hpp>

namespace
{
using StreamIterator = std::istream_iterator<int>;

struct CountedElement
{
    int value;
    int* comparisons;

    bool operator==(int other) const
    {
        ++*comparisons;
        return value == other;
    }
};
} // namespace

STATIC_CHECK([] {
    int const left[]{1, 2, 3};
    int const right[]{1, 2, 3};
    STATIC_CHECK(std::is_same_v<decltype(bys::equal(left, left + 3, right, right + 3)), bool>,
        "bys::equal must return bool");
    return bys::equal(left, left + 3, right, right + 3);
}(), "bys::equal must work in a constant expression");

TEST_CASE("equal.empty")
{
    int items[]{1};
    CHECK(bys::equal(items, items, items, items));
    CHECK_FALSE(bys::equal(items, items, items, items + 1));
    CHECK_FALSE(bys::equal(items, items + 1, items, items));
}

TEST_CASE("equal.respects_bounds")
{
    int const left[]{0, 1, 2, 3, 4};
    int const right[]{9, 1, 2, 3, 8};
    CHECK(bys::equal(left + 1, left + 4, right + 1, right + 4));
}

TEST_CASE("equal.different_elements")
{
    int const left[]{1, 2, 3};
    int const right[]{1, 3, 2};
    CHECK_FALSE(bys::equal(left, left + 3, right, right + 3));
}

TEST_CASE("equal.single_pass")
{
    std::istringstream left("4 8 15");
    std::istringstream right("4 8 15");
    CHECK(bys::equal(StreamIterator{left}, StreamIterator{}, StreamIterator{right}, StreamIterator{}));
}

TEST_CASE("equal.single_pass_different_lengths")
{
    int comparisons = 0;
    CountedElement const items[]{{4, &comparisons}, {8, &comparisons}};
    std::istringstream longer("4 8 15");
    std::istringstream shorter("4");

    CHECK_FALSE(bys::equal(items, items + 2, StreamIterator{longer}, StreamIterator{}));
    CHECK(comparisons <= 2);

    comparisons = 0;
    CHECK_FALSE(bys::equal(items, items + 2, StreamIterator{shorter}, StreamIterator{}));
    CHECK(comparisons <= 1);
}

TEST_CASE("equal.comparison_limit")
{
    int comparisons = 0;
    CountedElement const left[]{{1, &comparisons}, {2, &comparisons}, {3, &comparisons}};
    int const right[]{1, 2, 3};

    CHECK(bys::equal(left, left + 3, right, right + 3));
    CHECK(comparisons <= 3);
}

TEST_CASE("equal.random_access_different_lengths")
{
    int comparisons = 0;
    CountedElement const left[]{{1, &comparisons}, {2, &comparisons}, {3, &comparisons}};
    int const right[]{1, 2};

    CHECK_FALSE(bys::equal(left, left + 3, right, right + 2));
    CHECK_EQ(comparisons, 0);
}
