#ifndef DESALT_MATCH_ADAPTORS_HPP_INCLUDED_
#define DESALT_MATCH_ADAPTORS_HPP_INCLUDED_

#include <desalt/match/fwd.hpp>
#include <tuple>
#include <variant>
#include <vector>
#include <map>

namespace desalt::match {

namespace detail {

template<typename T, typename U,
    typename R = std::remove_reference_t<U>,
    typename Result = std::conditional_t<std::is_lvalue_reference_v<T>, R&, R&&>>
Result forward_partially(U && x) {
    return static_cast<Result>(x);
}

}

namespace traits {

template<typename ...Ts>
struct pattern<std::tuple<Ts...>> {
    template<typename Tag, typename T> static std::true_type test(T const &) { return {}; }
    template<std::size_t I, typename T> static std::true_type test(T const &) { return {}; }
    template<typename Tag, typename T>
    static decltype(auto) get(T && v) {
        return match::detail::forward_partially<T>(std::get<Tag>(v));
    }
    template<std::size_t I, typename T>
    static decltype(auto) get(T && v) {
        return match::detail::forward_partially<T>(std::get<I>(v));
    }
};

template<typename ...Ts>
struct pattern<std::variant<Ts...>> {
    template<typename Tag, typename T> static bool test(T const & v) { return std::holds_alternative<Tag>(v); }
    template<std::size_t I, typename T> static bool test(T const & v) { return v.index() == I; }
    template<typename Tag, typename T> static decltype(auto) get(T && v) {
        return std::get<Tag>(std::forward<T>(v));
    }
    template<std::size_t I, typename T> static decltype(auto) get(T && v) {
        return std::get<I>(std::forward<T>(v));
    }
};

template<typename E>
struct pattern<std::vector<E>> {
    using size_type = typename std::vector<E>::size_type;
    template<typename Index, typename T> static bool test(Index const & i, T const & v) { return 0 <= (size_type)i && (size_type)i < v.size(); }
    template<typename Index, typename T> static decltype(auto) get(Index const & i, T && v) {
        return std::forward<T>(v)[(size_type)i];
    }
};
template<typename Key, typename Value>
struct pattern<std::map<Key, Value>> {
    using size_type = typename std::map<Key, Value>::size_type;
    template<typename K, typename T> static bool test(K const & k, T const & v) { return v.find(k) != v.end(); }
    template<typename K, typename T> static decltype(auto) get(K const & k, T && v) {
        return std::forward<T>(v).at(k);
    }
};

}

}

#endif
