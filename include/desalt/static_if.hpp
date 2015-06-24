#include <desalt/require.hpp>

namespace desalt {
namespace detail {
template<typename T> struct wrap { using type = T; };
}
template<bool Cond, typename ...Args, typename True, typename False, DESALT_REQUIRE_C(Cond)>
auto static_if(True const & t, False const &) { return t(detail::wrap<Args>{}...); }
template<bool Cond, typename ...Args, typename True, typename False, DESALT_REQUIRE_C(!Cond)>
auto static_if(True const &, False const & f) { return f(detail::wrap<Args>{}...); }
}
