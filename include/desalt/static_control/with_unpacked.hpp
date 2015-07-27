#if !defined STATIC_CONTROL_WITH_UNPACKED_HPP_INCLUDED_
#define      STATIC_CONTROL_WITH_UNPACKED_HPP_INCLUDED_

#include <tuple>
#include <utility>
#include <desalt/static_control/with_index_sequence.hpp>

namespace desalt { namespace static_control {
namespace here = static_control;

template<typename ...Ts, typename F>
constexpr auto with_unpacked(std::tuple<Ts...> & tup, F f) {
    return here::with_index_sequence<Ts...>([&] (auto ...is) {
        return f(std::get<is>(tup)...);
    });
}
template<typename ...Ts, typename F>
constexpr auto with_unpacked(std::tuple<Ts...> const & tup, F f) {
    return here::with_index_sequence<Ts...>([&] (auto ...is) {
        return f(std::get<is>(tup)...);
    });
}
template<typename ...Ts, typename F>
constexpr auto with_unpacked(std::tuple<Ts...> && tup, F f) {
    return here::with_index_sequence<Ts...>([&] (auto ...is) {
        return f(std::get<is>(std::move(tup))...);
    });
}

}} // namespace desalt { namespace static_control {

#endif
