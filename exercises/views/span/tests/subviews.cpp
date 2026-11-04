#include <cstddef>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include <utility>

#include <bys/span.hpp>
#include <bys/test/test.hpp>

namespace
{
using Fixed = bys::span<int, 4>;
using Dynamic = bys::span<int>;
using ReadOnly = bys::span<int const, 4>;

template <class S>
concept HasWritableBytes = requires (S view) { bys::as_writable_bytes(view); };

template <class S>
void check_runtime_subviews(S view)
{
    for (std::size_t offset = 0; offset <= view.size(); ++offset)
    {
        auto first = view.first(offset);
        auto last = view.last(offset);
        auto tail = view.subspan(offset);
        CHECK(first.data() == view.data());
        CHECK_EQ(first.size(), offset);
        CHECK(last.data() == view.data() + (view.size() - offset));
        CHECK_EQ(last.size(), offset);
        CHECK(tail.data() == view.data() + offset);
        CHECK_EQ(tail.size(), view.size() - offset);

        auto explicit_tail = view.subspan(offset, bys::dynamic_extent);
        CHECK(explicit_tail.data() == tail.data());
        CHECK_EQ(explicit_tail.size(), tail.size());
        for (std::size_t count = 0; count <= view.size() - offset; ++count)
        {
            auto slice = view.subspan(offset, count);
            CHECK(slice.data() == view.data() + offset);
            CHECK_EQ(slice.size(), count);
        }
    }
}
} // namespace

STATIC_CHECK(std::is_same_v<decltype(std::declval<Fixed>().first<0>()), bys::span<int, 0>>&& std::is_same_v<decltype(std::declval<Dynamic>().first<2>()), bys::span<int, 2>>&& std::is_same_v<decltype(std::declval<Dynamic>().last<2>()), bys::span<int, 2>>&& std::is_same_v<decltype(std::declval<Fixed>().last<0>()), bys::span<int, 0>>&& std::is_same_v<decltype(std::declval<Fixed>().subspan<1>()), bys::span<int, 3>>&& std::is_same_v<decltype(std::declval<Fixed>().subspan<4>()), bys::span<int, 0>>&& std::is_same_v<decltype(std::declval<Dynamic>().subspan<1>()), Dynamic>&& std::is_same_v<decltype(std::declval<Dynamic>().subspan<1, 2>()), bys::span<int, 2>>&& std::is_same_v<decltype(std::declval<ReadOnly>().subspan<1, 2>()), bys::span<int const, 2>>,
    "template subviews must calculate their extent and preserve element qualifications");
STATIC_CHECK(std::is_same_v<decltype(std::declval<Fixed>().first(2)), Dynamic>&& std::is_same_v<decltype(std::declval<Fixed>().last(2)), Dynamic>&& std::is_same_v<decltype(std::declval<Fixed>().subspan(2)), Dynamic>&& std::is_same_v<decltype(std::declval<ReadOnly>().subspan(1, 2)), bys::span<int const>>,
    "runtime subviews must have dynamic extent and preserve element qualifications");

STATIC_CHECK([] {
    int items[]{1, 2, 3, 4};
    Fixed const fixed = items;
    Dynamic const dynamic = fixed;
    auto middle = dynamic.subspan<1, 2>();
    middle.front() = 8;
    return fixed.first<2>().back() == 8 && fixed.last<2>().front() == 3 && fixed.first<0>().data() == items && fixed.last<0>().data() == items + 4 && fixed.subspan<1>().size() == 3 && fixed.subspan<4>().data() == items + 4 && fixed.subspan<1, 2>().data() == items + 1 && dynamic.subspan<4>().empty() && dynamic.subspan<1>().data() == items + 1 && dynamic.subspan<1>().size() == 3 && dynamic.first<2>().back() == 8 && dynamic.last<2>().front() == 3 && fixed.first(2).back() == 8 && fixed.last(2).front() == 3 && dynamic.subspan(1, 2).back() == 3 && dynamic.subspan(1).back() == 4;
}(),
    "subviews must alias the correct part of the original sequence in constant expressions");

