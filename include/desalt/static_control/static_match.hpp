#if !defined STATIC_CONTROL_STATIC_MATCH_HPP_INCLUDED_
#define      STATIC_CONTROL_STATIC_MATCH_HPP_INCLUDED_

#include <type_traits>

namespace desalt { namespace static_control {
namespace here = static_control;

// forward declarations
template<typename ...Ts> constexpr auto static_match(Ts && ...);

namespace detail {
namespace here = detail;

template<std::size_t, typename ...Ts> constexpr auto static_match_impl_1(Ts && ...);
template<std::size_t, typename Trailing, typename ...Ts> constexpr auto static_match_impl_2(std::true_type, Trailing, Ts && ...);
template<std::size_t, typename ...Ts> constexpr auto static_match_impl_2(std::false_type, std::true_type, Ts && ...);
template<std::size_t, typename ...Ts> constexpr auto static_match_impl_2(std::false_type, std::false_type, Ts && ...);
template<typename F, typename ...Ts, typename = decltype(std::declval<F>()(std::declval<Ts>()...))> constexpr std::true_type is_valid(int);
template<typename F, typename ...Ts> constexpr std::false_type is_valid(...);

} // namespace detail {


// static_match function implementation
template<typename ...Ts>
constexpr auto static_match(Ts && ...ts) {
    return detail::static_match_impl_1<0>(std::forward<Ts>(ts)...);
}

namespace detail {

template<std::size_t I, typename ...Ts>
constexpr auto static_match_impl_1(Ts && ...ts) {
    return [&] (auto f, auto ...fs) {
        auto valid = detail::is_valid<decltype(f), Ts &&...>(0);
        auto trailing = std::integral_constant<bool, sizeof...(fs)>{};
        return here::static_match_impl_2<I>(valid, trailing, std::forward<Ts>(ts)...)(std::move(f), std::move(fs)...);
    };
}

template<std::size_t, typename Trailing, typename ...Ts>
constexpr auto static_match_impl_2(std::true_type, Trailing, Ts && ...ts) {
    return [&] (auto f, auto && ...) {
        return f(std::forward<Ts>(ts)...);
    };
}
template<std::size_t I, typename ...Ts>
constexpr auto static_match_impl_2(std::false_type, std::true_type, Ts && ...ts) {
    return [&] (auto &&, auto ...fs) {
        return here::static_match_impl_1<I+1>(std::forward<Ts>(ts)...)(std::move(fs)...);
    };
}
template<std::size_t, typename ...Ts>
constexpr auto static_match_impl_2(std::false_type, std::false_type, Ts && ...ts) {
    return [&] (auto &&) {};
}

template<typename F, typename ...Ts, typename>
constexpr std::true_type is_valid(int) { return {}; }
template<typename F, typename ...Ts>
constexpr std::false_type is_valid(...) { return {}; }

} // namespace detail {

} } // namespace desalt { namespace static_control {


#endif
