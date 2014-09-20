#if !defined DESALT_FORMAT_CONSTEXPR_ARRAY_HPP_INCLUDED_
#define      DESALT_FORMAT_CONSTEXPR_ARRAY_HPP_INCLUDED_

#include <cstddef>

namespace desalt { namespace format { namespace detail {
namespace here = detail;

template<typename T, std::size_t N>
struct array {
    static constexpr std::size_t size() {
        return N;
    }
    using t = T const [N];
    constexpr t const & data() const {
        return a;
    }
    T a[N];
};

}}}

#endif
