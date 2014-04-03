#if !defined DESALT_STRUCT_HPP_INCLUDED_
#define      DESALT_STRUCT_HPP_INCLUDED_

#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/tuple/eat.hpp>
#include <desalt/preprocessor/is_empty.hpp>
#include <desalt/parameter_pack.hpp>
#include <type_traits>

#define DESALT_STRUCT(...) DESALT_STRUCT_I(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define DESALT_STRUCT_I(seq) \
    DESALT_STRUCT_II(BOOST_PP_IIF(DESALT_PP_IS_EMPTY(BOOST_PP_SEQ_ELEM(0, seq)), , seq))
#define DESALT_STRUCT_II(seq) \
    [] (auto && ...params) { \
        struct type { \
            BOOST_PP_SEQ_FOR_EACH_I(DESALT_STRUCT_M, ~, seq) \
        }; \
        return type{static_cast<decltype(params)>(params)...}; \
    }

#define DESALT_STRUCT_M(r, d, i, name) \
    typename desalt::parameter_pack::at_c<desalt::parameter_pack::type_seq<typename std::decay<decltype(params)>::type...>, i>::type name;

#endif
