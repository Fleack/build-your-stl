#include <bys/vector.hpp>
#include <bys/test/test.hpp>

#include <algorithm>
#include <array>
#include <compare>
#include <iterator>
#include <limits>
#include <memory>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace
{
using Vector = bys::vector<int>;

struct Node
{
    int value;
    bys::vector<Node> children;
};
} // namespace

STATIC_CHECK(std::is_same_v<Vector::value_type, int> && std::is_same_v<Vector::allocator_type, std::allocator<int>>
    && std::is_same_v<Vector::pointer, int*> && std::is_same_v<Vector::const_pointer, int const*>
    && std::is_same_v<Vector::reference, int&> && std::is_same_v<Vector::const_reference, int const&>,
    "vector must expose its element and allocator types");
STATIC_CHECK(std::is_unsigned_v<Vector::size_type> && std::is_signed_v<Vector::difference_type>
    && std::is_same_v<Vector::iterator, std::ranges::iterator_t<Vector>>
    && std::is_same_v<Vector::const_iterator, std::ranges::iterator_t<Vector const>>
    && std::is_same_v<Vector::reverse_iterator, std::reverse_iterator<Vector::iterator>>
    && std::is_same_v<Vector::const_reverse_iterator, std::reverse_iterator<Vector::const_iterator>>,
    "vector must expose its size and iterator types");
STATIC_CHECK(std::ranges::contiguous_range<Vector> && std::ranges::contiguous_range<Vector const>,
    "mutable and const vectors must be contiguous ranges");
STATIC_CHECK(std::is_same_v<decltype(bys::vector{1, 2}), Vector>
    && std::is_same_v<decltype(bys::vector(std::declval<int*>(), std::declval<int*>())), Vector>,
    "deduction must work for initializer lists and iterator pairs");
STATIC_CHECK(std::is_nothrow_default_constructible_v<Vector> && std::is_nothrow_move_constructible_v<Vector>
    && std::is_nothrow_move_assignable_v<Vector>, "default-allocator construction and moves must be noexcept");

STATIC_CHECK([] {
    Vector items{1, 3};
    items.reserve(4);
    items.emplace(items.cbegin() + 1, 2);
    items.push_back(4);
    items.pop_back();
    Vector copy = items;
    copy.resize(4, 5);
    return copy.size() == 4 && copy.capacity() >= copy.size() && copy.at(1) == 2
        && copy.data()[2] == 3 && *copy.crbegin() == 5 && items.back() == 3;
}(), "construction, allocation, modifiers and access must work in constant expressions");

TEST_CASE("vector.construction")
{
    Vector empty;
    CHECK(empty.empty());
    CHECK_EQ(empty.size(), 0u);
    CHECK(empty.begin() == empty.end());
    CHECK(empty.rbegin() == empty.rend());

    CHECK(std::ranges::equal(Vector(3), std::array{0, 0, 0}));
    CHECK(std::ranges::equal(Vector(3, 7), std::array{7, 7, 7}));
    CHECK(std::ranges::equal(Vector{3, 7}, std::array{3, 7}));

    int const source[]{2, 4, 6, 8};
    Vector const subrange(source + 1, source + 3);
    CHECK(std::ranges::equal(subrange, std::array{4, 6}));

    std::istringstream input("5 6 7 8");
    using Iterator = std::istream_iterator<int>;
    Vector const single_pass(Iterator{input}, Iterator{});
    CHECK(std::ranges::equal(single_pass, std::array{5, 6, 7, 8}));
}

TEST_CASE("vector.assignment")
{
    Vector items{9, 8, 7};
    items.assign(2, 4);
    CHECK(std::ranges::equal(items, std::array{4, 4}));

    int const source[]{1, 2, 3, 4};
    items.assign(source + 1, source + 3);
    CHECK(std::ranges::equal(items, std::array{2, 3}));
    std::istringstream input("5 6 7");
    using Iterator = std::istream_iterator<int>;
    items.assign(Iterator{input}, Iterator{});
    CHECK(std::ranges::equal(items, std::array{5, 6, 7}));

    items.assign({8, 9});
    CHECK(std::ranges::equal(items, std::array{8, 9}));
    CHECK(&(items = {1, 2, 3}) == &items);
    CHECK(std::ranges::equal(items, std::array{1, 2, 3}));
    items.assign(source, source);
    CHECK(items.empty());
}

