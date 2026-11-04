#include <algorithm>
#include <array>
#include <cstddef>
#include <deque>
#include <iterator>
#include <limits>
#include <memory>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

#include <bys/span.hpp>
#include <bys/test/test.hpp>

namespace
{
using Dynamic = bys::span<int>;
using Fixed = bys::span<int, 3>;
using ReadOnly = bys::span<int const>;
using ReadOnlyFixed = bys::span<int const, 3>;
using Zero = bys::span<int, 0>;

struct Base
{
    int value;
};
struct Derived : Base
{};
struct SourceError
{};

struct SizedEnd
{
    int* end;
    bool fail = false;

    friend bool operator==(int* position, SizedEnd sentinel)
    {
        return position == sentinel.end;
    }

    friend std::ptrdiff_t operator-(SizedEnd sentinel, int* position)
    {
        if (sentinel.fail)
        {
            throw SourceError{};
        }
        return sentinel.end - position;
    }

    friend std::ptrdiff_t operator-(int* position, SizedEnd sentinel)
    {
        return -(sentinel - position);
    }
};

struct Range
{
    enum class Failure
    {
        none,
        data,
        size
    };

    int* first;
    std::size_t count;
    Failure failure;

    int* begin() const { return first; }
    int* end() const { return first + count; }
    int* data() const
    {
        if (failure == Failure::data)
        {
            throw SourceError{};
        }
        return first;
    }
    std::size_t size() const
    {
        if (failure == Failure::size)
        {
            throw SourceError{};
        }
        return count;
    }
};

template <class T>
void accept(T);

template <class T, class... Args>
concept ImplicitListConstruction = requires (Args... args) { accept<T>({args...}); };
} // namespace

STATIC_CHECK(bys::dynamic_extent == std::numeric_limits<std::size_t>::max() && Dynamic::extent == bys::dynamic_extent && Fixed::extent == 3 && Zero::extent == 0,
    "extent must reflect the template argument");
STATIC_CHECK(std::is_same_v<ReadOnly::element_type, int const>&& std::is_same_v<ReadOnly::value_type, int>&& std::is_same_v<Dynamic::size_type, std::size_t>&& std::is_same_v<Dynamic::difference_type, std::ptrdiff_t>&& std::is_same_v<Dynamic::pointer, int*>&& std::is_same_v<Dynamic::const_pointer, int const*>&& std::is_same_v<Dynamic::reference, int&>&& std::is_same_v<Dynamic::const_reference, int const&>&& std::is_same_v<ReadOnly::pointer, int const*>&& std::is_same_v<ReadOnly::reference, int const&>&& std::is_same_v<Fixed::reverse_iterator, std::reverse_iterator<Fixed::iterator>>,
    "span must expose its element, size, reference and iterator types");
STATIC_CHECK(std::is_nothrow_default_constructible_v<Dynamic>&& std::is_nothrow_default_constructible_v<Zero> && !std::is_default_constructible_v<Fixed>, "only empty and dynamic spans can be default constructed");
STATIC_CHECK(std::is_nothrow_copy_constructible_v<Fixed>&& std::is_nothrow_copy_assignable_v<Fixed>&& std::is_nothrow_destructible_v<Fixed>, "span copy operations and destruction must be noexcept");

STATIC_CHECK(ImplicitListConstruction<Dynamic, int*, std::size_t>&& ImplicitListConstruction<Dynamic, int*, int*> && !ImplicitListConstruction<Fixed, int*, std::size_t> && !ImplicitListConstruction<Fixed, int*, int*>, "iterator constructors must be explicit for fixed extent");
STATIC_CHECK(std::is_convertible_v<int (&)[3], Fixed>&& std::is_convertible_v<std::array<int, 3>&, Fixed>&& std::is_convertible_v<std::array<int, 3> const&, ReadOnlyFixed> && !std::is_constructible_v<Fixed, int (&)[2]> && !std::is_constructible_v<Fixed, std::array<int, 2>&> && !std::is_constructible_v<Fixed, int const (&)[3]>, "array constructors must match extent and qualifications");
STATIC_CHECK(std::is_convertible_v<Fixed, Dynamic>&& std::is_convertible_v<Fixed, ReadOnlyFixed>&& std::is_convertible_v<Fixed, ReadOnly>&& std::is_convertible_v<Dynamic, ReadOnly>&& std::is_nothrow_constructible_v<ReadOnlyFixed, Dynamic> && !std::is_convertible_v<Dynamic, ReadOnlyFixed> && !std::is_constructible_v<Fixed, bys::span<int, 2>> && !std::is_constructible_v<Dynamic, ReadOnly>,
    "span conversions must respect the extent, constness and explicitness");
