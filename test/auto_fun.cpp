
#include <type_traits>
#include <desalt/auto_fun.hpp>

template<typename T>
DESALT_AUTO_FUN_NOEXCEPT(f(T a, T b), a + b);
template<typename T>
DESALT_AUTO_FUN_NOEXCEPT(g(T, T), throw 1);

static_assert(noexcept(::f(1, 2)), "1");
static_assert(!noexcept(::g(1, 2)), "2");
static_assert(std::is_same<decltype(::f(1, 2)), int>::value, "3");

int main() {}
