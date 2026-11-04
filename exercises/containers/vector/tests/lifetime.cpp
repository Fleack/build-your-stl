#include <bys/vector.hpp>
#include <bys/test/test.hpp>

#include <cstdint>
#include <memory>
#include <utility>

namespace
{
struct ElementError {};

struct Counts
{
    int live = 0;
    int transfers = 0;
    int transfers_before_throw = -1;
};

struct Element
{
    Counts* counts;
    int value;

    Element(Counts& counts, int value) : counts(&counts), value(value)
    {
        ++counts.live;
    }

    Element(Element const& other) : counts(other.counts), value(other.value)
    {
        record_transfer();
    }

    Element(Element&& other) : counts(other.counts), value(other.value)
    {
        record_transfer();
        other.value = -1;
    }

    Element& operator=(Element const&) = default;
    Element& operator=(Element&&) = default;

    ~Element()
    {
        --counts->live;
    }

    void record_transfer()
    {
        if (counts->transfers_before_throw == 0)
        {
            throw ElementError{};
        }
        if (counts->transfers_before_throw > 0)
        {
            --counts->transfers_before_throw;
        }
        ++counts->live;
        ++counts->transfers;
    }
};
} // namespace

TEST_CASE("vector.element_lifetime")
{
    Counts counts;
    {
        bys::vector<Element> items;
        items.reserve(8);
        CHECK_EQ(counts.live, 0);
        items.emplace_back(counts, 1);
        items.emplace_back(counts, 2);
        items.emplace_back(counts, 3);
        CHECK_EQ(counts.live, 3);
        items.erase(items.cbegin() + 1);
        CHECK_EQ(counts.live, 2);
        CHECK_EQ(items.back().value, 3);
        items.pop_back();
        CHECK_EQ(counts.live, 1);
        items.clear();
        CHECK_EQ(counts.live, 0);
        items.emplace_back(counts, 4);
        CHECK_EQ(counts.live, 1);
    }
    CHECK_EQ(counts.live, 0);
}

TEST_CASE("vector.move_only_elements")
{
    bys::vector<std::unique_ptr<int>> source;
    source.push_back(std::make_unique<int>(1));
    source.insert(source.cbegin(), std::make_unique<int>(2));
    auto const original = source.front().get();
    source.reserve(source.capacity() + 4);
    CHECK(source.front().get() == original);
    auto const storage = source.data();

    auto moved = std::move(source);
    CHECK(moved.data() == storage);
    REQUIRE(moved.size() == 2 && moved.front() && moved.back());
    CHECK_EQ(*moved.front(), 2);
    CHECK_EQ(*moved.back(), 1);
    source = std::move(moved);
    CHECK(source.data() == storage);
    source.erase(source.cbegin());
    REQUIRE(source.size() == 1 && source.front());
    CHECK_EQ(*source.front(), 1);
}

TEST_CASE("vector.constructor_rolls_back")
{
    Counts counts;
    Element const seed(counts, 7);
    counts.transfers_before_throw = 1;
    CHECK_THROWS_AS((bys::vector<Element>(3, seed)), ElementError);
    CHECK_EQ(counts.live, 1);
    CHECK_EQ(seed.value, 7);
}

TEST_CASE("vector.reserve_rolls_back")
{
    Counts counts;
    bys::vector<Element> items;
    items.reserve(2);
    items.emplace_back(counts, 1);
    items.emplace_back(counts, 2);
    auto const data = items.data();
    auto const capacity = items.capacity();

    counts.transfers_before_throw = 1;
    CHECK_THROWS_AS(items.reserve(capacity + 1), ElementError);
    REQUIRE(items.size() == 2);
    CHECK_EQ(items.capacity(), capacity);
    CHECK(items.data() == data);
    CHECK_EQ(items.front().value, 1);
    CHECK_EQ(items.back().value, 2);
    CHECK_EQ(counts.live, 2);
}

TEST_CASE("vector.append_rolls_back_on_growth")
{
    Counts counts;
    Element const seed(counts, 7);
    bys::vector<Element> items;
    items.reserve(2);
    while (items.size() < items.capacity())
    {
        items.emplace_back(counts, 3);
    }
    auto const data = items.data();
    auto const size = items.size();
    auto const capacity = items.capacity();
    int const live_before = counts.live;

    counts.transfers_before_throw = 1;
    CHECK_THROWS_AS(items.push_back(seed), ElementError);
    REQUIRE(items.size() == size);
    CHECK_EQ(items.capacity(), capacity);
    CHECK(items.data() == data);
    CHECK_EQ(items.front().value, 3);
    CHECK_EQ(items.back().value, 3);
    CHECK_EQ(counts.live, live_before);
}

TEST_CASE("vector.amortized_growth")
{
    constexpr int count = 1024;
    Counts counts;
    {
        bys::vector<Element> items;
        for (int value = 0; value < count; ++value)
        {
            items.emplace_back(counts, value);
        }
        CHECK_EQ(items.size(), static_cast<bys::vector<Element>::size_type>(count));
        CHECK_EQ(counts.live, count);
        // Allow different growth factors while catching reallocation on every append.
        CHECK(counts.transfers < 16 * count);
        int expected = 0;
        for (auto const& element : items)
        {
            REQUIRE(element.value == expected);
            ++expected;
        }
    }
    CHECK_EQ(counts.live, 0);
}

TEST_CASE("vector.overaligned_elements")
{
    struct alignas(64) Aligned
    {
        int value;
    };

    bys::vector<Aligned> items;
    items.emplace_back(Aligned{7});
    items.reserve(items.capacity() + 8);
    CHECK(reinterpret_cast<std::uintptr_t>(items.data()) % alignof(Aligned) == 0);
    CHECK_EQ(items.front().value, 7);
}
