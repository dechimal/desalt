#if !defined STATIC_CONTROL_STATIC_IF_HPP_INCLUDED_
#define      STATIC_CONTROL_STATIC_IF_HPP_INCLUDED_

#include <utility>
#include <desalt/static_control/detail/wrap.hpp>
#include <desalt/static_control/static_match.hpp>

namespace desalt { namespace static_control {
namespace here = static_control;

template<typename F, typename ...Fs> constexpr auto static_if(F f, Fs ...fs);

namespace detail {

enum struct make_dependency;
template<typename, make_dependency> struct depend_impl;

} // namespace detail {


template<typename F, typename ...Fs>
constexpr auto static_if(F f, Fs ...fs) {
    return here::static_match<detail::make_dependency>(f, fs...);
}

template<typename T, detail::make_dependency Dep>
using depend = typename detail::depend_impl<T, Dep>::type;

namespace detail {

template<typename T, make_dependency>
struct depend_impl
    : wrap<T>
{};

enum struct make_dependency {};

template<>
struct wrap<make_dependency> {
    constexpr operator make_dependency() const { return {}; }
    template<typename T>
    constexpr T && operator()(T && x) const { return std::forward<T>(x); }
};

} // namespace detail {

} } // namespace desalt { namespace static_control {

#endif
