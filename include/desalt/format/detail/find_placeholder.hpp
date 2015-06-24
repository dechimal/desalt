#if !defined DESALT_FORMAT_CONSTEXPR_FIND_PLACEHOLDER_HPP_INCLUDED_
#define      DESALT_FORMAT_CONSTEXPR_FIND_PLACEHOLDER_HPP_INCLUDED_

#include <desalt/format/constexpr/algorithm.hpp>

namespace desalt { namespace format { namespace detail {
namespace here = detail;

template<typename I>
constexpr I find_placeholder(I f, I l) {
    struct impl {
        static constexpr I f1(I f, I l) {
            return impl::f2(l, here::find(f, l, '%'));
        }
        static constexpr I f2(I l, I i) {
            return i == l
                ? i
                : impl::f3(l, i, here::next(i));
        }
        static constexpr I f3(I l, I i, I j) {
            return j == l || *j != '%'
                ? i
                : impl::f1(here::next(j), l);
        }
    };
    return impl::f1(f, l);
}

}}}

#endif
