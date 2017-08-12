#ifndef DESALT_MATCH_FWD_HPP_INCLUDED_
#define DESALT_MATCH_FWD_HPP_INCLUDED_

#if __cplusplus < 201700
#error "This library requires feature of C++1z"
#endif

namespace desalt::match {
namespace detail {
namespace here = detail;

template<typename T, typename Patterns, typename Indexes> struct match_result;

template<typename Pattern, typename ...Patterns> struct structured_pattern;
template<typename Pattern> struct bound_pattern;
template<typename Pattern> struct unbound_pattern;
template<typename Pattern, typename T> struct value_pattern;
template<typename Tag> struct tag_pattern;
template<std::size_t I> struct index_pattern;
template<typename Key> struct key_pattern;

struct bound_pattern_tag;
struct unbound_pattern_tag;
struct structured_pattern_tag;
struct value_pattern_tag;
struct tag_pattern_tag;
struct index_pattern_tag;
struct key_pattern_tag;

template<typename Pattern> struct structured_pattern_op;
template<typename Pattern> struct bound_pattern_op;
template<typename Pattern> struct unbound_pattern_op;
template<typename Pattern> struct value_pattern_op;

template<typename Pattern> constexpr int has_binding();

template<typename T, typename  ...Patterns> auto match(T && v, Patterns && ...patterns);

template<typename T, typename Pattern> auto test(T const & v, Pattern const & pattern);
template<typename T, typename Structured, std::size_t ...Is> auto test_structured_pattern(T const & v, Structured const & pattern, std::index_sequence<Is...>);

template<int HasBinding = -1, typename T, typename Pattern> auto get(T && v, Pattern const & pattern);
template<bool HasBinding, typename T, typename Structured, std::size_t ...Is> auto get_structured_pattern(T && v, Structured const & pattern, std::index_sequence<Is...>);

template<typename T, typename Pattern> decltype(auto) get_field(T && v, Pattern const & pattern);

template<typename Tag> constexpr tag_pattern<Tag> tag{};
template<typename Key> key_pattern<Key> at(Key && k);

template<typename Pattern, typename ...Tags>
constexpr bool kind = (std::is_same_v<typename std::remove_cv_t<std::remove_reference_t<Pattern>>::pattern_tag, Tags> || ...);

}

using detail::match;
using detail::tag;
using detail::at;

namespace traits {
template<typename T> struct pattern;
}

namespace literals {
template<char ...Cs> constexpr auto operator""_();
}

}

#endif