TEST_CASE("vector.copy_is_independent")
{
    bys::vector<std::string> const source{"first", "second"};
    auto copy = source;
    copy.front() = "changed";
    CHECK_EQ(source.front(), "first");
    CHECK_EQ(copy.back(), "second");

    bys::vector<std::string> target{"old", "values", "extra"};
    CHECK(&(target = source) == &target);
    CHECK_EQ(target.size(), source.size());
    target.back() = "different";
    CHECK_EQ(source.back(), "second");
    auto const& self = target;
    target = self;
    CHECK_EQ(target.back(), "different");
}

TEST_CASE("vector.access_and_iterators")
{
    Vector items{4, 1, 3};
    auto const& view = items;
    STATIC_CHECK(std::is_same_v<decltype(items[0]), int&> && std::is_same_v<decltype(view[0]), int const&>
        && std::is_same_v<decltype(items.at(0)), int&> && std::is_same_v<decltype(view.at(0)), int const&>
        && std::is_same_v<decltype(items.front()), int&> && std::is_same_v<decltype(view.front()), int const&>
        && std::is_same_v<decltype(items.back()), int&> && std::is_same_v<decltype(view.back()), int const&>
        && std::is_same_v<decltype(items.data()), int*> && std::is_same_v<decltype(view.data()), int const*>,
        "element access must preserve constness and return references or pointers");
    STATIC_CHECK(noexcept(view.data()) && noexcept(items.data()) && noexcept(view.size()) && noexcept(view.capacity())
        && noexcept(view.empty()) && noexcept(view.max_size()) && noexcept(view.get_allocator())
        && noexcept(items.begin()) && noexcept(view.begin()) && noexcept(items.end()) && noexcept(view.end())
        && noexcept(items.cbegin()) && noexcept(items.cend())
        && noexcept(items.rbegin()) && noexcept(view.rbegin()) && noexcept(items.rend()) && noexcept(view.rend())
        && noexcept(items.crbegin()) && noexcept(items.crend()), "capacity, data and iterator queries must be noexcept");

    std::ranges::sort(items);
    items.at(1) = 2;
    items.back() = 5;
    CHECK_EQ(view.front(), 1);
    CHECK_EQ(view[1], 2);
    CHECK_EQ(view.at(2), 5);
    CHECK(items.data() == std::to_address(items.begin()));
    CHECK(view.data() == std::to_address(view.begin()));
    CHECK(view.data() + 2 == std::addressof(view.back()));
    CHECK(items.cbegin() == view.begin());
    CHECK(items.cend() == view.end());
    CHECK_EQ(view.end() - view.begin(), 3);
    CHECK(items.crbegin() == view.rbegin());
    CHECK(items.crend() == view.rend());
    CHECK_EQ(*items.rbegin(), 5);
    CHECK_EQ(view.rend() - view.rbegin(), 3);
    CHECK_THROWS_AS(items.at(items.size()), std::out_of_range);
    CHECK_THROWS_AS(view.at(std::numeric_limits<Vector::size_type>::max()), std::out_of_range);
}

TEST_CASE("vector.lexicographic_comparison")
{
    Vector const left{1, 2, 9};
    Vector const right{1, 3, 0};
    CHECK(left == Vector{1, 2, 9});
    CHECK(left != right);
    CHECK(left < right && left <= right && right > left && right >= left);
    CHECK((left <=> right) == std::strong_ordering::less);
    CHECK((left <=> Vector{1, 2}) == std::strong_ordering::greater);
    CHECK((left <=> Vector{1, 2, 9}) == std::strong_ordering::equal);
    CHECK(Vector{} < left);

    bys::vector<double> const nan{std::numeric_limits<double>::quiet_NaN()};
    CHECK((nan <=> bys::vector<double>{0.0}) == std::partial_ordering::unordered);
}

TEST_CASE("vector.incomplete_element_type")
{
    Node root{1, {}};
    root.children.push_back(Node{2, {}});
    root.children.front().children.push_back(Node{3, {}});
    CHECK_EQ(root.children.front().children.front().value, 3);
}
