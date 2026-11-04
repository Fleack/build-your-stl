#pragma once

#include <functional>

namespace bys
{
constexpr int add(int a, int b) { return std::plus<int>{}(a, b); }
} // namespace bys
