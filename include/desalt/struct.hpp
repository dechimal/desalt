#if !defined DESALT_STRUCT_HPP_INCLUDED_
#define      DESALT_STRUCT_HPP_INCLUDED_

#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/tuple/eat.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/cat.hpp>
#include <desalt/preprocessor/is_empty.hpp>
#include <desalt/parameter_pack.hpp>
#include <type_traits>

namespace desalt { namespace struct_ {
namespace here = struct_;

template<typename T> struct compatible_helper{};
template<typename T> struct compatible_helper<void(T)> { using type = T; };
template<typename T> using compatible_helper_t = typename compatible_helper<T>::type;

enum struct tag_type {};

template<typename Derived>
struct base {
    template<std::size_t I> decltype(auto) get() & { return static_cast<Derived &>(*this).get(std::integral_constant<std::size_t, I>{}); }
    template<std::size_t I> decltype(auto) get() const & { return static_cast<Derived const &>(*this).get(std::integral_constant<std::size_t, I>{}); }
    template<std::size_t I> decltype(auto) get() && { return static_cast<Derived &&>(*this).get(std::integral_constant<std::size_t, I>{}); }
    template<std::size_t I> decltype(auto) get() const && { return static_cast<Derived const &&>(*this).get(std::integral_constant<std::size_t, I>{}); }
};

template<std::size_t I, typename Derived> decltype(auto) get(base<Derived> & s) { return s.template get<I>(); }
template<std::size_t I, typename Derived> decltype(auto) get(base<Derived> const & s) { return s.template get<I>(); }
template<std::size_t I, typename Derived> decltype(auto) get(base<Derived> && s) { return std::move(s).template get<I>(); }
template<std::size_t I, typename Derived> decltype(auto) get(base<Derived> const && s) { return std::move(s).template get<I>(); }

}

using struct_::get;

} // namespace desalt { namespace struct_ {

#define DESALT_STRUCT(...) DESALT_STRUCT_I(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define DESALT_STRUCT_I(seq) \
    DESALT_STRUCT_II(BOOST_PP_IIF(DESALT_PP_IS_EMPTY(BOOST_PP_SEQ_ELEM(0, seq)), , seq))
#define DESALT_STRUCT_II(seq) \
    [] (auto && ...params) { \
        using namespace desalt; \
        BOOST_PP_SEQ_FOR_EACH_I(DESALT_STRUCT_M_1, ~, seq) \
        struct type: desalt::struct_::base<type>, BOOST_PP_ENUM_PARAMS(BOOST_PP_SEQ_SIZE(seq), desalt_struct_type) { \
            type(BOOST_PP_ENUM(BOOST_PP_SEQ_SIZE(seq), DESALT_STRUCT_M_3, ~)): BOOST_PP_ENUM(BOOST_PP_SEQ_SIZE(seq), DESALT_STRUCT_M_4, ~) {} \
            BOOST_PP_REPEAT(BOOST_PP_SEQ_SIZE(seq), DESALT_STRUCT_M_5, ~) \
        }; \
        return type{static_cast<decltype(params)>(params)...}; \
    }

#define DESALT_STRUCT_M_1(r, d, i, name) \
    struct BOOST_PP_CAT(desalt_struct_type, i) { \
        typename parameter_pack::at_c<parameter_pack::type_seq<std::decay_t<decltype(params)>...>, i>::type name; \
        struct compatible_type { \
            desalt::struct_::compatible_helper_t<void(typename parameter_pack::at_c<parameter_pack::type_seq<std::decay_t<decltype(params)>...>, i>::type name)> v; \
        }; \
        auto & get(std::integral_constant<std::size_t, i>) & { \
            return reinterpret_cast<compatible_type*>(this)->v; \
        } \
        auto const & get(std::integral_constant<std::size_t, i>) const & { \
            return reinterpret_cast<compatible_type const *>(this)->v; \
        } \
        auto && get(std::integral_constant<std::size_t, i>) && { \
            return static_cast<decltype(self().v)&&>(self().v); \
        } \
        decltype(auto) get(std::integral_constant<std::size_t, i>) const && { \
            using type = decltype(self().v); \
            using result_type = std::conditional_t< \
                std::is_lvalue_reference<type>{}, \
                std::remove_reference_t<type> const &, \
                std::remove_reference_t<type> const &&>; \
            return static_cast<result_type>(self().v); \
        } \
    private: \
        compatible_type && self() { return std::move(*reinterpret_cast<compatible_type *>(this)); } \
        compatible_type const && self() const { return std::move(*reinterpret_cast<compatible_type const *>(this)); } \
    };
#define DESALT_STRUCT_M_2(r, d, i, name) \
    BOOST_PP_COMMA_IF(i) typename parameter_pack::at_c<parameter_pack::type_seq<std::decay_t<decltype(params)>...>, i>::type name
#define DESALT_STRUCT_M_3(z, i, d) \
    typename parameter_pack::at_c<parameter_pack::type_seq<decltype(params)...>, i>::type BOOST_PP_CAT(a, i)
#define DESALT_STRUCT_M_4(z, i, d) \
    BOOST_PP_CAT(desalt_struct_type, i){ std::forward<decltype(std::declval<typename BOOST_PP_CAT(desalt_struct_type, i)::compatible_type>().v)>(BOOST_PP_CAT(a, i)) }
#define DESALT_STRUCT_M_5(z, i, d) \
    using BOOST_PP_CAT(desalt_struct_type, i)::get; \

#endif
