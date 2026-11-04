#include <algorithm>
#include <array>
#include <forward_list>
#include <functional>
#include <iterator>
#include <numeric>
#include <type_traits>
#include <vector>

#include <bys/lower_bound.hpp>
#include <bys/test/test.hpp>

namespace
{
struct Key
{
    explicit Key(int value)
        : value(value)
    {}
    Key(Key const&) = delete;

    int value;
};

struct Record
{
    int value;

    bool operator<(Key const& key) const
    {
        return value < key.value;
    }
};

struct MeasuredKey
{
    int value;
    int* comparisons;
};

bool operator<(int element, MeasuredKey const& key)
{
    ++*key.comparisons;
    return element < key.value;
}
} // namespace

STATIC_CHECK([] {
    int const items[]{1, 3, 3, 5};
    STATIC_CHECK(std::is_same_v<decltype(bys::lower_bound(items, items + 4, 3)), int const*>,
        "bys::lower_bound must return the iterator type");
    return bys::lower_bound(items, items + 4, 3) == items + 1;
}(),
    "bys::lower_bound must work in a constant expression");

STATIC_CHECK([] {
    int items[]{5, 3, 3, 1};
    STATIC_CHECK(std::is_same_v<decltype(bys::lower_bound(items, items + 4, 3, std::greater<>{})), int*>,
        "bys::lower_bound with a comparator must return the iterator type");
    return bys::lower_bound(items, items + 4, 3, std::greater<>{}) == items + 1;
}(),
    "bys::lower_bound with a comparator must work in a constant expression");

TEST_CASE("lower_bound.small_partitions")
{
    constexpr int max_length = 32;
    int items[max_length]{};
    for (int length = 0; length <= max_length; ++length)
    {
        for (int boundary = 0; boundary <= length; ++boundary)
        {
            std::fill(items, items + boundary, 0);
            std::fill(items + boundary, items + length, 1);
            REQUIRE(bys::lower_bound(items, items + length, 1) == items + boundary);
            REQUIRE(bys::lower_bound(items, items + length, 1, std::not_equal_to<>{}) == items + boundary);
        }
    }
}

TEST_CASE("lower_bound.subrange_and_missing_values")
{
    int items[]{99, 1, 3, 3, 5, -1};
    auto const first = items + 1;
    auto const last = items + 5;

    CHECK(bys::lower_bound(first, last, 0) == first);
    CHECK(bys::lower_bound(first, last, 3) == items + 2);
    CHECK(bys::lower_bound(first, last, 4) == items + 4);
    CHECK(bys::lower_bound(first, last, 5) == items + 4);
    CHECK(bys::lower_bound(first, last, 6) == last);
}

TEST_CASE("lower_bound.partitioned_without_sorting")
{
    int items[]{4, 1, 3, 8, 5, 7};
    CHECK(bys::lower_bound(items, items + 6, 5) == items + 3);
}

TEST_CASE("lower_bound.forward_iterators")
{
    std::forward_list<int> const items{1, 3, 3, 5, 7};
    auto const first = items.begin();
    auto const last = items.end();

    CHECK(bys::lower_bound(first, last, 0) == first);
    CHECK(bys::lower_bound(first, last, 3) == std::next(first));
    CHECK(bys::lower_bound(first, last, 4) == std::next(first, 3));
    CHECK(bys::lower_bound(first, last, 8) == last);
}

TEST_CASE("lower_bound.proxy_reference")
{
    std::vector<bool> items{false, false, true, true};
    CHECK(bys::lower_bound(items.begin(), items.end(), true) == items.begin() + 2);
}

TEST_CASE("lower_bound.heterogeneous_noncopyable_key")
{
    Record const items[]{{1}, {3}, {3}, {5}};
    Key const key{3};
    CHECK(bys::lower_bound(items, items + 4, key) == items + 1);

    int const offset = 1;
    auto const before = [offset](Record const& item, Key const& key) {
        return item.value < key.value + offset;
    };
    CHECK(bys::lower_bound(items, items + 4, key, before) == items + 3);
}

TEST_CASE("lower_bound.descending")
{
    int const items[]{9, 7, 7, 4, 1};
    bool (*before)(int, int) = [](int element, int value) { return element > value; };
    CHECK(bys::lower_bound(items, items + 5, 10, before) == items);
    CHECK(bys::lower_bound(items, items + 5, 7, before) == items + 1);
    CHECK(bys::lower_bound(items, items + 5, 6, before) == items + 3);
    CHECK(bys::lower_bound(items, items + 5, 0, before) == items + 5);
}

TEST_CASE("lower_bound.empty_and_throwing_comparator")
{
    struct ComparisonError
    {};
    int items[]{1};
    auto const before = [](int, int) -> bool { throw ComparisonError{}; };

    CHECK(bys::lower_bound(items, items, 1, before) == items);
    CHECK_THROWS_AS(bys::lower_bound(items, items + 1, 1, before), ComparisonError);
}

TEST_CASE("lower_bound.comparison_budget")
{
    constexpr int length = 4096;
    // Leave room for the constant term in the logarithmic bound.
    constexpr int budget = 32;

    std::array<int, length> items{};
    std::iota(items.begin(), items.end(), 0);
    std::forward_list<int> const forward_items(items.begin(), items.end());

    int comparisons = 0;
    auto const before = [&comparisons](int element, int value) {
        ++comparisons;
        return element < value;
    };
    for (int value : {0, 1, length / 2, length - 1, length})
    {
        comparisons = 0;
        CHECK(bys::lower_bound(items.begin(), items.end(), MeasuredKey{value, &comparisons}) == items.begin() + value);
        CHECK(comparisons <= budget);

        comparisons = 0;
        auto const expected = std::next(forward_items.begin(), value);
        CHECK(bys::lower_bound(forward_items.begin(), forward_items.end(), value, before) == expected);
        CHECK(comparisons <= budget);
    }
}
