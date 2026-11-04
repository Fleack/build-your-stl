#include <bys/array.hpp>
#include <bys/test/test.hpp>

#include <algorithm>
#include <compare>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace
{
using Array = bys::array<int, 3>;

struct NoDefault
{
    explicit NoDefault(int value) : value(value) {}
    int value;
};

struct AdlSwappable
{
    explicit AdlSwappable(int value) : value(value) {}
    AdlSwappable(AdlSwappable const&) = delete;
    AdlSwappable& operator=(AdlSwappable const&) = delete;

    friend void swap(AdlSwappable& left, AdlSwappable& right) noexcept
    {
        std::swap(left.value, right.value);
    }

    int value;
};

struct SwapError {};

struct ThrowingSwap
{
    friend void swap(ThrowingSwap&, ThrowingSwap&)
    {
        throw SwapError{};
    }
};

struct LessOnly
{
    int value;

    friend bool operator<(LessOnly left, LessOnly right)
    {
        return left.value < right.value;
    }
};

template <bys::array<int, 2> Values>
struct ArrayTag {};
} // namespace

STATIC_CHECK(std::is_aggregate_v<Array>, "array must be an aggregate");
STATIC_CHECK(std::is_same_v<Array::value_type, int> && std::is_same_v<Array::size_type, std::size_t>
    && std::is_same_v<Array::difference_type, std::ptrdiff_t>, "array must expose the standard value and size types");
STATIC_CHECK(std::is_same_v<Array::reference, int&> && std::is_same_v<Array::const_reference, int const&>
    && std::is_same_v<Array::pointer, int*> && std::is_same_v<Array::const_pointer, int const*>,
    "array must expose the standard reference and pointer types");
STATIC_CHECK(std::ranges::contiguous_range<Array> && std::ranges::contiguous_range<Array const>,
    "mutable and const arrays must be contiguous ranges");
STATIC_CHECK(std::is_same_v<Array::iterator, std::ranges::iterator_t<Array>>
    && std::is_same_v<Array::const_iterator, std::ranges::iterator_t<Array const>>, "begin must return the declared iterator types");
STATIC_CHECK(std::is_same_v<Array::reverse_iterator, std::reverse_iterator<Array::iterator>>
    && std::is_same_v<Array::const_reverse_iterator, std::reverse_iterator<Array::const_iterator>>,
    "array must expose the standard reverse iterator types");
STATIC_CHECK(std::is_same_v<ArrayTag<bys::array<int, 2>{1, 2}>, ArrayTag<bys::array<int, 2>{1, 2}>>
    && !std::is_same_v<ArrayTag<bys::array<int, 2>{1, 2}>, ArrayTag<bys::array<int, 2>{2, 1}>>,
    "arrays of structural elements must be usable as template arguments");

STATIC_CHECK([] {
    Array items{1, 2};
    auto const& view = items;
    STATIC_CHECK(std::is_same_v<decltype(items[0]), int&> && std::is_same_v<decltype(view[0]), int const&>
        && std::is_same_v<decltype(items.at(0)), int&> && std::is_same_v<decltype(view.at(0)), int const&>
        && std::is_same_v<decltype(items.front()), int&> && std::is_same_v<decltype(view.front()), int const&>
        && std::is_same_v<decltype(items.back()), int&> && std::is_same_v<decltype(view.back()), int const&>,
        "element access must preserve constness and return references");
    STATIC_CHECK(std::is_same_v<decltype(items.data()), int*> && std::is_same_v<decltype(view.data()), int const*>,
        "data must return a pointer with the matching const qualification");
    STATIC_CHECK(noexcept(items.size()) && noexcept(items.max_size()) && noexcept(items.empty())
        && noexcept(items.data()) && noexcept(view.data())
        && noexcept(items.begin()) && noexcept(view.begin()) && noexcept(items.end()) && noexcept(view.end())
        && noexcept(items.cbegin()) && noexcept(items.cend())
        && noexcept(items.rbegin()) && noexcept(view.rbegin()) && noexcept(items.rend()) && noexcept(view.rend())
        && noexcept(items.crbegin()) && noexcept(items.crend()), "capacity, data and iterator access must be noexcept");

    if (items.empty() || items.size() != 3 || items.max_size() != 3 || view.back() != 0
        || items.data() != &items.front() || view.data() != &view.front()
        || items.end() - items.begin() != 3 || view.end() - view.begin() != 3
        || items.cbegin() != view.begin() || items.cend() != view.end()
        || *items.rbegin() != 0 || items.rend() - items.rbegin() != 3
        || *view.rbegin() != 0 || view.rend() - view.rbegin() != 3
        || items.crbegin() != view.rbegin() || items.crend() != view.rend())
    {
        return false;
    }

    items.at(0) = 7;
    if (view.at(0) != 7 || view[0] != 7)
    {
        return false;
    }

    Array other{};
    other.fill(9);
    items.swap(other);
    return items == Array{9, 9, 9} && other == Array{7, 2, 0};
}(), "array operations must work in constant expressions");

TEST_CASE("array.aggregate_initialization")
{
    Array const value_initialized{};
    Array const partial{8};
    bys::array<NoDefault, 2> const explicit_elements{NoDefault{3}, NoDefault{5}};
    bys::array<int const, 1> const immutable{7};

    CHECK(value_initialized == Array{0, 0, 0});
    CHECK(partial == Array{8, 0, 0});
    CHECK_EQ(explicit_elements.front().value, 3);
    CHECK_EQ(explicit_elements.back().value, 5);
    CHECK_EQ(immutable.front(), 7);
}

