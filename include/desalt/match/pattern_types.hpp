#ifndef DESALT_MATCH_PATTERN_TYPES_HPP_INCLUDED_
#define DESALT_MATCH_PATTERN_TYPES_HPP_INCLUDED_

#include <desalt/match/fwd.hpp>

namespace desalt::match::detail {

// pattern types
template<typename Pattern, typename ...Patterns>
struct structured_pattern :
    bound_pattern_op<structured_pattern<Pattern, Patterns...>>
{
    using pattern_tag = structured_pattern_tag;
    using base_pattern = Pattern;
    static constexpr std::size_t size = sizeof...(Patterns);
    template<typename P, typename ...Ps>
    structured_pattern(P && p, Ps && ...ps)
        : pattern(std::forward<P>(p)), patterns(std::forward<Ps>(ps)...)
    {}
    Pattern pattern;
    std::tuple<Patterns...> patterns;
};
template<typename Pattern>
struct bound_pattern {
    using pattern_tag = bound_pattern_tag;
    using base_pattern = Pattern;
    Pattern pattern;
};
template<typename Pattern>
struct unbound_pattern {
    using pattern_tag = unbound_pattern_tag;
    using base_pattern = Pattern;
    Pattern pattern;
};
template<typename Pattern, typename T>
struct value_pattern :
    structured_pattern_op<value_pattern<Pattern, T>>,
    bound_pattern_op<value_pattern<Pattern, T>>
{
    using pattern_tag = value_pattern_tag;
    using base_pattern = Pattern;
    template<typename P, typename U>
    value_pattern(P && p, U && v)
        : pattern(std::forward<P>(p)), val(std::forward<U>(v))
    {}
    Pattern pattern;
    T val;
};
template<typename Tag>
struct tag_pattern :
    value_pattern_op<tag_pattern<Tag>>,
    structured_pattern_op<tag_pattern<Tag>>,
    unbound_pattern_op<tag_pattern<Tag>>
{
    using pattern_tag = tag_pattern_tag;
    using tag = Tag;
};
template<std::size_t I>
struct index_pattern :
    value_pattern_op<index_pattern<I>>,
    structured_pattern_op<index_pattern<I>>,
    unbound_pattern_op<index_pattern<I>>
{
    using pattern_tag = index_pattern_tag;
    static constexpr std::size_t index = I;
};
template<typename Key>
struct key_pattern :
    value_pattern_op<key_pattern<Key>>,
    structured_pattern_op<key_pattern<Key>>,
    unbound_pattern_op<key_pattern<Key>>
{
    using pattern_tag = key_pattern_tag;
    template<typename K>
    key_pattern(K && key) : key(std::forward<K>(key)) {}
    Key key;
};

// tags
struct bound_pattern_tag {};
struct unbound_pattern_tag {};
struct structured_pattern_tag {};
struct value_pattern_tag {};
struct tag_pattern_tag {};
struct index_pattern_tag {};
struct key_pattern_tag {};

// operator helpers
template<typename Pattern>
struct structured_pattern_op {
    template<typename ...Patterns>
    structured_pattern<Pattern, std::decay_t<Patterns>...>
    operator()(Patterns && ...patterns) const {
        return {static_cast<Pattern const &>(*this), std::forward<Patterns>(patterns)...};
    }
};
template<typename Pattern>
struct bound_pattern_op {
    bound_pattern<Pattern> operator+() const {
        return {static_cast<Pattern const &>(*this)};
    }
};
template<typename Pattern>
struct unbound_pattern_op {
    unbound_pattern<Pattern> operator-() const {
        return {static_cast<Pattern const &>(*this)};
    }
};
template<typename Pattern>
struct value_pattern_op {
    template<typename T>
    value_pattern<Pattern, std::decay_t<T>> operator[](T && v) const {
        return {static_cast<Pattern const &>(*this), std::forward<T>(v)};
    }
};

}

#endif
