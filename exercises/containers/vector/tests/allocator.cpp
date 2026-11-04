#include <bys/vector.hpp>
#include <bys/test/test.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace
{
struct AllocationState
{
    std::ptrdiff_t live_slots = 0;
    int live_elements = 0;
    bool fail_allocation = false;
    AllocationState* copy_target = nullptr;
};

template <class T, bool Propagate = false>
struct TrackingAllocator
{
    using value_type = T;
    using propagate_on_container_copy_assignment = std::bool_constant<Propagate>;
    using propagate_on_container_move_assignment = std::bool_constant<Propagate>;
    using propagate_on_container_swap = std::bool_constant<Propagate>;
    using is_always_equal = std::false_type;

    template <class U>
    struct rebind
    {
        using other = TrackingAllocator<U, Propagate>;
    };

    AllocationState* state;

    explicit TrackingAllocator(AllocationState& state) noexcept : state(&state) {}

    template <class U>
    TrackingAllocator(TrackingAllocator<U, Propagate> const& other) noexcept : state(other.state) {}

    T* allocate(std::size_t count)
    {
        if (state->fail_allocation)
        {
            throw std::bad_alloc{};
        }
        auto* data = std::allocator<T>{}.allocate(count);
        state->live_slots += static_cast<std::ptrdiff_t>(count);
        return data;
    }

    void deallocate(T* data, std::size_t count) noexcept
    {
        state->live_slots -= static_cast<std::ptrdiff_t>(count);
        std::allocator<T>{}.deallocate(data, count);
    }

    template <class U, class... Args>
    void construct(U* address, Args&&... args)
    {
        std::construct_at(address, std::forward<Args>(args)...);
        ++state->live_elements;
    }

    template <class U>
    void destroy(U* address)
    {
        std::destroy_at(address);
        --state->live_elements;
    }

    TrackingAllocator select_on_container_copy_construction() const noexcept
    {
        return TrackingAllocator{state->copy_target ? *state->copy_target : *state};
    }

    template <class U>
    bool operator==(TrackingAllocator<U, Propagate> const& other) const noexcept
    {
        return state == other.state;
    }
};
} // namespace

TEST_CASE("vector.allocator_construction_and_lifetime")
{
    AllocationState state;
    using Alloc = TrackingAllocator<int>;
    using Vector = bys::vector<int, Alloc>;
    Alloc const allocator(state);
    STATIC_CHECK(std::is_same_v<decltype(bys::vector(std::declval<int*>(), std::declval<int*>(), allocator)), Vector>,
        "iterator-pair deduction must retain the explicit allocator type");

    {
        Vector items(allocator);
        items.reserve(8);
        CHECK(items.get_allocator() == allocator);
        CHECK(state.live_slots >= 8);
        CHECK_EQ(state.live_elements, 0);

        Vector const default_inserted(2, allocator);
        Vector const filled(2, 7, allocator);
        int const values[]{3, 5};
        Vector const range(values, values + 2, allocator);
        Vector const listed({9, 11}, allocator);
        items.emplace_back(13);
        CHECK_EQ(state.live_elements, 9);
        CHECK(std::ranges::equal(default_inserted, std::array{0, 0}));
        CHECK(std::ranges::equal(filled, std::array{7, 7}));
        CHECK(std::ranges::equal(range, std::array{3, 5}));
        CHECK(std::ranges::equal(listed, std::array{9, 11}));
        items.clear();
        CHECK_EQ(state.live_elements, 8);
    }
    CHECK_EQ(state.live_elements, 0);
    CHECK_EQ(state.live_slots, 0);
}

TEST_CASE("vector.allocator_copy_selection")
{
    AllocationState source_state;
    AllocationState selected_state;
    AllocationState target_state;
    source_state.copy_target = &selected_state;
    using Alloc = TrackingAllocator<int>;
    using Vector = bys::vector<int, Alloc>;
    Alloc const source_allocator(source_state);
    Alloc const target_allocator(target_state);
    {
        Vector const source({1, 2}, source_allocator);
        Vector const selected(source);
        Vector const explicit_copy(source, target_allocator);
        Vector assigned({9}, target_allocator);
        assigned = source;

        CHECK(selected.get_allocator() == Alloc(selected_state));
        CHECK(explicit_copy.get_allocator() == target_allocator);
        CHECK(assigned.get_allocator() == target_allocator);
        CHECK(std::ranges::equal(selected, source));
        CHECK(std::ranges::equal(explicit_copy, source));
        CHECK(std::ranges::equal(assigned, source));
    }
    CHECK_EQ(source_state.live_slots, 0);
    CHECK_EQ(selected_state.live_slots, 0);
    CHECK_EQ(target_state.live_slots, 0);
}