TEST_CASE("span.runtime_subviews")
{
    int items[]{1, 2, 3, 4};
    Fixed const fixed = items;
    Dynamic const dynamic = fixed;
    check_runtime_subviews(fixed);
    check_runtime_subviews(dynamic);

    auto middle = fixed.subspan(1, 2);
    middle.back() = 9;
    CHECK_EQ(items[2], 9);
    CHECK_EQ(dynamic.last(2).front(), 9);
}

TEST_CASE("span.empty_subviews")
{
    Dynamic const empty;
    CHECK(empty.first<0>().empty() && empty.last<0>().empty() && empty.subspan<0>().empty());
    CHECK(empty.first(0).data() == nullptr);
    CHECK(empty.last(0).data() == nullptr);
    CHECK(empty.subspan(0).data() == nullptr);

    int item = 1;
    bys::span<int, 0> const at_address{&item, std::size_t{0}};
    check_runtime_subviews(at_address);
    CHECK(at_address.subspan<0, 0>().data() == &item);
}

STATIC_CHECK(HasWritableBytes<Dynamic>&& HasWritableBytes<Fixed> && !HasWritableBytes<ReadOnly> && !HasWritableBytes<bys::span<int const>>,
    "writable bytes must require mutable elements");
STATIC_CHECK(std::is_same_v<decltype(bys::as_bytes(std::declval<ReadOnly>())), bys::span<std::byte const, 4 * sizeof(int)>>&& std::is_same_v<decltype(bys::as_bytes(std::declval<Dynamic>())), bys::span<std::byte const>>&& std::is_same_v<decltype(bys::as_writable_bytes(std::declval<Fixed>())), bys::span<std::byte, 4 * sizeof(int)>>&& std::is_same_v<decltype(bys::as_writable_bytes(std::declval<Dynamic>())), bys::span<std::byte>>,
    "byte views must preserve dynamic extent or scale a fixed extent by element size");
STATIC_CHECK(noexcept(bys::as_bytes(std::declval<ReadOnly>())) && noexcept(bys::as_writable_bytes(std::declval<Fixed>())), "byte views must be noexcept");

TEST_CASE("span.readable_bytes")
{
    std::uint32_t const items[]{0x01020304u, 0x10203040u};
    auto fixed = bys::as_bytes(bys::span{items});
    auto dynamic = bys::as_bytes(bys::span<std::uint32_t const>{items});
    CHECK(static_cast<void const*>(fixed.data()) == static_cast<void const*>(items));
    CHECK(dynamic.data() == fixed.data());
    CHECK_EQ(fixed.size(), sizeof(items));
    CHECK_EQ(dynamic.size(), sizeof(items));
    CHECK_EQ(std::memcmp(fixed.data(), items, sizeof(items)), 0);
}

TEST_CASE("span.writable_bytes")
{
    unsigned char items[]{1, 2, 3};
    auto writable = bys::as_writable_bytes(bys::span{items}.subspan(1));
    auto readable = bys::as_bytes(bys::span{items});
    REQUIRE(writable.size() == 2 && readable.size() == 3);
    writable[0] = std::byte{7};
    CHECK_EQ(items[1], 7);
    CHECK(readable[1] == std::byte{7});
    CHECK_EQ(items[0], 1);
    CHECK_EQ(items[2], 3);
}

TEST_CASE("span.empty_byte_views")
{
    auto read = bys::as_bytes(bys::span<int const>{});
    auto write = bys::as_writable_bytes(bys::span<int, 0>{});
    STATIC_CHECK(std::is_same_v<decltype(write), bys::span<std::byte, 0>>, "zero fixed extent must stay zero");
    CHECK(read.empty() && write.empty());
    CHECK(read.data() == nullptr && write.data() == nullptr);
}
