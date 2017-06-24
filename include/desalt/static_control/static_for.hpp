#if !defined STATIC_CONTROL_STATIC_FOR_HPP_INCLUDED_
#define      STATIC_CONTROL_STATIC_FOR_HPP_INCLUDED_

#include <tuple>
#include <utility>
#include <desalt/static_control/detail/split.hpp>
#include <desalt/static_control/clause.hpp>
#include <desalt/static_control/static_while.hpp>

namespace desalt { namespace static_control {
namespace here = static_control;

namespace detail {
template<std::size_t N>
using counter = std::integral_constant<std::size_t, N>;
enum struct first {};
enum struct last {};
template<std::size_t, std::size_t, bool, typename ...Args> constexpr auto static_for_impl(Args && ...);
}

template<std::size_t, std::size_t, typename ...Args> constexpr auto static_for(Args && ...);
template<detail::first, detail::last, typename ...Args> constexpr auto static_for(Args && ...);
template<detail::first, typename ...Args> constexpr auto static_for(Args && ...);
template<detail::last, typename ...Args> constexpr auto static_for(Args && ...);
template<typename ...Args> constexpr auto static_for(Args && ...);

detail::first first(std::size_t);
detail::last last(std::size_t);



template<std::size_t First, std::size_t Last, typename ...Args>
constexpr auto static_for(Args && ...args) {
    return detail::static_for_impl<First, Last, true>(std::forward<Args>(args)...);
}
template<detail::first First, detail::last Last, typename ...Args>
constexpr auto static_for(Args && ...args) {
    return detail::static_for_impl<(std::size_t)First, (std::size_t)Last, true>(std::forward<Args>(args)...);
}
template<detail::first First, typename ...Args>
constexpr auto static_for(Args && ...args) {
    return detail::static_for_impl<(std::size_t)First, 0, false>(std::forward<Args>(args)...);
}
template<detail::last Last, typename ...Args>
constexpr auto static_for(Args && ...args) {
    return detail::static_for_impl<0, (std::size_t)Last, true>(std::forward<Args>(args)...);
}
template<typename ...Args>
constexpr auto static_for(Args && ...args) {
    return detail::static_for_impl<0, 0, false>(std::forward<Args>(args)...);
}

detail::first first(std::size_t n) { return (detail::first)n; }
detail::last last(std::size_t n) { return (detail::last)n; }

namespace detail {

template<std::size_t First, std::size_t Last, bool RangeCheck, typename ...Args>
constexpr auto static_for_impl(Args && ...args) {
    return detail::split([&] (auto step, auto && ...states) {
        // workaround for gcc:
        // a) if define step1 with type deduction as following:
        //      auto step1 = regular<decltype(step)>{std::move(step)};
        //    step1 is ugly type in result type deduction of step2.
        // b) sfinae in result type deduction of step2 does not work
        //    if conditions of step2 (cond1/cond2 in below code) extract to
        //    a another function and call it in result type deduction of step2.
        regular<decltype(step)> step1{std::move(step)};
        auto step2 = [&] (auto i, auto && ...states) ->
            decltype(
                (std::enable_if_t<!RangeCheck || (decltype(i){} < Last)>)0,   // cond1
                (void)(step1(i, std::forward<decltype(states)>(states)...)),  // cond2
                std::tuple_cat(std::make_tuple(counter<decltype(i){}+1>{}), step1(i, std::forward<decltype(states)>(states)...)))
        {
            return std::tuple_cat(std::make_tuple(counter<decltype(i){}+1>{}), step1(i, std::forward<decltype(states)>(states)...));
        };
        return detail::static_while_impl(
            std::move(step2),
            std::make_index_sequence<sizeof...(states) + 1>{},
            std::forward_as_tuple(counter<First>{}, std::forward<decltype(states)>(states)...));
    }, std::forward<Args>(args)...);
}

} // namespace detail {

}} // namespace desalt { namespace static_control {

#endif