STATIC_CHECK(std::is_convertible_v<Dynamic, bys::span<int const volatile>> && !std::is_constructible_v<Dynamic, bys::span<int volatile>> && !std::is_constructible_v<bys::span<Base>, Derived*, std::size_t> && !std::is_constructible_v<bys::span<Base const>, bys::span<Derived>> && !std::is_constructible_v<Dynamic, short*, short*>, "only qualification conversions may change the element type");

using UnsizedRange = std::ranges::subrange<int*, std::unreachable_sentinel_t>;
STATIC_CHECK(std::is_convertible_v<std::vector<int>&, Dynamic>&& std::is_constructible_v<Fixed, std::vector<int>&> && !std::is_convertible_v<std::vector<int>&, Fixed> && std::is_constructible_v<ReadOnly, std::vector<int>> && !std::is_constructible_v<Dynamic, std::vector<int>> && !std::is_constructible_v<Dynamic, std::vector<int> const&> && !std::is_constructible_v<Dynamic, std::deque<int>&> && !std::is_constructible_v<ReadOnly, std::vector<bool>&> && !std::is_constructible_v<Dynamic, UnsizedRange> && !std::is_constructible_v<Dynamic, int*, std::unreachable_sentinel_t>,
    "range constructors must enforce contiguity, size, borrowing and element compatibility");

STATIC_CHECK(std::ranges::view<Dynamic>&& std::ranges::view<Fixed>&& std::ranges::view<Zero>&& std::ranges::borrowed_range<Dynamic>&& std::ranges::borrowed_range<Fixed>&& std::ranges::borrowed_range<Zero>&& std::ranges::contiguous_range<Fixed const>&& std::ranges::sized_range<Fixed const>&& std::ranges::common_range<Fixed const>, "span must participate in the C++20 ranges concepts");

STATIC_CHECK([] {
    int items[]{1, 2, 3};
    Fixed const view = items;
    STATIC_CHECK(std::is_same_v<decltype(view.begin()), Fixed::iterator> && std::is_same_v<decltype(view[0]), int&> && std::is_same_v<decltype(view.front()), int&> && std::is_same_v<decltype(view.back()), int&> && std::is_same_v<decltype(view.data()), int*>,
        "const qualification of the view must preserve mutable element access");
    STATIC_CHECK(noexcept(view.size()) && noexcept(view.size_bytes()) && noexcept(view.empty()) && noexcept(view.data()) && noexcept(view.begin()) && noexcept(view.end()) && noexcept(view.rbegin()) && noexcept(view.rend()),
        "size, data and iterator queries must be noexcept");

    view.front() = 4;
    *view.rbegin() = 9;
    Dynamic copy = view;
    ReadOnlyFixed read_only(copy);
    copy = Dynamic{};
    return items[0] == 4 && items[2] == 9 && view[1] == 2 && view.back() == 9 && view.data() == items && view.end() - view.begin() == 3 && view.rend() - view.rbegin() == 3 && view.size() == 3 && view.size_bytes() == sizeof(items) && !view.empty() && read_only.data() == items && read_only.size() == 3 && copy.empty() && copy.data() == nullptr;
}(),
    "construction, conversion, element access and iteration must work in constant expressions");

TEST_CASE("span.default_and_empty")
{
    Dynamic const dynamic;
    Zero const fixed;
    CHECK(dynamic.empty() && fixed.empty());
    CHECK(dynamic.data() == nullptr && fixed.data() == nullptr);
    CHECK_EQ(dynamic.size(), 0u);
    CHECK_EQ(fixed.size_bytes(), 0u);
    CHECK(dynamic.begin() == dynamic.end());
    CHECK(fixed.rbegin() == fixed.rend());

    int item = 7;
    Zero const empty_at_address{&item, std::size_t{0}};
    CHECK(empty_at_address.empty());
    CHECK(empty_at_address.data() == &item);
}

TEST_CASE("span.array_deduction")
{
    int items[]{1, 2, 3};
    int const read_only[]{4, 5, 6};
    std::array<int, 3> array{7, 8, 9};
    std::array<int, 0> const empty{};
    auto raw_view = bys::span{items};
    auto const_view = bys::span{read_only};
    auto array_view = bys::span{array};
    auto const_array_view = bys::span{std::as_const(array)};
    auto empty_view = bys::span{empty};
    STATIC_CHECK(std::is_same_v<decltype(raw_view), Fixed> && std::is_same_v<decltype(const_view), ReadOnlyFixed> && std::is_same_v<decltype(array_view), Fixed> && std::is_same_v<decltype(const_array_view), ReadOnlyFixed> && std::is_same_v<decltype(empty_view), bys::span<int const, 0>>, "array deduction must retain extent and constness");
    CHECK(raw_view.data() == items && const_view.data() == read_only);
    CHECK(array_view.data() == array.data() && const_array_view.data() == array.data());
    array_view[1] = 10;
    CHECK_EQ(array[1], 10);
    CHECK(empty_view.empty() && empty_view.data() == empty.data());
}

