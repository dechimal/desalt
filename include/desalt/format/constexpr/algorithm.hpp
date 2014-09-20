#if !defined DESALT_FORMAT_CONSTEXPR_ALGORITHM_HPP_INCLUDED_
#define      DESALT_FORMAT_CONSTEXPR_ALGORITHM_HPP_INCLUDED_

namespace desalt { namespace format { namespace detail {
namespace here = detail;

template<typename T>
constexpr T const * next(T const * p) {
    return p+1;
}
template<typename T>
constexpr T const * prev(T const * p) {
    return p-1;
}

template<typename T>
constexpr T max(T x) {
    return x;
}
template<typename T, typename U, typename ...Args>
constexpr auto max(T x, U y, Args ...args) {
    return here::max(x >= y ? x : y, args...);
}
template<typename T>
constexpr T min(T x) {
    return x;
}
template<typename T, typename U, typename ...Args>
constexpr auto const & min(T x, U y, Args ...args) {
    return here::min(x <= y ? x : y, args...);
}

template<typename P>
struct not_pred {
    constexpr not_pred(P p) : p(p) {}
    template<typename T>
    constexpr bool operator()(T const & t) const { return !p(t); }
    P p;
};
template<typename P>
constexpr not_pred<P> not_(P p) { return not_pred<P>{p}; }

template<typename I, typename P>
constexpr I find_if(I f, I l, P p) {
    return f == l ? f
                  : p(*f) ? f
                          : here::find_if(here::next(f), l, p);
}

template<typename T>
struct find_pred {
    constexpr find_pred(T v) : v(v) {}
    constexpr bool operator()(T const & w) const { return v == w; }
    T v;
};
template<typename I, typename T>
constexpr I find(I f, I l, T v) {
    return here::find_if(f, l, find_pred<T>(v));
}

template<typename I>
struct find_first_of_pred {
    constexpr find_first_of_pred(I f, I l) : f(f), l(l) {}
    template<typename T>
    constexpr bool operator()(T const & ch) const { return here::find(f, l, ch) != l; }
    I f, l;
};
template<typename I1, typename I2>
constexpr I1 find_first_of(I1 f1, I1 l1, I2 f2, I2 l2) {
    return here::find(f1, l1, find_first_of_pred<I2>{f2, l2});
}

}}}

#endif
