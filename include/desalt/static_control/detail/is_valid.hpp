#if !defined STATIC_CONTROL_DETAIL_IS_VALID_HPP_INCLUDED_
#define      STATIC_CONTROL_DETAIL_IS_VALID_HPP_INCLUDED_

#include <type_traits>
#include <utility>

namespace desalt { namespace static_control { namespace detail {

template<typename F, typename ...Ts, typename = decltype(std::declval<F>()(std::declval<Ts>()...))>
constexpr std::true_type is_valid(int) {
    return {};
}
template<typename F, typename ...Ts>
constexpr std::false_type is_valid(...) {
    return {};
}

}}}

#endif
