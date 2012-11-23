#if !defined DESALT_NEWTYPE_HPP_INCLUDED_
#define      DESALT_NEWTYPE_HPP_INCLUDED_

#include <utility>
#include <type_traits>
#include <boost/mpl/or.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/utility/identity_type.hpp>

#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/variadic/size.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/tuple/rem.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/to_seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/seq/seq.hpp>

#include <desalt/require.hpp>
#include <desalt/auto_fun.hpp>
#include <desalt/preprocessor/dispatch.hpp>

#define DESALT_NEWTYPE(name, base, ...) \
    DESALT_NEWTYPE_I(name, BOOST_IDENTITY_TYPE(base), \
                     __VA_ARGS__ \
                     BOOST_PP_COMMA_IF(BOOST_PP_EQUAL(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 1)))
#define DESALT_NEWTYPE_I(name, base, ...) \
    DESALT_NEWTYPE_II(name, base, __VA_ARGS__)
#define DESALT_NEWTYPE_II(name, base, unwrap, ...) \
    struct name : private desalt::newtype::base_wrapper<typename base> { \
        typename base & unwrap() noexcept { return *this; } \
        typename base const & unwrap() const noexcept { return *this; } \
        friend class desalt::newtype::wrapper_access; \
        explicit name(typename base const & b) noexcept(noexcept(typename base(b))) : desalt::newtype::base_wrapper<typename base>(b) {} \
        explicit name(typename base && b) noexcept(noexcept(typename base(std::move(b)))) : desalt::newtype::base_wrapper<typename base>(std::move(b)) {} \
        BOOST_PP_SEQ_FOR_EACH(DESALT_NEWTYPE_M, (name, typename base), BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
    }

#define DESALT_NEWTYPE_M(r, d, op) \
    DESALT_NEWTYPE_M_I(op, BOOST_PP_TUPLE_REM() d)
#define DESALT_NEWTYPE_M_I(op, d) \
    DESALT_PP_DISPATCH(DESALT_NEWTYPE_KW_, DESALT_NEWTYPE_GET_MACRO_, DESALT_NEWTYPE_USING, op)(DESALT_PP_DISPATCH_GET_ARG(DESALT_NEWTYPE_KW_, op), d)

#define DESALT_NEWTYPE_USING(member, name, base) \
    using base::member;

#define DESALT_NEWTYPE_KW_this ,
#define DESALT_NEWTYPE_GET_MACRO_this DESALT_NEWTYPE_CTOR,
#define DESALT_NEWTYPE_CTOR(_, name, base) \
    using desalt::newtype::base_wrapper<base>::base_wrapper;

#define DESALT_NEWTYPE_KW_typename ,
#define DESALT_NEWTYPE_GET_MACRO_typename DESALT_NEWTYPE_USING_TYPENAME,
#define DESALT_NEWTYPE_USING_TYPENAME(type, name, base) \
    using base::type;

#define DESALT_NEWTYPE_KW_auto ,
#define DESALT_NEWTYPE_GET_MACRO_auto DESALT_NEWTYPE_WRAPPED,
#define DESALT_NEWTYPE_WRAPPED(member, name, base) \
    DESALT_NEWTYPE_WRAPPED_I(member, name, base,) \
    DESALT_NEWTYPE_WRAPPED_I(member, name, base, const)

#define DESALT_NEWTYPE_WRAPPED_I(member, name, base, cv) \
    template<typename ...Args, \
             DESALT_REQUIRE(boost::mpl::not_<boost::mpl::or_<desalt::newtype::is_same_unqualified<Args, base>...> >)> \
    auto member(Args && ...args) cv \
        -> decltype(desalt::newtype::wrapper_access::wrap<name, base>( \
                        std::declval<base cv>().member( \
                            desalt::newtype::wrapper_access::unwrap<name, base>( \
                                std::forward<Args>(args)...)))) \
    { \
        return desalt::newtype::wrapper_access::wrap<name, base>( \
                   desalt::newtype::base_wrapper<base>::member( \
                       desalt::newtype::wrapper_access::unwrap<name, base>( \
                           std::forward<Args>(args)...))); \
    }

// explicit
#define DESALT_NEWTYPE_KW_explicit ,
#define DESALT_NEWTYPE_GET_MACRO_explicit DESALT_NEWTYPE_WITH_ARGUMENT,
#define DESALT_NEWTYPE_WITH_ARGUMENT(f_sig, name, base) \
    DESALT_NEWTYPE_WITH_ARGUMENT_I(DESALT_NEWTYPE_GET_NAME() name_sig, BOOST_PP_TUPLE_EAT() name_sig, name, bas)
#define DESALT_NEWTYPE_WITH_ARGUMENT_I(f, sig, name, base) \
    DESALT_NEWTPYE_WITH_ARGUMENT_II(f, BOOST_PP_VARIADIC_SIZE sig, sig, name, base)
#define DESALT_NEWTYPE_WITH_ARGUMENT(f, arity, sig, name, base) \
    DESALT_AUTO_FUN(f(DESALT_NEWTYPE_WITH_ARGUMENT_GEN_PARAM_LIST(sig)), \
                    this->base_wrapper::f(DESALT_NEWTYPE_WITH_ARGUMENT_GEN_FORWARD_LIST(sig));

// namespace
#define DESALT_NEWTYPE_KW_namespace ,
#define DESALT_NEWTYPE_GET_MACRO_namespace DESALT_NEWTYPE_NONMEMBER,
#define DESALT_NEWTYPE_NONMEMBER(f, name, base) \
    DESALT_PP_DISPATCH(DESALT_NEWTYPE_F_KW_, DESALT_NEWTYPE_F_GET_MACRO_, DESALT_NEWTYPE_NONMEMBER_I, f)(DESALT_PP_DISPATCH_GET_ARG(DESALT_NEWTYPE_F_KW_, f), name, base)
/*
#define DESALT_NEWTYPE_NONMEMBER_I(_, name, base) \
    template<typename ...Args, \
             typename BaseRes = \
                 decltype(f(std::declval<typename desalt::newtype::unwrap_if_derived< \
                                         Args, name, base>::type>()...)), \
             typename Res = typename \
                 desalt::newtype::wrap_if_base< \
                     BaseRes, name, base>::type> \
    friend Res f(Args && ...args) { \
        using desalt::newtype::is_same_unqualified; \
        static_assert(!boost::mpl::or_<is_same_unqualified<Args, base>...>::value, ""); \
        return static_cast<Res>(f(std::forward<Args>(args)...)); \
    }
*/
// namespace explicit
#define DESALT_NEWTYPE_F_KW_explicit ,
#define DESALT_NEWTYPE_F_GET_MACRO_explicit DESALT_NEWTYPE_NONMEMBER_WITH_ARGUMENT,
#define DESALT_NEWTYPE_NONMEMBER_WITH_ARGUMENT(f_sig, name, base) \
    DESALT_PP_DISPATCH(DESALT_NEWTYPE_F_E_KW_, DESALT_NEWTYPE_F_E_GET_MACRO_, DESALT_NEWTYPE_NONMEMBER_WITH_ARGUMENT_II, f_sig)(DESALT_PP_DISPATCH_GET_ARG(DESALT_NEWTYPE_F_E_KW_, f_sig), name, base)

// #define DESALT_NEWTYPE_NONMEMBER_WITH_ARGUMENT_II(f, sig, name, base)

// namespace operator
// #define DESALT_NEWTYPE_F_KW_operator ,
// #define DESALT_NEWTYPE_F_GET_MACRO_operator DESALT_NEWTYPE_NONMEMBER_OPERATOR,
/*
#define DESALT_NEWTYPE_NONMEMBER_OPERATOR(op, name, base) \
    template<typename L, typename R, \
             typename BaseL = typename \
                 desalt::newtype::unwrap_if_derived<L, name, base>::type, \
             typename BaseR = typename \
                 desalt::newtype::unwrap_if_derived<R, name, base>::type, \
             typename BaseRes = \
                 decltype(std::declval<BaseL>() op std::declval<BaseR>()), \
             typename Res = typename \
                 desalt::newtype::wrap_if_base< \
                     BaseRes, name, base>::type> \
    friend Res f(L && l, R && r) { \
        using desalt::newtype::is_same_unqualified; \
        static_assert(!is_same_unqualified<L, base>::value && !is_same_unqualified<R, base>::value, ""); \
        return static_cast<Res>(std::forward<L>(l) op std::forward<R>(r)); \
    }
*/

// namespace explicit operator (only binary operators)
#define DESALT_NEWTYPE_F_E_KW_operator ,
#define DESALT_NEWTYPE_F_E_GET_MACRO_operator DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT,
#define DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT(op_sig, name, base) \
    DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_I(DESALT_NEWTYPE_GET_NAME() op_sig, BOOST_PP_TUPLE_EAT() op_sig, name, base)
#define DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_I(op, sig, name, base) \
    DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_II(op, BOOST_PP_TUPLE_REM() (boost::mpl::vector<BOOST_PP_TUPLE_REM() sig>), name, base)
#define DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_II(op, vec, name, base) \
    DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_III( \
        op, \
        BOOST_PP_TUPLE_REM() (typename boost::mpl::at_c<vec, 0>::type), \
        BOOST_PP_TUPLE_REM() (typename boost::mpl::at_c<vec, 1>::type), \
        name, \
        base)
#define DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_III(op, ltype, rtype, name, base) \
    DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_IV( \
        op, \
        BOOST_PP_TUPLE_REM() ( \
            typename desalt::newtype::wrap_if_base< \
                decltype(std::declval<typename desalt::newtype::unwrap_if_derived<ltype, name, base>::type>() \
                      BOOST_PP_TUPLE_REM() op std::declval<typename desalt::newtype::unwrap_if_derived<rtype, name, base>::type>()), \
                name, base>::type), \
        BOOST_PP_TUPLE_REM() (ltype), \
        BOOST_PP_TUPLE_REM() (rtype), \
        name, \
        base)
#define DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_IV(op, restype, ltype, rtype, name, base) \
    friend restype operator BOOST_PP_TUPLE_REM() op(ltype l, rtype r) { \
        return desalt::newtype::wrapper_access::unwrap<name, base>(l) \
            BOOST_PP_TUPLE_REM() op desalt::newtype::wrapper_access::unwrap<name, base>(r); \
    }

#define DESALT_NEWTYPE_GET_NAME() \
    DESALT_NEWTYPE_GET_NAME_I
#define DESALT_NEWTYPE_GET_NAME_I(op) \
    (op) BOOST_PP_TUPLE_EAT()


#define DESALT_NEWTYPE_KW_new ,
#define DESALT_NEWTYPE_GET_MACRO_new DESALT_NEWTYPE_NEW_MEMBER,
#define DESALT_NEWTYPE_NEW_MEMBER(def, name, base) \
    BOOST_PP_TUPLE_REM() def

namespace desalt { namespace newtype {
namespace mpl = boost::mpl;

template<typename T>
struct unqualify
    : std::remove_cv<typename
          std::remove_pointer<typename
              std::decay<T>::type>::type>
{};

template<typename T, typename U>
struct is_same_unqualified
    : std::is_same<typename unqualify<T>::type, U>
{};

template<typename T, typename U>
struct copy_qualifier {
    using type = U;
};
template<typename T, typename U>
struct copy_qualifier<T &, U> : mpl::identity<typename copy_qualifier<T, U>::type &> {};
template<typename T, typename U>
struct copy_qualifier<T &&, U> : mpl::identity<typename copy_qualifier<T, U>::type &&> {};
template<typename T, typename U>
struct copy_qualifier<T *, U> : mpl::identity<typename copy_qualifier<T, U>::type *> {};
template<typename T, typename U>
struct copy_qualifier<T const, U> : mpl::identity<typename copy_qualifier<T, U>::type const> {};
template<typename T, typename U>
struct copy_qualifier<T volatile, U> : mpl::identity<typename copy_qualifier<T, U>::type volatile> {};
template<typename T, typename U>
struct copy_qualifier<T const volatile, U> : mpl::identity<typename copy_qualifier<T, U>::type const volatile> {};

template<typename T, typename Derived, typename Base>
struct wrap_if_base
    : mpl::eval_if<is_same_unqualified<T, Base>,
                   copy_qualifier<T, Derived>,
                   mpl::identity<T>>
{};
template<typename T, typename Derived, typename Base>
struct unwrap_if_derived
    : mpl::eval_if<is_same_unqualified<T, Derived>,
                   copy_qualifier<T, Base>,
                   mpl::identity<T>>
{};

struct wrapper_access {
    template<typename Derived, typename Base, typename T>
    static typename wrap_if_base<T&&, Derived, Base>::type wrap(T && t) {
        return typename wrap_if_base<T&&, Derived, Base>::type(std::forward<T>(t));
    }
    template<typename Derived, typename Base, typename T>
    static typename unwrap_if_derived<T&&, Derived, Base>::type unwrap(T && t) {
        return std::forward<T>(t);
    }
};

template<typename T>
struct base_wrapper : T {
    using T::T;
};

enum class overload_resolution_barrier { value };

}}

#endif
