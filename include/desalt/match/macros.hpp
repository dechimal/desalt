#ifndef DESALT_MATCH_MACRO_HPP_INCLUDED_
#define DESALT_MATCH_MACRO_HPP_INCLUDED_

#include <desalt/match/match.hpp>
#include <desalt/preprocessor/is_empty.hpp>
#include <boost/preprocessor/control/iif.hpp>
#include <boost/preprocessor/tuple/rem.hpp>
#include <boost/preprocessor/tuple/eat.hpp>

namespace desalt::match::detail {

template<std::size_t I> struct counter_helper_t : counter_helper_t<I-1> {};
template<> struct counter_helper_t<0> {};

template struct counter_helper_t<500>;
constexpr counter_helper_t<1024> counter_limit{};

template<typename T, typename Patterns, std::size_t ...Is>
auto macro_test_impl(T const & obj, Patterns const & patterns, std::index_sequence<Is...>) {
    return here::test(obj, std::get<Is>(patterns)...);
}

template<typename T, typename ...Patterns>
auto macro_test(T const & obj, std::tuple<Patterns...> const & patterns) {
    return here::macro_test_impl(obj, patterns, std::index_sequence_for<Patterns...>{});
}

template<typename T, typename Patterns, std::size_t ...Is>
auto macro_get_impl(T && obj, Patterns const & patterns, std::index_sequence<Is...>) {
    return here::get(std::forward<T>(obj), std::get<Is>(patterns)...);
}

template<typename T, typename ...Patterns>
auto macro_get(T && obj, std::tuple<Patterns...> const & patterns) {
    return here::macro_get_impl(std::forward<T>(obj), patterns, std::index_sequence_for<Patterns...>{});
}

}

#define DESALT_MATCH_COUNTER_GET() (sizeof(desalt_match_case_clause_counter_(desalt::match::detail::counter_limit)) - 1)
#define DESALT_MATCH_COUNTER_INIT() char (&desalt_match_case_clause_counter_(desalt::match::detail::counter_helper_t<0>))[1]
#define DESALT_MATCH_COUNTER_INC() char (&desalt_match_case_clause_counter_(desalt::match::detail::counter_helper_t<DESALT_MATCH_COUNTER_GET()+1>))[DESALT_MATCH_COUNTER_GET()+2]

#define DESALT_MATCH(...) \
if (auto && _desalt_match_object_ = (__VA_ARGS__); false) {} \
else if (DESALT_MATCH_COUNTER_INIT(); false) {} \
else if (std::size_t _desalt_match_current_clause_ = 0; false) {} \
else for (bool _desalt_match_case_entered_ = true, _desalt_match_case_continued_ = true; \
    _desalt_match_case_continued_ && _desalt_match_case_continued_;) \
switch (_desalt_match_case_entered_ = _desalt_match_case_continued_ = false, \
    _desalt_match_current_clause_++) \
case (std::size_t)-1: for (; false; _desalt_match_case_continued_ = true)

#define DESALT_CASE(...) \
break; \
case DESALT_MATCH_COUNTER_GET(): \
DESALT_MATCH_COUNTER_INC(); \
BOOST_PP_IIF(DESALT_PP_IS_EMPTY(__VA_ARGS__), BOOST_PP_TUPLE_EAT, BOOST_PP_TUPLE_REM)() \
(if (auto && _desalt_match_case_patterns_ = std::tuple(__VA_ARGS__); false) {} \
else if (!desalt::match::detail::macro_test(_desalt_match_object_, _desalt_match_case_patterns_)) \
    continue; \
else) if (!(_desalt_match_case_entered_ = true)) {} \
else DESALT_CASE_DECOMPOSITE

#define DESALT_CASE_DECOMPOSITE(...) \
BOOST_PP_IIF(DESALT_PP_IS_EMPTY(__VA_ARGS__), BOOST_PP_TUPLE_EAT, BOOST_PP_TUPLE_REM)() \
(if (auto && [__VA_ARGS__] = \
    desalt::match::detail::macro_get(static_cast<decltype(_desalt_match_object_)>(_desalt_match_object_), \
    _desalt_match_case_patterns_); false) {} \
else)

#endif

// macro MATCH(e) overview
//
// if (auto obj = e; false) {}
// else if (COUNTER_INIT(); false) {}
// else switch (std::size_t clause = 0)
// for (bool enteted, cont; entered && cont;)
// case 0: switch (entered = cont = false, clause++)
// for (; false; cont = true)

// macro CASE(pattern...) overview
// 
// break;
// case COUNTER_GET():
// COUNTER_INC();
// if (auto ps = tuple(patterns)) {}
// else if (!test(obj, ps)) continue;
// else if (!(entered = true)) {}
// else if (auto && [vars] = get(obj, ps); true)
//     ...;
