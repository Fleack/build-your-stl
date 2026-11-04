#include <bys/array.hpp>
#include <bys/test/test.hpp>

#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace
{
using Array = bys::array<int, 2>;
} // namespace

STATIC_CHECK(std::is_same_v<decltype(bys::array{1}), bys::array<int, 1>>
    && std::is_same_v<decltype(bys::array{1, 2}), Array>, "deduction must preserve the element type and infer the size");
STATIC_CHECK(std::tuple_size_v<Array> == 2 && std::tuple_size_v<Array const> == 2
    && std::tuple_size_v<bys::array<int, 0>> == 0, "tuple_size must match the array size");
STATIC_CHECK(std::is_same_v<std::tuple_element_t<1, Array>, int>
    && std::is_same_v<std::tuple_element_t<1, Array const>, int const>, "tuple_element must preserve the element type and constness");

STATIC_CHECK(std::is_same_v<decltype(bys::get<0>(std::declval<Array&>())), int&>
    && std::is_same_v<decltype(bys::get<0>(std::declval<Array const&>())), int const&>
    && std::is_same_v<decltype(bys::get<0>(std::declval<Array&&>())), int&&>
    && std::is_same_v<decltype(bys::get<0>(std::declval<Array const&&>())), int const&&>,
    "get must preserve constness and value category");
STATIC_CHECK(noexcept(bys::get<0>(std::declval<Array&>())) && noexcept(bys::get<0>(std::declval<Array const&>()))
    && noexcept(bys::get<0>(std::declval<Array&&>())) && noexcept(bys::get<0>(std::declval<Array const&&>())),
    "all get overloads must be noexcept");

STATIC_CHECK([] {
    int const source[]{4, 5};
    auto items = bys::to_array(source);
    STATIC_CHECK(std::is_same_v<decltype(items), Array>, "to_array must remove top-level const from the element type");
    auto& [first, second] = items;
    first = 7;
    return source[0] == 4 && bys::get<0>(items) == 7 && second == 5 && bys::get<1>(std::as_const(items)) == 5;
}(), "to_array, get and structured bindings must work in constant expressions");

TEST_CASE("array.get_and_structured_bindings")
{
    Array items{1, 2};
    bys::get<1>(items) = 9;
    auto& [first, second] = items;
    first = 7;
    auto const& [const_first, const_second] = items;
    CHECK_EQ(bys::get<0>(items), 7);
    CHECK_EQ(second, 9);
    CHECK_EQ(const_first, 7);
    CHECK_EQ(const_second, 9);
    CHECK(std::addressof(bys::get<0>(items)) == std::addressof(first));

    bys::array<std::unique_ptr<int>, 1> owner{std::make_unique<int>(42)};
    auto moved = bys::get<0>(std::move(owner));
    CHECK(bys::get<0>(owner) == nullptr);
    REQUIRE(moved != nullptr);
    CHECK_EQ(*moved, 42);
}

TEST_CASE("array.to_array_copies_lvalue")
{
    std::string source[]{"first", "second"};
    auto copy = bys::to_array(source);
    copy.front() = "changed";
    CHECK_EQ(source[0], "first");
    CHECK_EQ(copy.back(), "second");

    constexpr auto text = bys::to_array("hi");
    STATIC_CHECK(std::is_same_v<decltype(text), bys::array<char, 3> const>, "to_array must preserve the full extent of a string literal");
    CHECK(text == bys::array<char, 3>{'h', 'i', '\0'});
}

TEST_CASE("array.to_array_moves_rvalue")
{
    std::unique_ptr<int> source[]{std::make_unique<int>(1), std::make_unique<int>(2)};
    auto moved = bys::to_array(std::move(source));
    STATIC_CHECK(std::is_same_v<decltype(moved), bys::array<std::unique_ptr<int>, 2>>, "to_array must preserve the element type and extent");
    CHECK(source[0] == nullptr);
    CHECK(source[1] == nullptr);
    REQUIRE(moved.front() != nullptr && moved.back() != nullptr);
    CHECK_EQ(*moved.front(), 1);
    CHECK_EQ(*moved.back(), 2);
}
