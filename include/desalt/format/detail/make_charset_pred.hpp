#if !defined DESALT_FORMAT_CONSTEXPR_MAKE_CHARSET_PRED_HPP_INCLUDED_
#define      DESALT_FORMAT_CONSTEXPR_MAKE_CHARSET_PRED_HPP_INCLUDED_

#include <desalt/format/constexpr/algorithm.hpp>

namespace desalt { namespace format { namespace detail {
namespace here = detail;

template<typename Char, std::size_t N>
constexpr auto make_charset_pred(Char const (& chars)[N]) {
    return here::not_(find_first_of_pred<Char const *>{chars, chars+sizeof(chars)});
}

}}}

#endif
