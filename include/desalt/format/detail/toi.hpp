#if !defined DESALT_FORMAT_DETAIL_TOI_HPP_INCLUDED_
#define      DESALT_FORMAT_DETAIL_TOI_HPP_INCLUDED_

namespace desalt { namespace format { namespace detail {
namespace here = detail;

template<typename I, typename T>
constexpr T toi_1(I f, I l, T x) {
    return f == l ? x : here::toi_1(f+1, l, x*10+(*f-'0'));
}
template<typename T, typename I>
constexpr T toi(I f, I l) {
    return f == l ? 0 : *f == '-' ? -here::toi_1(f+1, l, static_cast<T>(0)) : here::toi_1(f, l, static_cast<T>(0));
}

}}}

#endif
