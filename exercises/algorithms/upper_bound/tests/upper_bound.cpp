#include <algorithm>
#include <array>
#include <forward_list>
#include <functional>
#include <iterator>
#include <list>
#include <numeric>
#include <type_traits>
#include <vector>

#include <bys/test/test.hpp>
#include <bys/upper_bound.hpp>

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

    friend bool operator<(Key const& key, Record const& record)
    {
        return key.value < record.value;
    }
};

struct MeasuredKey
{
    int value;
    int* comparisons;
};

bool operator<(MeasuredKey const& key, int element)
{
    ++*key.comparisons;
    return key.value < element;
}
} // namespace

STATIC_CHECK([] {
    int const items[]{1, 3, 3, 5};
    STATIC_CHECK(std::is_same_v<decltype(bys::upper_bound(items, items + 4, 3)), int const*>,
        "bys::upper_bound must return the iterator type");
    return bys::upper_bound(items, items + 4, 3) == items + 3;
}(),
    "bys::upper_bound must work in a constant expression");

STATIC_CHECK([] {
    int items[]{5, 3, 3, 1};
    STATIC_CHECK(std::is_same_v<decltype(bys::upper_bound(items, items + 4, 3, std::greater<>{})), int*>,
        "bys::upper_bound with a comparator must return the iterator type");
    return bys::upper_bound(items, items + 4, 3, std::greater<>{}) == items + 3;
}(),
    "bys::upper_bound with a comparator must work in a constant expression");

TEST_CASE("upper_bound.small_partitions")
{
    constexpr int max_length = 32;
    int items[max_length]{};
    for (int length = 0; length <= max_length; ++length)
    {
        for (int boundary = 0; boundary <= length; ++boundary)
        {
            std::fill(items, items + boundary, 0);
            std::fill(items + boundary, items + length, 1);
            REQUIRE(bys::upper_bound(items, items + length, 0) == items + boundary);
            REQUIRE(bys::upper_bound(items, items + length, 0, std::not_equal_to<>{}) == items + boundary);
        }
    }
}

TEST_CASE("upper_bound.subrange_and_missing_values")
{
    int items[]{99, 1, 3, 3, 5, -1};
    auto const first = items + 1;
    auto const last = items + 5;

    CHECK(bys::upper_bound(first, last, 0) == first);
    CHECK(bys::upper_bound(first, last, 1) == items + 2);
    CHECK(bys::upper_bound(first, last, 3) == items + 4);
    CHECK(bys::upper_bound(first, last, 4) == items + 4);
    CHECK(bys::upper_bound(first, last, 5) == last);
    CHECK(bys::upper_bound(first, last, 6) == last);
}

TEST_CASE("upper_bound.partitioned_without_sorting")
{
    int items[]{4, 1, 4, 8, 5, 7};
    CHECK(bys::upper_bound(items, items + 6, 4) == items + 3);
}

TEST_CASE("upper_bound.forward_iterators")
{
    std::forward_list<int> const items{1, 3, 3, 5, 7};
    auto const first = items.begin();
    auto const last = items.end();

    CHECK(bys::upper_bound(first, last, 0) == first);
    CHECK(bys::upper_bound(first, last, 3) == std::next(first, 3));
    CHECK(bys::upper_bound(first, last, 4) == std::next(first, 3));
    CHECK(bys::upper_bound(first, last, 7) == last);
}

TEST_CASE("upper_bound.proxy_reference")
{
    std::vector<bool> items{false, false, true, true};
    CHECK(bys::upper_bound(items.begin(), items.end(), false) == items.begin() + 2);
    CHECK(bys::upper_bound(items.begin(), items.end(), true) == items.end());
}

TEST_CASE("upper_bound.heterogeneous_noncopyable_key")
{
    Record const items[]{{1}, {3}, {3}, {5}};
    Key const key{3};
    CHECK(bys::upper_bound(items, items + 4, key) == items + 3);

    int offset = 1;
    auto const before = [offset](Key const& key, Record const& item) {
        return key.value - offset < item.value;
    };
    CHECK(bys::upper_bound(items, items + 4, key, before) == items + 1);
}

TEST_CASE("upper_bound.descending")
{
    std::list<int> const items{9, 7, 7, 4, 1};
    auto const first = items.begin();
    auto const last = items.end();
    bool (*before)(int, int) = [](int value, int element) { return value > element; };

    CHECK(bys::upper_bound(first, last, 10, before) == first);
    CHECK(bys::upper_bound(first, last, 7, before) == std::next(first, 3));
    CHECK(bys::upper_bound(first, last, 6, before) == std::next(first, 3));
    CHECK(bys::upper_bound(first, last, 1, before) == last);
    CHECK(bys::upper_bound(first, last, 0, before) == last);
}

TEST_CASE("upper_bound.empty_and_throwing_comparator")
{
    struct ComparisonError
    {};
    int items[]{1};
    auto const before = [](int, int) -> bool { throw ComparisonError{}; };

    CHECK(bys::upper_bound(items, items, 1, before) == items);
    CHECK_THROWS_AS(bys::upper_bound(items, items + 1, 1, before), ComparisonError);
}

TEST_CASE("upper_bound.comparison_budget")
{
    constexpr int length = 4096;
    // Leave room for the constant term in the logarithmic bound.
    constexpr int budget = 32;

    std::array<int, length> items{};
    std::iota(items.begin(), items.end(), 0);
    std::forward_list<int> const forward_items(items.begin(), items.end());

    int comparisons = 0;
    auto const before = [&comparisons](int value, int element) {
        ++comparisons;
        return value < element;
    };
    for (int value : {-1, 0, length / 2, length - 2, length - 1})
    {
        int const boundary = value + 1;
        comparisons = 0;
        CHECK(bys::upper_bound(items.begin(), items.end(), MeasuredKey{value, &comparisons}) == items.begin() + boundary);
        CHECK(comparisons <= budget);

        comparisons = 0;
        auto const expected = std::next(forward_items.begin(), boundary);
        CHECK(bys::upper_bound(forward_items.begin(), forward_items.end(), value, before) == expected);
        CHECK(comparisons <= budget);
    }
}
