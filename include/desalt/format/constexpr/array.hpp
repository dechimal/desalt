#if !defined DESALT_FORMAT_CONSTEXPR_ARRAY_HPP_INCLUDED_
#define      DESALT_FORMAT_CONSTEXPR_ARRAY_HPP_INCLUDED_

#include <cstddef>

namespace desalt { namespace format { namespace detail {
namespace here = detail;

template<typename T, std::size_t N>
struct array {
    constexpr std::size_t size() const {
        return N;
    }
    using t = T const [N];
    constexpr t const & data() const {
        return a;
    }
    T a[N];
};

template<typename T>
struct array<T, 0> {
    constexpr std::size_t size() const {
        return 0;
    }
};

template<typename T, typename ...Args>
array<T, sizeof...(Args)> make_array(Args ...args) {
    return {{args...}};
}
template<typename T, typename ...Args>
array<T, 0> make_array() {
    return {};
}

}}}

#endif
