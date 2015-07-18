#if !defined STATIC_CONTROL_MACRO_HPP_INCLUDED_
#define      STATIC_CONTROL_MACRO_HPP_INCLUDED_

#include <type_traits>
#include <utility>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/is_empty.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/tuple/rem.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <desalt/preprocessor/is_keyword.hpp>
#include <desalt/static_control/detail/is_valid.hpp>
#include <desalt/static_control/detail/wrap.hpp>


#define DESALT_STATIC_MATCH(...) \
    desalt::static_control::detail::static_match_helper() = desalt::static_control::detail::begin_clause<__VA_ARGS__>()
#define DESALT_CASE(...) DESALT_STATIC_MATCH_CASE_I(DESALT_STATIC_MATCH_SPLIT(__VA_ARGS__))
#define DESALT_STATIC_MATCH_CASE_I(a) DESALT_STATIC_MATCH_CASE_II(a)
#define DESALT_STATIC_MATCH_CASE_II(vars, conds, ...) \
    ->* (desalt::static_control::detail::case_tag *)nullptr \
    ->* [&] vars -> typename std::enable_if<decltype(desalt::static_control::detail::condition<BOOST_PP_TUPLE_REM() conds>(0))::value>::type {} \
    ->* [&] vars

#define DESALT_DEFAULT DESALT_CASE(auto ...)
#define DESALT_STATIC_MATCH_SPLIT(...) \
    (BOOST_PP_SEQ_FOR_EACH_I(DESALT_STATIC_MATCH_SPLIT_M, ~, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))), (), ()
#define DESALT_STATIC_MATCH_SPLIT_M(r, d, i, e) \
    BOOST_PP_CAT(DESALT_STATIC_MATCH_SPLIT_M_I_, DESALT_PP_IS_KEYWORD(DESALT_STATIC_MATCH_IS_KW_IF, e))(i, e)
#define DESALT_STATIC_MATCH_SPLIT_M_I_0(i, e) BOOST_PP_COMMA_IF(i) e
#define DESALT_STATIC_MATCH_SPLIT_M_I_1(i, e) ), (BOOST_PP_CAT(DESALT_STATIC_MATCH_REMOVE_IF, e)
#define DESALT_STATIC_MATCH_IS_KW_IFif ,
#define DESALT_STATIC_MATCH_REMOVE_IFif


namespace desalt { namespace static_control { namespace detail {
namespace here = detail;

struct static_match_helper;
template<typename ...> struct begin_clause;
struct case_tag;
template<typename ...> struct case_condition;
template<typename ...> struct case_next;
template<typename ...> struct case_take;
template<typename, typename ...> struct matched;
template<typename T, typename = std::enable_if_t<!T::value>> constexpr std::false_type ok(int);
template<typename> constexpr std::true_type ok(...);
template<typename ...Bs> constexpr bool all(bool b, Bs ...bs);
constexpr bool all();
template<typename ...Ts, typename = std::enable_if_t<here::all(here::ok<Ts>(0).value...)>> constexpr std::true_type condition(int);
template<typename ...> constexpr std::false_type condition(...);


struct static_match_helper {
    template<typename F, typename ...Ts>
    auto operator=(matched<F, Ts...> && r) const { return std::forward<matched<F, Ts...>>(r).get(); }
    template<typename ...Ts>
    void operator=(begin_clause<Ts...>) const {}
};
template<typename ...Ts> struct begin_clause {
    constexpr case_condition<Ts...> operator->*(case_tag *) const { return {}; }
};
template<typename ...Ts> struct case_condition {
    template<typename P>
    constexpr auto operator->*(P) const {
        return dispatch(here::is_valid<P, wrap<Ts>...>(0));
    }
    constexpr case_take<Ts...> dispatch(std::true_type) const {
        return {};
    }
    constexpr case_next<Ts...> dispatch(std::false_type) const {
        return {};
    }
};
template<typename ...Ts> struct case_take {
    template<typename F> constexpr auto operator->*(F f) const { return matched<F, Ts...>{f}; }
};
template<typename ...Ts> struct case_next {
    template<typename F> constexpr auto operator->*(F) const { return begin_clause<Ts...>{}; }
};
template<typename F, typename ...Ts>
struct matched {
    constexpr auto get() const { return f(wrap<Ts>{}...); }
    template<typename G> constexpr matched operator->*(G) { return *this; }
    F f;
};

template<typename ..., typename> constexpr std::true_type condition(int) { return {}; }
template<typename ...> constexpr std::false_type condition(...) { return {}; }
template<typename, typename> constexpr std::false_type ok(int) { return {}; }
template<typename> constexpr std::true_type ok(...) { return {}; }
template<typename ...Bs> constexpr bool all(bool b, Bs ...bs) { return b && here::all(bs...); }
constexpr bool all() { return true; }

}}} // namespace desalt { namespace detail { namespace static_control {

#endif
