#if !defined DESALT_AUTO_FUN_HPP_INCLUDED_
#define      DESALT_AUTO_FUN_HPP_INCLUDED_

#define DESALT_AUTO_FUN(name_sig, ...) \
    auto name_sig -> decltype(__VA_ARGS__) { \
        return __VA_ARGS__; \
    } static_assert(true, "")

#define DESALT_AUTO_FUN_NOEXCEPT(name_sig, ...) \
    DESALT_AUTO_FUN(name_sig noexcept(noexcept(__VA_ARGS__)), __VA_ARGS__)

#endif
