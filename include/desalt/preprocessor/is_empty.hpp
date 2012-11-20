#if !defined DESALT_PP_IS_EMPTY_HPP_INCLUDED_
#define      DESALT_PP_IS_EMPTY_HPP_INCLUDED_

#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/tuple/eat.hpp>
#include <desalt/preprocessor/cat.hpp>
#include <desalt/preprocessor/starts_with_enclosing_paren.hpp>

#define DESALT_PP_IS_EMPTY(...) \
    BOOST_PP_IIF(DESALT_PP_STARTS_WITH_ENCLOSING_PAREN(__VA_ARGS__ ()), \
                 DESALT_PP_IS_EMPTY_I, \
                 0 BOOST_PP_TUPLE_EAT())(__VA_ARGS__)
#define DESALT_PP_IS_EMPTY_I(...) \
    BOOST_PP_IIF(DESALT_PP_STARTS_WITH_ENCLOSING_PAREN(__VA_ARGS__), \
                 0 BOOST_PP_TUPLE_EAT(), \
                 DESALT_PP_IS_EMPTY_II)(__VA_ARGS__)
#define DESALT_PP_IS_EMPTY_II(...) \
    DESALT_PP_STARTS_WITH_ENCLOSING_PAREN(DESALT_PP_CAT(DESALT_PP_IS_EMPTY_HELPER, __VA_ARGS__) ())
#define DESALT_PP_IS_EMPTY_HELPER() ()

#endif