TEST_CASE("span.iterator_construction")
{
    std::vector<int> items{1, 2, 3, 4, 5};
    auto by_count = bys::span(items.begin() + 1, 3);
    auto by_end = bys::span(items.data() + 1, items.data() + 4);
    auto const by_sentinel = bys::span(std::counted_iterator{items.data() + 1, 3}, std::default_sentinel);
    Fixed const fixed_by_count{items.begin() + 1, 3};
    Fixed const fixed_by_sentinel{items.data() + 1, SizedEnd{items.data() + 4}};

    STATIC_CHECK(std::is_same_v<decltype(by_count), Dynamic> && std::is_same_v<decltype(by_end), Dynamic>,
        "iterator-pair deduction must yield dynamic extent");
    CHECK(by_count.data() == items.data() + 1 && by_end.data() == by_count.data());
    CHECK(by_sentinel.data() == by_count.data() && fixed_by_count.data() == by_count.data());
    CHECK(fixed_by_sentinel.data() == by_count.data());
    CHECK_EQ(by_count.size(), 3u);
    CHECK_EQ(by_end.size(), 3u);
    CHECK_EQ(by_sentinel.size(), 3u);
    CHECK_EQ(fixed_by_sentinel.back(), 4);
    CHECK_THROWS_AS((Dynamic{items.data(), SizedEnd{items.data() + items.size(), true}}), SourceError);
}

TEST_CASE("span.range_construction")
{
    std::vector<int> items{1, 2, 3};
    auto deduced = bys::span{items};
    auto const_deduced = bys::span{std::as_const(items)};
    STATIC_CHECK(std::is_same_v<decltype(deduced), Dynamic> && std::is_same_v<decltype(const_deduced), ReadOnly>,
        "range deduction must yield dynamic extent and preserve element constness");
    Fixed const fixed{items};
    Dynamic borrowed = std::ranges::subrange(items.data() + 1, items.data() + 3);
    CHECK(deduced.data() == items.data() && const_deduced.data() == items.data());
    CHECK_EQ(deduced.size(), 3u);
    CHECK_EQ(const_deduced.size(), 3u);
    CHECK(fixed.data() == items.data());
    CHECK(borrowed.data() == items.data() + 1);
    CHECK_EQ(borrowed.size(), 2u);

    auto const read_first = [](ReadOnly view) { return view.front(); };
    CHECK_EQ(read_first(std::vector{7, 8}), 7);

    Range source{items.data(), items.size(), Range::Failure::none};
    CHECK(Dynamic{source}.data() == items.data());
    source.failure = Range::Failure::data;
    CHECK_THROWS_AS((Dynamic{source}), SourceError);
    source.failure = Range::Failure::size;
    CHECK_THROWS_AS((Dynamic{source}), SourceError);
}

TEST_CASE("span.copy_and_rebind")
{
    auto owner = std::make_unique<int>(7);
    int* const address = owner.get();
    int other[]{1, 2};
    Dynamic view{address, 1};
    {
        Dynamic const copy = view;
        CHECK(copy.data() == address);
        copy.front() = 9;
        view = Dynamic{other};
        CHECK(copy.data() == address);
        CHECK_EQ(copy.size(), 1u);
    }
    CHECK_EQ(*owner, 9);
    CHECK(view.data() == other);
    CHECK_EQ(view.size(), 2u);

    int first[]{3, 4, 5};
    int second[]{6, 7, 8};
    Fixed fixed = first;
    Fixed const fixed_copy = fixed;
    fixed = Fixed{second};
    CHECK(fixed_copy.data() == first && fixed.data() == second);
    CHECK_EQ(fixed_copy.front(), 3);
    CHECK_EQ(fixed.back(), 8);
}

TEST_CASE("span.ranges_integration")
{
    int items[]{3, 1, 2};
    std::ranges::sort(bys::span{items});
    auto found = std::ranges::find(bys::span{items}, 2);
    STATIC_CHECK(std::is_same_v<decltype(found), Fixed::iterator>, "a temporary span must return a live iterator");
    CHECK(std::to_address(found) == items + 1);
    auto reverse = bys::span{items} | std::views::reverse;
    CHECK_EQ(*reverse.begin(), 3);
}
