#include <type_traits>

#include <bys/integral_constant.hpp>
#include <bys/test/test.hpp>

namespace
{
using Negative = bys::integral_constant<int, -7>;

enum class State
{
    ready
};

using Ready = bys::integral_constant<State, State::ready>;
} // namespace

STATIC_CHECK(Negative::value == -7);
STATIC_CHECK(std::is_same_v<decltype(Negative::value), int const>);
STATIC_CHECK(std::is_same_v<Negative::value_type, int>);
STATIC_CHECK(std::is_same_v<Negative::type, Negative>);
STATIC_CHECK(!std::is_same_v<Negative, bys::integral_constant<int, 7>>);
STATIC_CHECK(!std::is_same_v<Negative, bys::integral_constant<long, -7>>);

STATIC_CHECK(std::is_convertible_v<Negative const&, int>);
STATIC_CHECK(noexcept(static_cast<int>(Negative{})));
STATIC_CHECK(noexcept(Negative{}()));
STATIC_CHECK(std::is_same_v<decltype(Negative{}()), int>);
STATIC_CHECK([] {
    Negative const constant{};
    int const converted = constant;
    return converted == -7 && constant() == -7;
}());

STATIC_CHECK(std::is_same_v<Ready::value_type, State>);
STATIC_CHECK(Ready::value == State::ready);
STATIC_CHECK(static_cast<State>(Ready{}) == State::ready);
STATIC_CHECK(Ready{}() == State::ready);

STATIC_CHECK(std::is_same_v<bys::bool_constant<true>, bys::integral_constant<bool, true>>);
STATIC_CHECK(std::is_same_v<bys::bool_constant<false>, bys::integral_constant<bool, false>>);
STATIC_CHECK(std::is_same_v<bys::true_type, bys::bool_constant<true>>);
STATIC_CHECK(std::is_same_v<bys::false_type, bys::bool_constant<false>>);

TEST_CASE("integral_constant.boolean_aliases")
{
    CHECK(bys::true_type{}());
    CHECK_FALSE(bys::false_type{}());
}
