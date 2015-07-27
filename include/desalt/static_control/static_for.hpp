#if !defined STATIC_CONTROL_STATIC_FOR_HPP_INCLUDED_
#define      STATIC_CONTROL_STATIC_FOR_HPP_INCLUDED_

#include <tuple>
#include <utility>
#include <desalt/static_control/detail/split.hpp>
#include <desalt/static_control/clause.hpp>

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

detail::first first(std::size_t n) { return (detail::first)n; };
detail::last last(std::size_t n) { return (detail::last)n; };

namespace detail {

template<std::size_t First, std::size_t Last, bool RangeCheck, typename ...Args>
constexpr auto static_for_impl(Args && ...args) {
    return detail::split([&] (auto step, auto && ...states) {
        auto step1 = regular<decltype(step)>(std::move(step));
        return detail::static_while_impl(
            static_control::clause(
                [&] (auto i, auto && ...states) -> decltype((std::enable_if_t<!RangeCheck || (i < Last)>)0, step1(i, std::forward<decltype(states)>(states)...)) {},
                [&] (auto i, auto && ...states)
        {
            return std::tuple_cat(std::make_tuple(counter<i+1>{}), step1(i, std::forward<decltype(states)>(states)...));
        }), counter<First>{}, std::forward<decltype(states)>(states)...);
    }, std::forward<Args>(args)...);
}

} // namespace detail {

}} // namespace desalt { namespace static_control {

#endif
