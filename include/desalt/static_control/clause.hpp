#if !defined DESALT_STATIC_CONTROL_CLAUSE_HPP_INCLUDED_
#define      DESALT_STATIC_CONTROL_CLAUSE_HPP_INCLUDED_

#include <utility>

namespace desalt { namespace static_control {
namespace here = static_control;

template<typename P, typename F>
constexpr auto clause(P, F f) {
    return [=] (auto && ...args) -> decltype(std::declval<P>()(std::forward<decltype(args)>(args)...), (void)0, f(std::forward<decltype(args)>(args)...)) {
        return f(std::forward<decltype(args)>(args)...);
    };
}

}} // namespace desalt { namespace static_control {

#endif
