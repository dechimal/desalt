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
template<std::size_t, typename Trailing, std::size_t ...Is, typename Args, typename F, typename ...Fs> constexpr auto static_match_impl_2(std::true_type, Trailing, std::index_sequence<Is...>, Args &&, F &&, Fs && ...);
template<std::size_t, std::size_t ...Is, typename Args, typename F, typename ...Fs> constexpr auto static_match_impl_2(std::false_type, std::true_type, std::index_sequence<Is...>, Args &&, F &&, Fs && ...);
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
        auto tup = std::forward_as_tuple(std::forward<Ts>(ts)...);
        return here::static_match_impl_2<I>(valid, trailing, std::index_sequence_for<Ts...>{}, std::move(tup), std::move(f), std::move(fs)...);
    };
}

template<std::size_t, typename Trailing, std::size_t ...Is, typename Args, typename F, typename ...Fs>
constexpr auto static_match_impl_2(std::true_type, Trailing, std::index_sequence<Is...>, Args && args, F && f, Fs && ...) {
    return f(std::forward<decltype(std::get<Is>(args))>(std::get<Is>(args))...);
}
template<std::size_t I, std::size_t ...Is, typename Args, typename F, typename ...Fs>
constexpr auto static_match_impl_2(std::false_type, std::true_type, std::index_sequence<Is...>, Args && args, F &&, Fs && ...fs) {
    return here::static_match_impl_1<I+1>(std::forward<decltype(std::get<Is>(args))>(std::get<Is>(args))...)(std::forward<Fs>(fs)...);
}
template<std::size_t, typename ...Ts>
constexpr auto static_match_impl_2(std::false_type, std::false_type, Ts && ...) {
    return [] (auto &&) {};
}

template<typename F, typename ...Ts, typename>
constexpr std::true_type is_valid(int) { return {}; }
template<typename F, typename ...Ts>
constexpr std::false_type is_valid(...) { return {}; }

} // namespace detail {

} } // namespace desalt { namespace static_control {


#endif
