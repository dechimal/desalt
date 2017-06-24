#if !defined STATIC_CONTROL_STATIC_IF_HPP_INCLUDED_
#define      STATIC_CONTROL_STATIC_IF_HPP_INCLUDED_

#include <utility>
#include <desalt/static_control/static_match.hpp>
#include <desalt/static_control/wrap.hpp>

namespace desalt { namespace static_control {
namespace here = static_control;

template<typename F, typename ...Fs> constexpr auto static_if(F f, Fs ...fs);

namespace detail {
namespace here = detail;

struct make_dependency {
    static constexpr bool value = false;
    template<typename T>
    constexpr T && operator()(T && x) const { return std::forward<T>(x); }
    template<typename T>
    constexpr T type(wrap<T> x) const;
};

} // namespace detail {


template<typename F, typename ...Fs>
constexpr auto static_if(F f, Fs ...fs) {
    return here::static_match(detail::make_dependency{})(std::move(f), std::move(fs)...);
}

} } // namespace desalt { namespace static_control {

#endif
