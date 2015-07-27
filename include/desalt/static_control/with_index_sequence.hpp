#if !defined STATIC_CONTROL_WITH_INDEX_SEQUENCE_HPP_INCLUDED_
#define      STATIC_CONTROL_WITH_INDEX_SEQUENCE_HPP_INCLUDED_

#include <type_traits>
#include <utility>

namespace desalt { namespace static_control {
namespace here = static_control;

namespace detail {
template<std::size_t ...Ixs, typename F> constexpr auto with_index_sequence_impl(std::index_sequence<Ixs...>, F);
}


template<std::size_t N, typename F>
constexpr auto with_index_sequence(F f) {
    return detail::with_index_sequence_impl(std::make_index_sequence<N>{}, f);
}
template<typename ...Pack, typename F>
constexpr auto with_index_sequence(F f) {
    return here::with_index_sequence<sizeof...(Pack)>(f);
}

namespace detail {

template<std::size_t ...Is, typename F>
constexpr auto with_index_sequence_impl(std::index_sequence<Is...>, F f) {
    return f(std::integral_constant<std::size_t, Is>{}...);
}

} // namespace detail {

}} // namespace desalt { namespace static_control {

#endif
