#if !defined DESALT_FORMAT_CONSTEXPR_FIND_PLACEHOLDER_HPP_INCLUDED_
#define      DESALT_FORMAT_CONSTEXPR_FIND_PLACEHOLDER_HPP_INCLUDED_

#include <desalt/format/constexpr/algorithm.hpp>

namespace desalt { namespace format { namespace detail {
namespace here = detail;

template<typename I> constexpr I find_placeholder(I, I);
template<typename I> constexpr I find_placeholder_1(I, I);
template<typename I> constexpr I find_placeholder_2(I, I, I);

template<typename I>
constexpr I find_placeholder(I f, I l) {
    return here::find_placeholder_1(l, here::find(f, l, '%'));
}
template<typename I>
constexpr I find_placeholder_1(I l, I i) {
    return i == l ? i : here::find_placeholder_2(l, i, here::find(here::next(i), l, '%'));
}
template<typename I>
constexpr I find_placeholder_2(I l, I i, I j) {
    return j == l || j != i+1 ? i : here::find_placeholder(here::next(j), l);
}

}}}

#endif
