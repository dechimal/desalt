#if !defined STATIC_CONTROL_DETAIL_SPLIT_HPP_INCLUDED_
#define      STATIC_CONTROL_DETAIL_SPLIT_HPP_INCLUDED_

#include <tuple>
#include <utility>
#include <desalt/static_control/with_index_sequence.hpp>

namespace desalt { namespace static_control { namespace detail {
namespace here = detail;

template<typename F, typename ...Args>
constexpr auto split(F f, Args && ...args) {
    constexpr auto last = sizeof...(Args) - 1;
    auto && tup = std::forward_as_tuple(std::forward<Args>(args)...);
    return static_control::with_index_sequence<last>([&] (auto ...is) {
        auto step = std::get<last>(std::forward<std::tuple<Args...>>(tup));
        return f(std::move(step), std::get<is>(std::forward<std::tuple<Args...>>(tup))...);
    });
}

}}} // namespace desalt { namespace static_control { namespace detail {

#endif
