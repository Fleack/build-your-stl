#include <bys/test/test.hpp>

#include <type_traits>

STATIC_CHECK(std::is_same_v<decltype(bys::test::TestCase::function), void (*)()>, "TestCase must store a function pointer");
STATIC_CHECK(std::is_same_v<int, int>, "Template commas must be accepted");
STATIC_CHECK(2 + 2 == 4, "Constant expressions must be checked at compile time");
STATIC_CHECK(noexcept(std::declval<const std::string_view&>().size()), "string_view::size must be noexcept");
