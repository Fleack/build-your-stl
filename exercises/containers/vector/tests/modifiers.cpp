#include <bys/vector.hpp>
#include <bys/test/test.hpp>

#include <algorithm>
#include <array>
#include <iterator>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace
{
using Vector = bys::vector<int>;
} // namespace

TEST_CASE("vector.reserve_and_shrink")
{
    Vector items{1, 2, 3};
    items.reserve(items.capacity() + 32);
    auto const capacity = items.capacity();
    auto const first = items.begin();
    auto const last = items.end();
    auto const data = items.data();
    CHECK(capacity >= 35);
    CHECK(std::ranges::equal(items, std::array{1, 2, 3}));

    items.reserve(0);
    items.reserve(capacity);
    REQUIRE(items.data() == data);
    CHECK_EQ(items.capacity(), capacity);
    CHECK(items.begin() == first);
    CHECK(items.end() == last);
    items.push_back(4);
    REQUIRE(items.data() == data);
    CHECK_EQ(*first, 1);
    CHECK_EQ(items.capacity(), capacity);

    items.shrink_to_fit();
    CHECK(items.capacity() >= items.size() && items.capacity() <= capacity);
    CHECK(std::ranges::equal(items, std::array{1, 2, 3, 4}));
}

TEST_CASE("vector.reserve_past_max_size")
{
    Vector items{1, 2, 3};
    REQUIRE(items.max_size() < std::numeric_limits<Vector::size_type>::max());
    auto const capacity = items.capacity();
    auto const data = items.data();
    CHECK_THROWS_AS(items.reserve(items.max_size() + 1), std::length_error);
    CHECK_EQ(items.capacity(), capacity);
    CHECK(items.data() == data);
    CHECK(std::ranges::equal(items, std::array{1, 2, 3}));
}

TEST_CASE("vector.resize_and_clear")
{
    Vector items{1, 2};
    items.resize(4);
    CHECK(std::ranges::equal(items, std::array{1, 2, 0, 0}));
    items.resize(6, 9);
    CHECK(std::ranges::equal(items, std::array{1, 2, 0, 0, 9, 9}));
    auto const capacity = items.capacity();
    items.resize(3);
    CHECK(std::ranges::equal(items, std::array{1, 2, 0}));
    CHECK_EQ(items.capacity(), capacity);
    items.clear();
    CHECK(items.empty());
    CHECK_EQ(items.capacity(), capacity);
    items.emplace_back(7);
    CHECK_EQ(items.back(), 7);
}

TEST_CASE("vector.insertion_overloads")
{
    Vector items{1, 6};
    items.reserve(16);
    auto const first = items.begin();
    int const value = 2;
    CHECK_EQ(items.insert(items.cbegin() + 1, value), items.begin() + 1);
    CHECK_EQ(items.insert(items.cbegin() + 2, 3), items.begin() + 2);
    CHECK_EQ(items.insert(items.cbegin() + 3, 2, 4), items.begin() + 3);
    CHECK_EQ(items.insert(items.cbegin() + 5, {5}), items.begin() + 5);
    CHECK_EQ(items.insert(items.cend(), 0, 99), items.end());

    int const tail[]{7, 8};
    CHECK_EQ(items.insert(items.cend(), tail, tail + 2), items.begin() + 7);
    CHECK_EQ(items.insert(items.cend(), tail, tail), items.end());
    CHECK_EQ(items.insert(items.cend(), {}), items.end());
    REQUIRE(items.begin() == first);
    CHECK_EQ(*first, 1);
    CHECK(std::ranges::equal(items, std::array{1, 2, 3, 4, 4, 5, 6, 7, 8}));
}

TEST_CASE("vector.single_pass_insert")
{
    Vector items{1, 5};
    std::istringstream input("2 3 4");
    using Iterator = std::istream_iterator<int>;
    CHECK_EQ(items.insert(items.cbegin() + 1, Iterator{input}, Iterator{}), items.begin() + 1);
    CHECK(std::ranges::equal(items, std::array{1, 2, 3, 4, 5}));
}

TEST_CASE("vector.append_and_insert_aliased_value")
{
    bys::vector<std::string> items{"first", "second"};
    items.resize(items.capacity(), "padding");
    auto const previous_capacity = items.capacity();
    items.push_back(items.front());
    CHECK(items.capacity() > previous_capacity);
    CHECK_EQ(items.front(), "first");
    CHECK_EQ(items.back(), "first");

    items.reserve(items.size() + 2);
    items.insert(items.cbegin(), 2, items[1]);
    CHECK_EQ(items[0], "second");
    CHECK_EQ(items[1], "second");
    CHECK_EQ(items[2], "first");
    CHECK_EQ(items[3], "second");
}

TEST_CASE("vector.emplace_and_erase")
{
    bys::vector<std::string> items{"left", "right"};
    items.reserve(8);
    auto& appended = items.emplace_back(3, 'z');
    CHECK(&appended == &items.back());
    CHECK_EQ(appended, "zzz");
    auto const middle = items.emplace(items.cbegin() + 1, 2, 'x');
    REQUIRE(middle == items.begin() + 1);
    CHECK_EQ(*middle, "xx");
    CHECK_EQ(items.erase(items.cbegin() + 1), items.begin() + 1);
    CHECK_EQ(items[1], "right");
    CHECK_EQ(items.erase(items.cbegin() + 1, items.cbegin() + 1), items.begin() + 1);
    CHECK_EQ(items.size(), 3u);
    CHECK_EQ(items.erase(items.cbegin() + 1, items.cend()), items.end());
    CHECK_EQ(items.back(), "left");
    items.pop_back();
    CHECK(items.empty());
}

TEST_CASE("vector.free_erase")
{
    Vector items{1, 2, 3, 2, 4, 5, 4};
    STATIC_CHECK(std::is_same_v<decltype(bys::erase(items, 2L)), Vector::size_type>
        && std::is_same_v<decltype(bys::erase_if(items, [](int) { return true; })), Vector::size_type>,
        "erase and erase_if must return the number erased as size_type");
    CHECK_EQ(bys::erase(items, 2L), 2u);
    CHECK(std::ranges::equal(items, std::array{1, 3, 4, 5, 4}));
    CHECK_EQ(bys::erase_if(items, [](int value) { return value % 2 == 0; }), 2u);
    CHECK(std::ranges::equal(items, std::array{1, 3, 5}));
    CHECK_EQ(bys::erase(items, 8), 0u);
    CHECK_EQ(bys::erase_if(items, [](int) { return true; }), 3u);
    CHECK(items.empty());
}

TEST_CASE("vector.swap_preserves_iterators")
{
    Vector left{1, 2};
    Vector right{3, 4, 5};
    left.reserve(10);
    right.reserve(20);
    auto const left_first = left.begin();
    auto const right_first = right.begin();
    auto const left_capacity = left.capacity();
    auto const right_capacity = right.capacity();

    STATIC_CHECK(noexcept(left.swap(right)) && noexcept(bys::swap(left, right)),
        "swap must be noexcept for the default allocator");
    left.swap(right);
    CHECK(left.begin() == right_first);
    CHECK(right.begin() == left_first);
    CHECK_EQ(left.capacity(), right_capacity);
    CHECK_EQ(right.capacity(), left_capacity);
    CHECK(std::ranges::equal(left, std::array{3, 4, 5}));

    bys::swap(left, right);
    CHECK(left.begin() == left_first);
    CHECK(right.begin() == right_first);
    CHECK(std::ranges::equal(left, std::array{1, 2}));
}
