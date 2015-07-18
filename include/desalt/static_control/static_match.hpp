#if !defined STATIC_CONTROL_STATIC_MATCH_HPP_INCLUDED_
#define      STATIC_CONTROL_STATIC_MATCH_HPP_INCLUDED_

#include <type_traits>
#include <desalt/static_control/detail/wrap.hpp>
#include <desalt/static_control/detail/is_valid.hpp>

namespace desalt { namespace static_control {
namespace here = static_control;

// forward declarations
template<typename ..., typename F, typename ...Fs> constexpr auto static_match(F f, Fs ...fs);

namespace detail {
namespace here = detail;

template<typename ..., typename F, typename ...Fs> constexpr auto static_match_impl(std::true_type, F, Fs ...);
template<typename ..., typename F0, typename F1, typename ...Fs> constexpr auto static_match_impl(std::false_type, F0, F1 f1, Fs ...);
template<typename ..., typename F0> constexpr void static_match_impl(std::false_type, F0);

} // namespace detail {


// static_match function implementation
template<typename ...Ts, typename F, typename ...Fs>
constexpr auto static_match(F f, Fs ...fs) {
    return detail::static_match_impl<Ts...>(detail::is_valid<F, detail::wrap<Ts>...>(0), f, fs...);
}

namespace detail {

template<typename ...Ts, typename F, typename ...Fs>
constexpr auto static_match_impl(std::true_type, F f, Fs ...) {
    return f(wrap<Ts>{}...);
}
template<typename ...Ts, typename F0, typename F1, typename ...Fs>
constexpr auto static_match_impl(std::false_type, F0, F1 f1, Fs ...fs) {
    return here::static_match_impl<Ts...>(here::is_valid<F1, wrap<Ts>...>(0), f1, fs...);
}
template<typename ..., typename F0>
constexpr void static_match_impl(std::false_type, F0) {}

} // namespace detail {

} } // namespace desalt { namespace static_control {


#endif
