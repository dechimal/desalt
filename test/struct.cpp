
#include <desalt/struct.hpp>
#include <vector>
#include <cassert>
#include <type_traits>

auto a0 = std::integral_constant<std::size_t, 0>{};
auto a1 = std::integral_constant<std::size_t, 1>{};
auto a2 = std::integral_constant<std::size_t, 2>{};

template<typename T> void print[[deprecated]]() {}

int main() {

    int a = 42;
    auto b = std::vector<int>{1, 2, 3};
    auto v = DESALT_STRUCT(x, &y, &&z)(1, a, std::move(b));
    auto const & cr = v;
    a = 50;

    static_assert(std::is_same<decltype(v.x), int>{}, "1");
    static_assert(std::is_same<decltype(v.y), int&>{}, "2");
    static_assert(std::is_same<decltype(v.z), std::vector<int>&&>{}, "3");
    static_assert(std::is_same<decltype(v.get(a0)), int&>{}, "4");
    static_assert(std::is_same<decltype(v.get(a1)), int&>{}, "5");
    static_assert(std::is_same<decltype(v.get(a2)), std::vector<int>&>{}, "6");
    static_assert(std::is_same<decltype(std::move(v).get(a0)), int&&>{}, "7");
    static_assert(std::is_same<decltype(std::move(v).get(a1)), int&>{}, "8");
    static_assert(std::is_same<decltype(std::move(v).get(a2)), std::vector<int>&&>{}, "9");
    static_assert(std::is_same<decltype(cr.get(a0)), int const &>{}, "10");
    static_assert(std::is_same<decltype(cr.get(a1)), int const &>{}, "11");
    static_assert(std::is_same<decltype(cr.get(a2)), std::vector<int> const &>{}, "12");
    static_assert(std::is_same<decltype(std::move(cr).get(a0)), int const &&>{}, "13");
    static_assert(std::is_same<decltype(std::move(cr).get(a1)), int const &>{}, "14");
    static_assert(std::is_same<decltype(std::move(cr).get(a2)), std::vector<int> const &&>{}, "15");

    assert(v.x == 1);
    assert(v.y == 50);
    assert(v.z == (std::vector<int>{1,2,3}));

    assert(v.x == desalt::get<0>(v));
    assert(v.y == desalt::get<1>(v));
    assert(v.z == desalt::get<2>(v));
}
