#if !defined DESALT_PREPROCESSOR_IS_KEYWORD_HPP_INCLUDED_
#define      DESALT_PREPROCESSOR_IS_KEYWORD_HPP_INCLUDED_

#include <boost/preprocessor/facilities/is_empty.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <desalt/preprocessor/cat.hpp>

#define DESALT_PP_IS_KEYWORD(keyword_pred, ...) \
    BOOST_PP_IS_EMPTY(BOOST_PP_TUPLE_ELEM(0, (DESALT_PP_CAT(keyword_pred, __VA_ARGS__))))

#endif
