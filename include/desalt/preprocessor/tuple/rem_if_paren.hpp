#if !defined DESALT_PP_TUPLE_REM_IF_PAREN_HPP_INCLUDED_
#define      DESALT_PP_TUPLE_REM_IF_PAREN_HPP_INCLUDED_

#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/tuple/rem.hpp>
#include <boost/preprocessor/facilities/empty.hpp>

#define DESALT_PP_TUPLE_REM_IF_PAREN(tup) \
    BOOST_PP_IIF(DESALT_PP_STARTS_WITH_ENCLOSING_PAREN(tup), \
                 BOOST_PP_TUPLE_REM, \
                 BOOST_PP_EMPTY)()
#endif
