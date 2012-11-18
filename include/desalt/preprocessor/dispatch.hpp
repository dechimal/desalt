#if !defined DESALT_PP_DISPATCH_HPP_INCLUDED_
#define      DESALT_PP_DISPATCH_HPP_INCLUDED_

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/tuple/rem.hpp>
#include <boost/preprocessor/tuple/eat.hpp>
#include <boost/preprocessor/facilities/is_empty.hpp>
#include <boost/preprocessor/expand.hpp>
#include <boost/preprocessor/control/iif.hpp>

#define DESALT_PP_DISPATCH(keyword_pred, get_macro, default_, toks) \
    BOOST_PP_EXPAND(BOOST_PP_IIF(DESALT_PP_DISPATCH_IS_KEYWORD(keyword_pred, toks), \
                                 DESALT_PP_DISPATCH_GET_MACRO_I, \
                                 default_ BOOST_PP_TUPLE_EAT())(get_macro, toks))
#define DESALT_PP_DISPATCH_IS_KEYWORD(keyword_pred, toks) \
    BOOST_PP_IS_EMPTY(BOOST_PP_TUPLE_ELEM(0, (BOOST_PP_CAT(keyword_pred, toks))))
#define DESALT_PP_DISPATCH_GET_MACRO_I(get_macro, toks) \
    BOOST_PP_TUPLE_ELEM(0, (BOOST_PP_CAT(get_macro, toks)))

#define DESALT_PP_DISPATCH_GET_ARG(keyword_pred, toks) \
    BOOST_PP_IIF(DESALT_PP_DISPATCH_IS_KEYWORD(keyword_pred, toks), \
                 DESALT_PP_DISPATCH_GET_ARG_I, \
                 toks BOOST_PP_TUPLE_EAT())(BOOST_PP_CAT(keyword_pred, toks))
#define DESALT_PP_DISPATCH_GET_ARG_I(...) \
    DESALT_PP_DISPATCH_GET_ARG_II(__VA_ARGS__)
#define DESALT_PP_DISPATCH_GET_ARG_II(x, ...) \
    __VA_ARGS__

#endif
