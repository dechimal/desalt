#if !defined DESALT_FORMAT_CONSTEXPR_FIND_INDEX_SPECIFIER_HPP_INCLUDED_
#define      DESALT_FORMAT_CONSTEXPR_FIND_INDEX_SPECIFIER_HPP_INCLUDED_

#include <desalt/format/constexpr/algorithm.hpp>
#include <desalt/format/detail/make_charset_pred.hpp>

namespace desalt { namespace format { namespace detail {
namespace here = detail;

template<typename I>
constexpr auto find_index_specifier_1(I f, I i, I l) {
    return i == f || i == l ? f : *i != '$' ? f : *f == '0' ? (throw "placeholder index must not be zero."), i : here::next(i);
}

template<typename I>
constexpr auto find_index_specifier(I f, I l) {
    return here::find_index_specifier_1(f, here::find_if(f, l, here::make_charset_pred("0123456789")), l);
}


}}}
#endif
