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
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/seq/to_tuple.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

#include <desalt/require.hpp>
#include <desalt/auto_fun.hpp>
#include <desalt/preprocessor/dispatch.hpp>
#include <desalt/preprocessor/is_empty.hpp>
#include <desalt/preprocessor/tuple/rem_if_paren.hpp>
#include <desalt/preprocessor/starts_with_enclosing_paren.hpp>

#define DESALT_NEWTYPE(wrapped, base, ...) \
    DESALT_NEWTYPE_I(wrapped, typename BOOST_IDENTITY_TYPE((DESALT_PP_TUPLE_REM_IF_PAREN(base) base)), \
                     __VA_ARGS__ \
                     BOOST_PP_COMMA_IF(BOOST_PP_EQUAL(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 1)))
#define DESALT_NEWTYPE_I(wrapped, base, ...) \
    DESALT_NEWTYPE_II(wrapped, base, __VA_ARGS__)
#define DESALT_NEWTYPE_II(wrapped, base, unwrap, ...) \
    struct wrapped : private desalt::newtype::base_wrapper<base> { \
        base & unwrap() noexcept { return *this; } \
        base const & unwrap() const noexcept { return *this; } \
        friend class desalt::newtype::wrapper_access; \
        explicit wrapped(base const & b) noexcept(noexcept(base(b))) : desalt::newtype::base_wrapper<base>(b) {} \
        explicit wrapped(base && b) noexcept(noexcept(base(std::move(b)))) : desalt::newtype::base_wrapper<base>(std::move(b)) {} \
        BOOST_PP_SEQ_FOR_EACH(DESALT_NEWTYPE_M, (wrapped, base), BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
    }

#define DESALT_NEWTYPE_M(r, d, op) \
    DESALT_NEWTYPE_M_I(op, BOOST_PP_TUPLE_REM() d)
#define DESALT_NEWTYPE_M_I(op, d) \
    DESALT_PP_DISPATCH(DESALT_NEWTYPE_KW_, DESALT_NEWTYPE_GET_MACRO_, DESALT_NEWTYPE_USING_OR_NOP, op)(DESALT_PP_DISPATCH_GET_ARG(DESALT_NEWTYPE_KW_, op), d)

#define DESALT_NEWTYPE_USING_OR_NOP(member, wrapped, base) \
    BOOST_PP_IIF(DESALT_PP_IS_EMPTY(member), \
                 BOOST_PP_TUPLE_EAT(), \
                 DESALT_NEWTYPE_USING)(member, wrapped, base)

#define DESALT_NEWTYPE_USING(member, wrapped, base) \
    using base::member;

#define DESALT_NEWTYPE_KW_this ,
#define DESALT_NEWTYPE_GET_MACRO_this DESALT_NEWTYPE_CTOR,
#define DESALT_NEWTYPE_CTOR(_, wrapped, base) \
    using desalt::newtype::base_wrapper<base>::base_wrapper;

#define DESALT_NEWTYPE_KW_typename ,
#define DESALT_NEWTYPE_GET_MACRO_typename DESALT_NEWTYPE_USING_TYPENAME,
#define DESALT_NEWTYPE_USING_TYPENAME(typename_, wrapped, base) \
    using base::typename_;

#define DESALT_NEWTYPE_KW_auto ,
#define DESALT_NEWTYPE_GET_MACRO_auto DESALT_NEWTYPE_WRAPPED,
#define DESALT_NEWTYPE_WRAPPED(member, wrapped, base) \
    DESALT_NEWTYPE_WRAPPED_I(member, wrapped, base,) \
    DESALT_NEWTYPE_WRAPPED_I(member, wrapped, base, const)

#define DESALT_NEWTYPE_WRAPPED_I(member, wrapped, base, cv) \
    template<typename ...Args, \
             DESALT_REQUIRE(boost::mpl::not_<boost::mpl::or_<desalt::newtype::is_same_unqualified<Args, base>...> >)> \
    auto member(Args && ...args) cv \
        -> decltype(desalt::newtype::wrapper_access::wrap<wrapped, base>( \
                        std::declval<base cv>().member( \
                            desalt::newtype::wrapper_access::unwrap<wrapped, base>( \
                                std::forward<Args>(args)...)))) \
    { \
        return desalt::newtype::wrapper_access::wrap<wrapped, base>( \
                   desalt::newtype::base_wrapper<base>::member( \
                       desalt::newtype::wrapper_access::unwrap<wrapped, base>( \
                           std::forward<Args>(args)...))); \
    }

// explicit
#define DESALT_NEWTYPE_KW_explicit ,
#define DESALT_NEWTYPE_GET_MACRO_explicit DESALT_NEWTYPE_WITH_ARGUMENT,
#define DESALT_NEWTYPE_WITH_ARGUMENT(member_sig, wrapped, base) \
    DESALT_NEWTYPE_WITH_ARGUMENT_I(DESALT_NEWTYPE_GET_NAME() member_sig, BOOST_PP_TUPLE_EAT() member_sig, wrapped, base)
#define DESALT_NEWTYPE_WITH_ARGUMENT_I(member, sig, wrapped, base) \
    DESALT_NEWTYPE_WITH_ARGUMENT_II(member, DESALT_NEWTYPE_WITH_ARGUMENT_GEN_PARAM_LIST(sig), DESALT_NEWTYPE_WITH_ARGUMENT_GEN_FORWARD_LIST(sig), BOOST_PP_TUPLE_EAT() sig, wrapped, base)
#define DESALT_NEWTYPE_WITH_ARGUMENT_II(member, params, forwards, qualifiers, wrapped, base) \
    DESALT_AUTO_FUN(member(params) qualifiers, this->desalt::newtype::base_wrapper<base>::member(forwards));

#define DESALT_NEWTYPE_WITH_ARGUMENT_GEN_PARAM_LIST(sig) \
    BOOST_PP_SEQ_FOR_EACH_I(DESALT_NEWTYPE_WITH_ARGUMENT_GEN_PARAM_LIST_M, ~, BOOST_PP_TUPLE_TO_SEQ(sig))
#define DESALT_NEWTYPE_WITH_ARGUMENT_GEN_PARAM_LIST_M(r, d, i, param_type) \
    BOOST_PP_COMMA_IF(i) DESALT_PP_TUPLE_REM_IF_PAREN(param_type) param_type BOOST_PP_CAT(a, i)
#define DESALT_NEWTYPE_WITH_ARGUMENT_GEN_FORWARD_LIST(sig) \
    BOOST_PP_SEQ_FOR_EACH_I(DESALT_NEWTYPE_WITH_ARGUMENT_GEN_FORWARD_LIST_M, ~, BOOST_PP_TUPLE_TO_SEQ(sig))
#define DESALT_NEWTYPE_WITH_ARGUMENT_GEN_FORWARD_LIST_M(r, d, i, param_type) \
    BOOST_PP_COMMA_IF(i) std::forward<DESALT_PP_TUPLE_REM_IF_PAREN(param_type) param_type>(BOOST_PP_CAT(a, i))

// friend
#define DESALT_NEWTYPE_KW_friend ,
#define DESALT_NEWTYPE_GET_MACRO_friend DESALT_NEWTYPE_FRIEND,
#define DESALT_NEWTYPE_FRIEND(friend_, wrapped, base) \
    DESALT_PP_DISPATCH(DESALT_NEWTYPE_F_KW_, DESALT_NEWTYPE_F_GET_MACRO_, DESALT_NEWTYPE_FRIEND_I, friend_)(DESALT_PP_DISPATCH_GET_ARG(DESALT_NEWTYPE_F_KW_, friend_), wrapped, base)

#define DESALT_NEWTYPE_FRIEND_I(friend_, wrapped, base) \
    friend DESALT_PP_TUPLE_REM_IF_PAREN(friend_) friend_;

#define DESALT_NEWTYPE_F_KW_class ,
#define DESALT_NEWTYPE_F_GET_MACRO_class DESALT_NEWTYPE_FRIEND_CLASS,
#define DESALT_NEWTYPE_FRIEND_CLASS(class_, wrapped, base) \
    friend class DESALT_PP_TUPLE_REM_IF_PAREN(class_) class_;

// namespace
#define DESALT_NEWTYPE_KW_namespace ,
#define DESALT_NEWTYPE_GET_MACRO_namespace DESALT_NEWTYPE_NONMEMBER,
#define DESALT_NEWTYPE_NONMEMBER(f, wrapped, base) \
    DESALT_PP_DISPATCH(DESALT_NEWTYPE_NS_KW_, DESALT_NEWTYPE_NS_GET_MACRO_, DESALT_NEWTYPE_NONMEMBER_I, f)(DESALT_PP_DISPATCH_GET_ARG(DESALT_NEWTYPE_NS_KW_, f), wrapped, base)
/*
#define DESALT_NEWTYPE_NONMEMBER_I(_, wrapped, base) \
    template<typename ...Args, \
             typename BaseRes = \
                 decltype(f(std::declval<typename desalt::newtype::unwrap_if_derived< \
                                         Args, wrapped, base>::type>()...)), \
             typename Res = typename \
                 desalt::newtype::wrap_if_base< \
                     BaseRes, wrapped, base>::type> \
    friend Res f(Args && ...args) { \
        using desalt::newtype::is_same_unqualified; \
        static_assert(!boost::mpl::or_<is_same_unqualified<Args, base>...>::value, ""); \
        return static_cast<Res>(f(std::forward<Args>(args)...)); \
    }
*/
// namespace explicit
#define DESALT_NEWTYPE_NS_KW_explicit ,
#define DESALT_NEWTYPE_NS_GET_MACRO_explicit DESALT_NEWTYPE_NONMEMBER_WITH_ARGUMENT,
#define DESALT_NEWTYPE_NONMEMBER_WITH_ARGUMENT(f_sig, wrapped, base) \
    DESALT_PP_DISPATCH(DESALT_NEWTYPE_NS_E_KW_, DESALT_NEWTYPE_NS_E_GET_MACRO_, DESALT_NEWTYPE_NONMEMBER_WITH_ARGUMENT_II, f_sig)(DESALT_PP_DISPATCH_GET_ARG(DESALT_NEWTYPE_NS_E_KW_, f_sig), wrapped, base)

// #define DESALT_NEWTYPE_NONMEMBER_WITH_ARGUMENT_II(f, sig, wrapped, base)

// namespace operator (only binary operators)
#define DESALT_NEWTYPE_NS_KW_operator ,
#define DESALT_NEWTYPE_NS_GET_MACRO_operator DESALT_NEWTYPE_NONMEMBER_OPERATOR,

#define DESALT_NEWTYPE_NONMEMBER_OPERATOR(op, wrapped, base) \
    template<typename L, typename R, \
             DESALT_REQUIRE(boost::mpl::or_<desalt::newtype::is_same_unqualified<L, wrapped>, desalt::newtype::is_same_unqualified<R, wrapped>>), \
             DESALT_REQUIRE(boost::mpl::not_<boost::mpl::or_<desalt::newtype::is_same_unqualified<L, base>, desalt::newtype::is_same_unqualified<R, base>>>)> \
    friend DESALT_AUTO_FUN(operator op(L && l, R && r), \
                           desalt::newtype::wrapper_access::wrap<wrapped, base>( \
                               desalt::newtype::wrapper_access::unwrap<wrapped, base>(l) \
                            op desalt::newtype::wrapper_access::unwrap<wrapped, base>(r)));


// namespace explicit operator (only binary operators)
#define DESALT_NEWTYPE_NS_E_KW_operator ,
#define DESALT_NEWTYPE_NS_E_GET_MACRO_operator DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT,
#define DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT(op_sig, wrapped, base) \
    DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_I((DESALT_NEWTYPE_GET_NAME() op_sig), BOOST_PP_TUPLE_EAT() op_sig, wrapped, base)
#define DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_I(op, sig, wrapped, base) \
    DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_II(op, BOOST_PP_TUPLE_REM() (boost::mpl::vector<BOOST_PP_TUPLE_REM() sig>), wrapped, base)
#define DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_II(op, vec, wrapped, base) \
    DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_III( \
        op, \
        BOOST_PP_TUPLE_REM() (typename boost::mpl::at_c<vec, 0>::type), \
        BOOST_PP_TUPLE_REM() (typename boost::mpl::at_c<vec, 1>::type), \
        wrapped, \
        base)
#define DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_III(op, ltype, rtype, wrapped, base) \
    DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_IV( \
        op, \
        BOOST_PP_TUPLE_REM() ( \
            typename desalt::newtype::wrap_if_base< \
                decltype(std::declval<typename desalt::newtype::unwrap_if_derived<ltype, wrapped, base>::type>() \
                      BOOST_PP_TUPLE_REM() op std::declval<typename desalt::newtype::unwrap_if_derived<rtype, wrapped, base>::type>()), \
                wrapped, base>::type), \
        BOOST_PP_TUPLE_REM() (ltype), \
        BOOST_PP_TUPLE_REM() (rtype), \
        wrapped, \
        base)
#define DESALT_NEWTYPE_NONMEMBER_OPERATOR_WITH_ARGUMENT_IV(op, restype, ltype, rtype, wrapped, base) \
    friend restype operator BOOST_PP_TUPLE_REM() op(ltype l, rtype r) { \
        return desalt::newtype::wrapper_access::unwrap<wrapped, base>(l) \
            BOOST_PP_TUPLE_REM() op desalt::newtype::wrapper_access::unwrap<wrapped, base>(r); \
    }

#define DESALT_NEWTYPE_GET_NAME() \
    DESALT_NEWTYPE_GET_NAME_I
#define DESALT_NEWTYPE_GET_NAME_I(name) \
    name BOOST_PP_TUPLE_EAT()


#define DESALT_NEWTYPE_KW_new ,
#define DESALT_NEWTYPE_GET_MACRO_new DESALT_NEWTYPE_NEW_MEMBER,
#define DESALT_NEWTYPE_NEW_MEMBER(def, wrapped, base) \
    DESALT_PP_TUPLE_REM_IF_PAREN(def) def

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

}}

#endif
