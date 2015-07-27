#if !defined STATIC_CONTROL_STATIC_IF_HPP_INCLUDED_
#define      STATIC_CONTROL_STATIC_IF_HPP_INCLUDED_

#include <utility>
#include <desalt/static_control/static_match.hpp>

namespace desalt { namespace static_control {
namespace here = static_control;

template<typename F, typename ...Fs> constexpr auto static_if(F f, Fs ...fs);

namespace detail {
namespace here = detail;

enum struct make_dependency_helper {};

struct make_dependency {
    constexpr operator make_dependency_helper() const { return {}; }
    template<typename T>
    constexpr T && operator()(T && x) const { return std::forward<T>(x); }
};

template<typename T, make_dependency_helper>
struct depend_impl {
    using type = T;
};

} // namespace detail {


template<typename F, typename ...Fs>
constexpr auto static_if(F f, Fs ...fs) {
    return here::static_match(detail::make_dependency{})(std::move(f), std::move(fs)...);
}

template<typename T, detail::make_dependency_helper Dep>
using depend = typename detail::depend_impl<T, Dep>::type;

} } // namespace desalt { namespace static_control {

#endif
