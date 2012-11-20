#if !defined DESALT_PP_CAT_HPP_INCLUDED_
#define      DESALT_PP_CAT_HPP_INCLUDED_

#define DESALT_PP_CAT(x, ...) \
    DESALT_PP_CAT_I(x, __VA_ARGS__)
#define DESALT_PP_CAT_I(x, ...) \
    x ## __VA_ARGS__

#endif
