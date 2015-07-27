#include <iostream>
#include <tuple>
#include <typeinfo>
#include <type_traits>
#include <desalt/static_control.hpp>
#include <cassert>

namespace sc = desalt::static_control;

using sc::static_match;
using sc::static_if;
using sc::static_while;
using sc::static_for;
using sc::depend;

template<typename T>
bool f(T const & x) {
    // Nth parameter of each clause is nth argument of static_match.
    // `typename decltype(x)::type = {}` is test that type of `x` has nested type named `type`.
    // if type of `x` has nested type, then first clause is instanciate and execute.
    return ::static_match(x)([] (auto x, typename decltype(x)::type = {}) {
        std::cerr << "has nested type: " << typeid(typename decltype(x)::type).name() << "." << std::endl;
        return true;
    }, [] (auto) {
        std::cerr << "does not have nested type." << std::endl;
        return false;
    });
}


struct hoge { using type = int; };

// This example is to make a metafunction using generic lambda.
// Non constant expression `has_type_v0` makes constant expression `has_type_v`.
// Class template `wrap` carries template parameter `T`. `wrap<T>::type` is `T`.
template<typename T>
auto has_type_impl = ::static_match(sc::wrap<T>{})(
    [] (auto x, typename decltype(x)::type::type = {}) { return std::true_type{}; },
    [] (auto) { return std::false_type{}; });

template<typename T>
constexpr auto has_type_v = decltype(has_type_impl<T>){};

int main() {
    assert(::f(hoge{}));
    assert(!::f(0));
    static_assert(has_type_v<hoge>);
    static_assert(!has_type_v<int>);

    // In `static_if`, first argument of each clauses named "dep" makes a dependency
    // and prevent to instanciate when the clause is not selected.
    // `dep(expr)` equals `expr` but first is dependent expression.
    // `depent<type, dep>` equals `type` but first is dependent type.
    ::static_if([] (auto dep, typename depend<hoge, dep>::type = {}) {
        static_assert(true);
    }, [] (auto dep) {
        static_assert(sizeof(typename depend<int, dep>::type) - sizeof(int));
    });

    ::static_if([] (auto dep, typename depend<int, dep>::type = {}) {
        static_assert(sizeof(typename depend<int, dep>::type) - sizeof(int));
    });

    ::static_match(sc::wrap<int>{})([&] (auto x, sc::wrap<decltype(std::declval<typename decltype(x)::type&>()++)>* = {}) {
        static_assert(true);
    }, [&] (auto x) {
        static_assert(sizeof(typename decltype(x)::type) - sizeof(int));
    });

    // `static_for(x0, x1, ... xn, f)` is used to compile-time loop.
    // `xs...` denotes parameter pack of `static_for` (each parameter is `std::forward`-ed).
    // `f` denotes a function object.
    // `i` denotes object of `std::integral_constant<std::size_t, 0>`.
    // The behavior is,
    // - if `f(i, xs...)` is ill-formed, return type is `std::tuple<Ts...>`.
    //   `Ts...` denotes a sequence of type that is removed const and rvalue reference of `decltype(x_k)`,
    // - otherwise instanciate and execute `f(xs...)`. And perform this step with `j` and `ys...`.
    //   `j` denotes `std::integral_constant<std::size_t, i+1>`.
    //   `ys...` denotes,
    //   - if type of `f(i, xs...)` is a instance of `std::tuple`, then elements of `f(i, xs...)`,
    //   - if type of `f(i, xs...)` is `void`, then empty,
    //   - otherwise `f(i, xs...)`.
    std::tuple<int, std::string, double> tup{42, "hoge", 3.5};
    constexpr auto size = std::tuple_size<decltype(tup)>::value;
    auto n = 0;
    auto m = ::static_for([&] (auto i, std::enable_if_t<(i < size)> * = {}) {
        std::cerr << std::get<i>(tup) << std::endl;
        ++n;
    });
    assert(n == size && std::get<0>(m) == size);
}