TEST_CASE("array.element_access")
{
    Array items{1, 2, 3};
    items.front() = 7;
    items.back() = 9;
    items.data()[1] = 8;
    auto const& view = items;

    CHECK_EQ(view.front(), 7);
    CHECK_EQ(view[1], 8);
    CHECK_EQ(view.at(2), 9);
    CHECK(std::to_address(items.begin()) == items.data());
    CHECK(std::to_address(view.begin()) == view.data());
    CHECK(&view.back() == view.data() + 2);
    CHECK_THROWS_AS(items.at(items.size()), std::out_of_range);
    CHECK_THROWS_AS(view.at(std::numeric_limits<Array::size_type>::max()), std::out_of_range);
}

TEST_CASE("array.iterators_and_algorithms")
{
    Array items{3, 1, 2};
    std::ranges::sort(items);
    CHECK(items == Array{1, 2, 3});

    auto reverse = items.rbegin();
    CHECK_EQ(*reverse, 3);
    *reverse = 4;
    CHECK_EQ(*++reverse, 2);
    CHECK_EQ(*++reverse, 1);
    CHECK(++reverse == items.rend());
    CHECK_EQ(*items.crbegin(), 4);
    CHECK_EQ(std::as_const(items).crend() - std::as_const(items).crbegin(), 3);
}

TEST_CASE("array.zero_size")
{
    bys::array<NoDefault, 0> items{};
    auto const& view = items;
    CHECK(items.empty());
    CHECK_EQ(view.size(), 0u);
    CHECK_EQ(view.max_size(), 0u);
    CHECK(items.begin() == items.end());
    CHECK(view.begin() == view.end());
    CHECK(view.cbegin() == view.cend());
    CHECK(items.rbegin() == items.rend());
    CHECK(view.rbegin() == view.rend());
    CHECK(view.crbegin() == view.crend());
    CHECK_THROWS_AS(items.at(0), std::out_of_range);
    CHECK_THROWS_AS(view.at(0), std::out_of_range);

    bys::array<ThrowingSwap, 0> left{};
    bys::array<ThrowingSwap, 0> right{};
    STATIC_CHECK(noexcept(left.swap(right)) && noexcept(bys::swap(left, right)),
        "swapping empty arrays must be noexcept");
    left.fill(ThrowingSwap{});
    left.swap(right);
    bys::swap(left, right);
}

TEST_CASE("array.copy_move_and_fill")
{
    bys::array<std::string, 2> source{"first", "second"};
    auto copy = source;
    copy.fill("replacement");
    CHECK_EQ(source.front(), "first");
    CHECK_EQ(source.back(), "second");
    CHECK_EQ(copy.front(), "replacement");
    CHECK_EQ(copy.back(), "replacement");
    copy = source;
    CHECK(copy == source);

    bys::array<std::unique_ptr<int>, 1> owner{std::make_unique<int>(42)};
    auto moved = std::move(owner);
    CHECK(owner.front() == nullptr);
    owner = std::move(moved);
    CHECK(moved.front() == nullptr);
    REQUIRE(owner.front() != nullptr);
    CHECK_EQ(*owner.front(), 42);
}

TEST_CASE("array.swap_preserves_element_addresses")
{
    bys::array<AdlSwappable, 2> left{AdlSwappable{1}, AdlSwappable{2}};
    bys::array<AdlSwappable, 2> right{AdlSwappable{3}, AdlSwappable{4}};
    auto const position = left.begin();
    auto const address = std::addressof(right.back());
    STATIC_CHECK(noexcept(left.swap(right)) && noexcept(bys::swap(left, right)), "swap must honor the element exception specification");

    left.swap(right);
    CHECK(position == left.begin());
    CHECK(std::addressof(right.back()) == address);
    CHECK_EQ(position->value, 3);
    CHECK_EQ(left.back().value, 4);
    CHECK_EQ(right.front().value, 1);
    CHECK_EQ(address->value, 2);

    bys::swap(left, right);
    CHECK_EQ(position->value, 1);
    CHECK_EQ(address->value, 4);
}

TEST_CASE("array.swap_propagates_exceptions")
{
    bys::array<ThrowingSwap, 1> left{};
    bys::array<ThrowingSwap, 1> right{};
    STATIC_CHECK(!noexcept(left.swap(right)) && !noexcept(bys::swap(left, right)), "swap may throw when element swap may throw");
    CHECK_THROWS_AS(left.swap(right), SwapError);
    CHECK_THROWS_AS(bys::swap(left, right), SwapError);
}

TEST_CASE("array.lexicographical_comparison")
{
    Array const left{1, 2, 9};
    Array const right{1, 3, 0};
    CHECK(left != right);
    CHECK(left < right);
    CHECK(left <= right);
    CHECK(right > left);
    CHECK(right >= left);
    CHECK((left <=> right) == std::strong_ordering::less);
    CHECK((right <=> left) == std::strong_ordering::greater);
    CHECK((left <=> Array{1, 2, 9}) == std::strong_ordering::equal);

    bys::array<int, 0> const empty{};
    CHECK(empty == bys::array<int, 0>{});
    CHECK((empty <=> bys::array<int, 0>{}) == std::strong_ordering::equal);

    bys::array<LessOnly, 1> const less_only{LessOnly{1}};
    CHECK((less_only <=> bys::array<LessOnly, 1>{LessOnly{2}}) == std::weak_ordering::less);
    bys::array<double, 1> const nan{std::numeric_limits<double>::quiet_NaN()};
    CHECK((nan <=> bys::array<double, 1>{0.0}) == std::partial_ordering::unordered);
}
