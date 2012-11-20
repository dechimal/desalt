#include <type_traits>
#include <desalt/require.hpp>

template<typename T,
         DESALT_REQUIRE(std::is_same<T, int>)> // or DESALT_REQUIRE_C(std::is_same<T, int>::value)
std::true_type call_if_int(T) {
    return {};
}
template<typename T,
         DESALT_REQUIRE_NOT(std::is_same<T, int>)> // or DESALT_REQUIRE_C(!std::is_same<T, int>::value)
std::false_type call_if_int(T) {
    return {};
}

template<typename T,
         DESALT_REQUIRE_EXPR(std::declval<T>() + std::declval<T>())>
std::true_type addable(T) {
    return {};
}

struct hoge {};

static_assert(decltype(call_if_int(1))::value, "1");
static_assert(!decltype(call_if_int(hoge{}))::value, "2");
static_assert(decltype(addable(1))::value, "3");

int main() {}
