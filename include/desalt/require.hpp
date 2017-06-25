#if !defined DESALT_REQUIRE_HPP_INCLUDED_
#define      DESALT_REQUIRE_HPP_INCLUDED_

#include <type_traits>

#define DESALT_REQUIRE(...) \
    DESALT_REQUIRE_C(__VA_ARGS__::value)

#define DESALT_REQUIRE_NOT(...) \
    DESALT_REQUIRE_C(!(__VA_ARGS__::value))

#define DESALT_REQUIRE_C(...) \
    typename std::enable_if<(__VA_ARGS__), desalt::aux::enabler_t>::type = desalt::aux::enabler_t::value

#define DESALT_REQUIRE_EXPR(...) \
    DESALT_REQUIRE(decltype((__VA_ARGS__), (void)0, std::true_type{}))

namespace desalt { namespace aux {

enum struct enabler_t {
    value
};

}}

#endif
