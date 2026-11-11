#include <algorithm>
#include <array>
#include <bit>
#include <compare>
#include <cstddef>
#include <deque>
#include <forward_list>
#include <functional>
#include <iterator>
#include <list>
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

struct ComparisonResult
{
    bool value;

    explicit operator bool() const
    {
        return value;
    }

    bool operator!() const = delete;
};

struct Record
{
    explicit Record(int value)
        : value(value)
    {}
    Record(Record const&) = delete;

    int value;

    ComparisonResult operator<(Key const& key) const
    {
        return {value < key.value};
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

template <bool RandomAccess>
struct CountingIterator
{
    using iterator_category = std::conditional_t<RandomAccess, std::random_access_iterator_tag, std::forward_iterator_tag>;
    using value_type = int;
    using difference_type = std::ptrdiff_t;
    using pointer = int const*;
    using reference = int const&;

    pointer current = nullptr;
    int* steps = nullptr;

    constexpr reference operator*() const
    {
        return *current;
    }

    constexpr pointer operator->() const
    {
        return current;
    }

    constexpr CountingIterator& operator++()
    {
        ++current;
        ++*steps;
        return *this;
    }

    constexpr CountingIterator operator++(int)
    {
        auto previous = *this;
        ++*this;
        return previous;
    }

    constexpr bool operator==(CountingIterator const& other) const
    {
        return current == other.current;
    }

    constexpr CountingIterator& operator--() requires RandomAccess
    {
        --current;
        ++*steps;
        return *this;
    }

    constexpr CountingIterator operator--(int) requires RandomAccess
    {
        auto previous = *this;
        --*this;
        return previous;
    }

    constexpr CountingIterator& operator+=(difference_type offset) requires RandomAccess
    {
        current += offset;
        ++*steps;
        return *this;
    }

    constexpr CountingIterator& operator-=(difference_type offset) requires RandomAccess
    {
        return *this += -offset;
    }

    constexpr reference operator[](difference_type offset) const requires RandomAccess
    {
        return *(*this + offset);
    }

    friend constexpr CountingIterator operator+(CountingIterator iterator, difference_type offset) requires RandomAccess
    {
        return iterator += offset;
    }

    friend constexpr CountingIterator operator+(difference_type offset, CountingIterator iterator) requires RandomAccess
    {
        return iterator += offset;
    }

    friend constexpr CountingIterator operator-(CountingIterator iterator, difference_type offset) requires RandomAccess
    {
        return iterator -= offset;
    }

    constexpr difference_type operator-(CountingIterator const& other) const requires RandomAccess
    {
        return current - other.current;
    }

    constexpr auto operator<=>(CountingIterator const& other) const requires RandomAccess
    {
        return current <=> other.current;
    }
};

using CountingForwardIterator = CountingIterator<false>;
using CountingRandomAccessIterator = CountingIterator<true>;

static_assert(std::forward_iterator<CountingForwardIterator>);
static_assert(std::random_access_iterator<CountingRandomAccessIterator>);
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

STATIC_CHECK([] {
    int const items[]{1, 3, 3, 5};
    int steps = 0;
    CountingForwardIterator const first{items, &steps};
    CountingForwardIterator const last{items + 4, &steps};
    CountingForwardIterator const expected{items + 1, &steps};
    return bys::lower_bound(first, last, 3) == expected && bys::lower_bound(first, last, 3, std::less<>{}) == expected;
}(),
    "both bys::lower_bound overloads must support constexpr forward iterators");

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

            int steps = 0;
            CountingForwardIterator const first{items, &steps};
            CountingForwardIterator const last{items + length, &steps};
            CountingForwardIterator const expected{items + boundary, &steps};
            REQUIRE(bys::lower_bound(first, last, 1) == expected);
            REQUIRE(bys::lower_bound(first, last, 1, std::not_equal_to<>{}) == expected);
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
    std::array items{4, 1, 3, 8, 5, 7};
    auto const original = items;
    CHECK(bys::lower_bound(items.begin(), items.end(), 5) == items.begin() + 3);
    CHECK(items == original);

    auto const before = [](int element, int value) { return element % 2 == value; };
    std::array parity_items{7, 3, 5, 8, 4, 2};
    auto const original_parity = parity_items;
    CHECK(bys::lower_bound(parity_items.begin(), parity_items.end(), 1, before) == parity_items.begin() + 3);
    CHECK(parity_items == original_parity);
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

TEST_CASE("lower_bound.bidirectional_and_noncontiguous_iterators")
{
    std::list<int> const linked{1, 3, 3, 5, 7};
    CHECK(bys::lower_bound(linked.begin(), linked.end(), 3) == std::next(linked.begin()));
    CHECK(bys::lower_bound(linked.rbegin(), linked.rend(), 3, std::greater<>{}) == std::next(linked.rbegin(), 2));

    std::deque<int> segmented(1024);
    std::iota(segmented.begin(), segmented.end(), 0);
    CHECK(bys::lower_bound(segmented.cbegin(), segmented.cend(), 700) == segmented.cbegin() + 700);
    CHECK(bys::lower_bound(segmented.begin(), segmented.end(), 300, std::less<>{}) == segmented.begin() + 300);
}

TEST_CASE("lower_bound.heterogeneous_noncopyable_values")
{
    Record const items[]{Record{1}, Record{3}, Record{3}, Record{5}};
    Key const key{3};
    CHECK(bys::lower_bound(items, items + 4, key) == items + 1);

    int const offset = 1;
    auto const before = [offset](Record const& item, Key const& key) -> ComparisonResult {
        return {item.value < key.value + offset};
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
    struct Element
    {
        bool operator<(int) const
        {
            throw ComparisonError{};
        }
    };

    Element elements[1]{};
    CHECK(bys::lower_bound(elements, elements, 1) == elements);
    CHECK_THROWS_AS(bys::lower_bound(elements, elements + 1, 1), ComparisonError);

    int items[]{1};
    auto const before = [](int, int) -> bool { throw ComparisonError{}; };

    CHECK(bys::lower_bound(items, items, 1, before) == items);
    CHECK_THROWS_AS(bys::lower_bound(items, items + 1, 1, before), ComparisonError);
}

TEST_CASE("lower_bound.const_lvalue_comparator")
{
    struct Compare
    {
        bool operator()(int element, int value)
        {
            return element < value;
        }
    };

    int const items[]{1, 3, 3, 5};
    Compare const before{};
    CHECK(bys::lower_bound(items, items + 4, 3, before) == items + 1);
}

TEST_CASE("lower_bound.comparator_by_reference")
{
    struct Entry
    {
        int value;
    };

    struct Compare
    {
        Compare() = default;
        Compare(Compare const&) = delete;

        int calls = 0;

        bool operator()(Entry const& element, Key const& key)
        {
            ++calls;
            return element.value < key.value;
        }
    };

    Entry const items[]{{1}, {3}, {3}, {5}};
    Compare before;
    CHECK(bys::lower_bound(items, items + 4, Key{3}, std::ref(before)) == items + 1);
    CHECK(before.calls > 0);
}

TEST_CASE("lower_bound.pointer_order")
{
    int first = 0;
    int second = 0;
    int third = 0;
    std::array items{&first, &second, &third};
    std::sort(items.begin(), items.end(), std::less<>{});

    for (auto it = items.begin(); it != items.end(); ++it)
    {
        CHECK(bys::lower_bound(items.begin(), items.end(), *it) == it);
    }
}

TEST_CASE("lower_bound.search_complexity")
{
    std::array<int, 4096> items{};
    std::iota(items.begin(), items.end(), 0);

    int comparisons = 0;
    auto const before = [&comparisons](int element, int value) {
        ++comparisons;
        return element < value;
    };
    for (int length : {63, 256, 4093, 4096})
    {
        // These regression budgets sample the growth rates, with room for different implementations.
        int const logarithmic_size = std::bit_width(static_cast<unsigned>(length));
        int const comparison_budget = logarithmic_size + 4;
        auto const first = items.data();
        auto const last = first + length;
        int steps = 0;

        auto const check_cost = [&]<class Iterator>(int value, int step_budget) {
            Iterator const begin{first, &steps};
            Iterator const end{last, &steps};
            Iterator const expected{first + value, &steps};

            steps = 0;
            comparisons = 0;
            CHECK(bys::lower_bound(begin, end, MeasuredKey{value, &comparisons}) == expected);
            CHECK(comparisons <= comparison_budget);
            CHECK(steps <= step_budget);

            steps = 0;
            comparisons = 0;
            CHECK(bys::lower_bound(begin, end, value, before) == expected);
            CHECK(comparisons <= comparison_budget);
            CHECK(steps <= step_budget);
        };

        for (int value : {0, 1, length / 2, length - 1, length})
        {
            check_cost.operator()<CountingForwardIterator>(value, 4 * length);
            check_cost.operator()<CountingRandomAccessIterator>(value, 8 * logarithmic_size);
        }
    }
}
