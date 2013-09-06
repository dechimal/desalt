#if !defined DESALT_PP_DISPATCH_HPP_INCLUDED_
#define      DESALT_PP_DISPATCH_HPP_INCLUDED_

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/tuple/rem.hpp>
#include <boost/preprocessor/tuple/eat.hpp>
#include <boost/preprocessor/facilities/is_empty.hpp>
#include <boost/preprocessor/expand.hpp>
#include <boost/preprocessor/control/iif.hpp>
#include <desalt/preprocessor/is_keyword.hpp>

#define DESALT_PP_DISPATCH(keyword_pred, get_macro, default_, ...) \
    BOOST_PP_EXPAND(BOOST_PP_IIF(DESALT_PP_IS_KEYWORD(keyword_pred, __VA_ARGS__), \
                                 DESALT_PP_DISPATCH_GET_MACRO_I, \
                                 default_ BOOST_PP_TUPLE_EAT())(get_macro, __VA_ARGS__))
#define DESALT_PP_DISPATCH_IS_KEYWORD(keyword_pred, ...) \
    BOOST_PP_IS_EMPTY(BOOST_PP_TUPLE_ELEM(0, (DESALT_PP_CAT(keyword_pred, __VA_ARGS__))))
#define DESALT_PP_DISPATCH_GET_MACRO_I(get_macro, ...) \
    BOOST_PP_TUPLE_ELEM(0, (DESALT_PP_CAT(get_macro, __VA_ARGS__)))

#define DESALT_PP_DISPATCH_GET_ARG(keyword_pred, ...) \
    BOOST_PP_IIF(DESALT_PP_DISPATCH_IS_KEYWORD(keyword_pred, __VA_ARGS__), \
                 DESALT_PP_DISPATCH_GET_ARG_KEYWORD, \
                 DESALT_PP_DISPATCH_GET_ARG_DEFAULT)(keyword_pred, __VA_ARGS__)
#define DESALT_PP_DISPATCH_GET_ARG_KEYWORD(keyword_pred, ...) \
    DESALT_PP_DISPATCH_GET_ARG_KEYWORD_I(DESALT_PP_CAT(keyword_pred, __VA_ARGS__))
#define DESALT_PP_DISPATCH_GET_ARG_KEYWORD_I(...) \
    DESALT_PP_DISPATCH_GET_ARG_KEYWORD_II(__VA_ARGS__)
#define DESALT_PP_DISPATCH_GET_ARG_KEYWORD_II(x, ...) \
    __VA_ARGS__
#define DESALT_PP_DISPATCH_GET_ARG_DEFAULT(keyword_pred, ...) \
    __VA_ARGS__

#endif
