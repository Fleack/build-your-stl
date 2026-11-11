#include <type_traits>

#include <bys/is_same.hpp>
#include <bys/test/test.hpp>

namespace
{
struct Incomplete;
struct First
{};
struct Second
{};

using IntAlias = int;
using Array = int[3];
using Function = void(int);

template <typename T, typename U>
constexpr bool check_is_same(bool expected)
{
    return bys::is_same<T, U>::value == expected && bys::is_same_v<T, U> == expected;
}

template <typename Trait>
constexpr bool check_trait_interface(bool expected)
{
    using Constant = typename Trait::type;
    Trait const trait{};
    bool const converted = trait;
    STATIC_CHECK(std::is_same_v<typename Trait::value_type, bool>);
    STATIC_CHECK(std::is_same_v<decltype(Trait::value), bool const>);
    STATIC_CHECK(std::is_same_v<decltype(trait()), bool>);
    STATIC_CHECK(noexcept(static_cast<bool>(trait)));
    STATIC_CHECK(noexcept(trait()));
    return Constant::value == expected && converted == expected && trait() == expected;
}
} // namespace

STATIC_CHECK(check_is_same<int, IntAlias>(true));
STATIC_CHECK(check_is_same<void, void>(true));
STATIC_CHECK(check_is_same<Incomplete, Incomplete>(true));
STATIC_CHECK(check_is_same<First, Second>(false));
STATIC_CHECK(check_is_same<int, long>(false));
STATIC_CHECK(check_is_same<long, int>(false));

STATIC_CHECK(check_is_same<int const, int const>(true));
STATIC_CHECK(check_is_same<int, int const>(false));
STATIC_CHECK(check_is_same<int, int volatile>(false));
STATIC_CHECK(check_is_same<int&, int&&>(false));
STATIC_CHECK(check_is_same<int, int&>(false));
STATIC_CHECK(check_is_same<int*, int const*>(false));
STATIC_CHECK(check_is_same<int*, int* const>(false));

STATIC_CHECK(check_is_same<Array, int[3]>(true));
STATIC_CHECK(check_is_same<Array, int[4]>(false));
STATIC_CHECK(check_is_same<Array, int*>(false));
STATIC_CHECK(check_is_same<Function, void(int)>(true));
STATIC_CHECK(check_is_same<Function, void(int) noexcept>(false));
STATIC_CHECK(check_is_same<Function, Function*>(false));

STATIC_CHECK(check_trait_interface<bys::is_same<int, IntAlias>>(true));
STATIC_CHECK(check_trait_interface<bys::is_same<int, long>>(false));
STATIC_CHECK(std::is_same_v<decltype(bys::is_same_v<int, long>), bool const>);

TEST_CASE("is_same.value")
{
    CHECK((bys::is_same_v<int, IntAlias>));
    CHECK_FALSE((bys::is_same_v<int, long>));
}
