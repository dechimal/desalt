#include <iostream>
#include <typeinfo>
#include <type_traits>
#include <desalt/static_control.hpp>
#include <desalt/static_control/macro.hpp>
#include <cassert>

using desalt::static_control::static_match;
using desalt::static_control::static_if;
using desalt::static_control::depend;

template<typename T>
bool f(T const &) {
    // function version
    ::static_match<T>([] (auto x, typename decltype(x)::type::type = {}) {
        std::cout << "has nested type: " << typeid(typename decltype(x)::type::type).name() << "." << std::endl;
    }, [] (auto) {
        std::cout << "does not have nested type." << std::endl;
    });

    // macro version
    return DESALT_STATIC_MATCH(T) DESALT_CASE(auto x, if typename decltype(x)::type::type) {
        return true;
    } DESALT_DEFAULT {
        return false;
    };
}


struct hoge { using type = int; };

template<typename T>
auto has_type_v0 = ::static_match<T>(
    [] (auto x, typename decltype(x)::type::type = {}) { return std::true_type{}; },
    [] (auto) { return std::false_type{}; });

template<typename T>
constexpr auto has_type_v = decltype(has_type_v0<T>){};

int main() {
    assert(::f(hoge{}));
    assert(!::f(0));
    static_assert(has_type_v<hoge>);
    static_assert(!has_type_v<int>);

    ::static_if([] (auto dep, typename depend<hoge, dep>::type = {}) {
        static_assert(true);
    }, [] (auto dep) {
        static_assert(sizeof(typename depend<int, dep>::type) - sizeof(int));
    });

    ::static_if([] (auto dep, typename depend<int, dep>::type = {}) {
        static_assert(sizeof(typename depend<int, dep>::type) - sizeof(int));
    });

    DESALT_STATIC_MATCH(int) DESALT_CASE(auto x, if decltype(std::declval<typename decltype(x)::type&>()++)) {
        static_assert(true);
    } DESALT_CASE(auto x) {
        static_assert(sizeof(decltype(x)::type) == sizeof(int));
    };
}