TEST_CASE("vector.allocator_moves_with_unequal_allocators")
{
    AllocationState source_state;
    AllocationState target_state;
    using Alloc = TrackingAllocator<std::unique_ptr<int>>;
    using Vector = bys::vector<std::unique_ptr<int>, Alloc>;
    Alloc const source_allocator(source_state);
    Alloc const target_allocator(target_state);
    STATIC_CHECK(!std::is_nothrow_move_assignable_v<Vector>,
        "move assignment may allocate when allocators do not propagate or always compare equal");

    {
        Vector source(source_allocator);
        source.push_back(std::make_unique<int>(7));
        Vector moved(std::move(source), target_allocator);
        CHECK(moved.get_allocator() == target_allocator);
        REQUIRE(moved.size() == 1 && moved.front());
        CHECK_EQ(*moved.front(), 7);

        Vector assigned(source_allocator);
        assigned.push_back(std::make_unique<int>(9));
        assigned = std::move(moved);
        CHECK(assigned.get_allocator() == source_allocator);
        REQUIRE(assigned.size() == 1 && assigned.front());
        CHECK_EQ(*assigned.front(), 7);
    }
    CHECK_EQ(source_state.live_slots, 0);
    CHECK_EQ(target_state.live_slots, 0);
    CHECK_EQ(source_state.live_elements, 0);
    CHECK_EQ(target_state.live_elements, 0);
}

TEST_CASE("vector.allocator_propagation")
{
    AllocationState left_state;
    AllocationState right_state;
    using Alloc = TrackingAllocator<int, true>;
    using Vector = bys::vector<int, Alloc>;
    Alloc const left_allocator(left_state);
    Alloc const right_allocator(right_state);
    STATIC_CHECK(std::is_nothrow_move_assignable_v<Vector>, "propagating move assignment must be noexcept");

    {
        Vector left({1, 2}, left_allocator);
        Vector right({3, 4, 5}, right_allocator);
        STATIC_CHECK(noexcept(left.swap(right)), "propagating swap must be noexcept");
        left.swap(right);
        CHECK(left.get_allocator() == right_allocator);
        CHECK(right.get_allocator() == left_allocator);
        CHECK(std::ranges::equal(left, std::array{3, 4, 5}));
        CHECK(std::ranges::equal(right, std::array{1, 2}));

        left = right;
        CHECK(left.get_allocator() == left_allocator);
        CHECK(std::ranges::equal(left, right));
        Vector destination({9}, right_allocator);
        auto const storage = left.data();
        destination = std::move(left);
        CHECK(destination.get_allocator() == left_allocator);
        CHECK(destination.data() == storage);
        CHECK(std::ranges::equal(destination, std::array{1, 2}));
    }
    CHECK_EQ(left_state.live_slots, 0);
    CHECK_EQ(right_state.live_slots, 0);
    CHECK_EQ(left_state.live_elements, 0);
    CHECK_EQ(right_state.live_elements, 0);
}

TEST_CASE("vector.allocator_failure")
{
    AllocationState state;
    using Alloc = TrackingAllocator<int>;
    bys::vector<int, Alloc> items({1, 2, 3}, Alloc(state));
    auto const data = items.data();
    auto const capacity = items.capacity();
    auto const slots = state.live_slots;

    state.fail_allocation = true;
    CHECK_THROWS_AS(items.reserve(capacity + 1), std::bad_alloc);
    CHECK(items.data() == data);
    CHECK_EQ(items.capacity(), capacity);
    CHECK(std::ranges::equal(items, std::array{1, 2, 3}));
    CHECK_EQ(state.live_slots, slots);
    CHECK_EQ(state.live_elements, 3);
}
