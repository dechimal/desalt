#ifndef DESALT_MATCH_LITERALS_HPP_INCLUDED_
#define DESALT_MATCH_LITERALS_HPP_INCLUDED_

#include <desalt/match/fwd.hpp>

namespace desalt::match {
namespace detail {

template<char ...Cs, std::size_t ...Is>
constexpr auto pat_impl(std::index_sequence<Is...>) {
    constexpr auto n = sizeof...(Cs);
    constexpr auto pow = [] (auto pow, auto i, auto j) -> std::size_t {
        return j ? i * pow(pow, i, j - 1) : 1;
    };
    return index_pattern<(((Cs - '0') * pow(pow, 10, n - Is - 1)) + ...)>{};
}

}

namespace literals {

template<char ...Cs>
constexpr auto operator""_() {
    return detail::pat_impl<Cs...>(std::make_index_sequence<sizeof...(Cs)>{});
}

}

}

#endif
