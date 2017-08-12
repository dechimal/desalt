#ifndef DESALT_MATCH_MATCH_HPP_INCLUDED_
#define DESALT_MATCH_MATCH_HPP_INCLUDED_

#include <type_traits>
#include <utility>
#include <tuple>
#include <desalt/match/fwd.hpp>
#include <desalt/match/pattern_types.hpp>

namespace desalt::match {
namespace detail {
namespace here = detail;

// definitions

// match_result
template<typename T, typename Patterns, std::size_t ...Is>
struct match_result<T, Patterns, std::index_sequence<Is...>> {
    explicit operator bool() const {
        return (here::test(val, std::get<Is>(patterns)) && ...);
    }
    auto operator*() {
        return std::tuple_cat(here::get(std::forward<T>(val), std::get<Is>(patterns))...);
    }
    T val;
    Patterns patterns;
};

// match
template<typename T, typename ...Patterns>
auto match(T && v, Patterns && ...patterns) {
    using type = match_result<
        T,
        std::tuple<std::decay_t<Patterns>...>,
        std::index_sequence_for<Patterns...>
    >;
    return type {
        std::forward<T>(v),
        std::tuple{std::forward<Patterns>(patterns)...},
    };
}

// test
template<typename T, typename Pattern>
auto test(T const & v, Pattern const & pattern) {
    if constexpr (kind<Pattern, structured_pattern_tag>) {
        return here::test_structured_pattern(v, pattern, std::make_index_sequence<pattern.size>{});
    } else if constexpr (kind<Pattern, value_pattern_tag>) {
        return here::test(v, pattern.pattern) && here::get_field(v, pattern.pattern) == pattern.val;
    } else if constexpr (kind<Pattern, tag_pattern_tag>) {
        return traits::pattern<std::decay_t<T>>::template test<typename std::remove_cv_t<std::remove_reference_t<Pattern>>::tag>(v);
    } else if constexpr (kind<Pattern, index_pattern_tag>) {
        return traits::pattern<std::decay_t<T>>::template test<pattern.index>(v);
    } else if constexpr (kind<Pattern, key_pattern_tag>) {
        return traits::pattern<std::decay_t<T>>::test(pattern.key, v);
    } else if constexpr (kind<Pattern, bound_pattern_tag, unbound_pattern_tag>) {
        return here::test(v, pattern.pattern);
    } else {
        static_assert(!std::is_same_v<Pattern, Pattern>);
    }
}
template<typename T, typename Structured, std::size_t ...Is>
auto test_structured_pattern(T const & v, Structured const & pattern, std::index_sequence<Is...>) {
    if (here::test(v, pattern.pattern)) {
        auto const & w = here::get_field(v, pattern.pattern);
        return (... && here::test(w, std::get<Is>(pattern.patterns)));
    } else {
        return false;
    }
}

// get
template<int HasBinding, typename T, typename Pattern>
auto get(T && v, Pattern const & pattern) {
    constexpr bool has_binding = (HasBinding == -1 ? here::has_binding<Pattern>() >= 0 : (bool)HasBinding);
    if constexpr (kind<Pattern, structured_pattern_tag>) {
        return here::get_structured_pattern<has_binding>(std::forward<T>(v), pattern, std::make_index_sequence<pattern.size>{});
    } else if constexpr (kind<Pattern, bound_pattern_tag, unbound_pattern_tag, value_pattern_tag>) {
        return here::get<has_binding>(std::forward<T>(v), pattern.pattern);
    } else if constexpr (kind<Pattern, tag_pattern_tag, index_pattern_tag, key_pattern_tag>) {
        if constexpr (has_binding) {
            return std::forward_as_tuple(get_field(std::forward<T>(v), pattern));
        } else {
            return std::tuple<>{};
        }
    } else {
        static_assert(!sizeof(Pattern));
    }
}
template<bool HasBinding, typename T, typename Structured, std::size_t ...Is>
auto get_structured_pattern(T && v, Structured const & pattern, std::index_sequence<Is...>) {
    auto && field = here::get_field(std::forward<T>(v), pattern.pattern);
    using field_type = decltype(field);
    auto bindings = std::tuple_cat(here::get(std::forward<field_type>(field), std::get<Is>(pattern.patterns))...);
    if constexpr (HasBinding) {
        return std::tuple_cat(std::forward_as_tuple(std::forward<field_type>(field)), std::move(bindings));
    } else {
        return bindings;
    }
}

// get_field
template<typename T, typename Pattern>
decltype(auto) get_field(T && v, Pattern const & pattern) {
    if constexpr (kind<Pattern, tag_pattern_tag>) {
        return traits::pattern<std::decay_t<T>>::template get<typename std::remove_cv_t<std::remove_reference_t<Pattern>>::tag>(std::forward<T>(v));
    } else if constexpr (kind<Pattern, index_pattern_tag>) {
        return traits::pattern<std::decay_t<T>>::template get<pattern.index>(std::forward<T>(v));
    } else if constexpr (kind<Pattern, key_pattern_tag>) {
        return traits::pattern<std::decay_t<T>>::get(pattern.key, std::forward<T>(v));
    } else if constexpr (kind<Pattern, bound_pattern_tag, unbound_pattern_tag, structured_pattern_tag, value_pattern_tag>) {
        return here::get_field(std::forward<T>(v), pattern.pattern);
    } else {
        static_assert(!sizeof(Pattern));
    }
}

// has_binding
template<typename Pattern>
constexpr int has_binding() {
    if constexpr (kind<Pattern, bound_pattern_tag>) {
        return 1;
    } else if constexpr (kind<Pattern, unbound_pattern_tag>) {
        return -1;
    } else if constexpr (kind<Pattern, tag_pattern_tag, index_pattern_tag, key_pattern_tag>) {
        return 0;
    } else if constexpr (kind<Pattern, structured_pattern_tag, value_pattern_tag>) {
        return here::has_binding<typename std::remove_cv_t<std::remove_reference_t<Pattern>>::base_pattern>() > 0 ? 1 : -1;
    } else {
        static_assert(!sizeof(Pattern));
    }
}

// at
template<typename Key>
key_pattern<Key> at(Key && key) { return {std::forward<Key>(key)}; }

}

}

#endif
