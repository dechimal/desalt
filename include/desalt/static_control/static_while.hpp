#if !defined STATIC_CONTROL_STATIC_WHILE_HPP_INCLUDED_
#define      STATIC_CONTROL_STATIC_WHILE_HPP_INCLUDED_

#include <tuple>
#include <desalt/static_control/with_index_sequence.hpp>
#include <desalt/static_control/with_unpacked.hpp>
#include <desalt/static_control/static_if.hpp>
#include <desalt/static_control/detail/split.hpp>
#include <desalt/static_control/wrap.hpp>

namespace desalt { namespace static_control {
namespace here = static_control;

template<typename ...Args> constexpr auto static_while(Args && ...);

namespace detail {
template<typename Step, std::size_t ...Is, typename States> constexpr auto static_while_impl(Step, std::index_sequence<Is...>, States &&);
template<typename Step> struct regular;
}

template<typename ...Args>
constexpr auto static_while(Args && ...args) {
    return detail::split([&] (auto step, auto && ...states) {
        auto tup = std::forward_as_tuple(std::forward<decltype(states)>(states)...);
        constexpr auto iseq = std::index_sequence_for<decltype(states)...>{};
        auto step1 = detail::regular<decltype(step)>(std::move(step));
        return detail::static_while_impl(std::move(step1), iseq, std::move(tup));
    }, std::forward<Args>(args)...);
}

namespace detail {
namespace here = detail;

template<typename Step, std::size_t ...Is, typename States>
constexpr auto static_while_impl(Step step, std::index_sequence<Is...>, States && states) {
    return static_control::static_match(step)([&] (auto step, wrap<decltype(step(std::get<Is>(std::forward<States>(states))...))> = {}) {
        auto r = step(std::get<Is>(std::forward<States>(states))...);
        return static_while_impl(std::move(step), std::make_index_sequence<std::tuple_size<decltype(r)>{}>{}, std::move(r));
    }, [&] (auto) {
        using result_type = std::tuple<
            std::conditional_t<
                std::is_rvalue_reference<std::tuple_element_t<Is, States>>{},
                std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<Is, States>>>,
                std::tuple_element_t<Is, States>
            >...
        >;
        return result_type{std::get<Is>(std::forward<States>(states))...};
    });
}

template<typename Step>
struct regular {
    constexpr regular(Step step) : step(step) {}
    template<typename ...States,
        typename = std::enable_if_t<std::is_same<decltype(std::declval<Step>()(std::declval<States>()...)), void>::value>>
    constexpr std::tuple<> operator()(States && ...states) {
        step(std::forward<States>(states)...);
        return {};
    }
    template<typename ...States,
        typename = std::enable_if_t<std::is_same<decltype(std::declval<Step>()(std::declval<States>()...)), void>::value>>
    constexpr std::tuple<> operator()(States && ...states) const {
        step(std::forward<States>(states)...);
        return {};
    }
    template<typename ...States,
        typename = std::enable_if_t<!std::is_same<decltype(std::declval<Step>()(std::declval<States>()...)), void>::value>,
        typename = void>
    constexpr auto operator()(States && ...states) {
        return regular::tuplize(step(std::forward<States>(states)...));
    }
    template<typename ...States,
        typename = std::enable_if_t<!std::is_same<decltype(std::declval<Step>()(std::declval<States>()...)), void>::value>,
        typename = void>
    constexpr auto operator()(States && ...states) const {
        return regular::tuplize(step(std::forward<States>(states)...));
    }
private:
    template<typename ...Ts>
    static constexpr std::tuple<Ts...> & tuplize(std::tuple<Ts...> & tup) {
        return std::forward<std::tuple<Ts...>>(tup);
    }
    template<typename ...Ts>
    static constexpr std::tuple<Ts...> const & tuplize(std::tuple<Ts...> const & tup) {
        return std::forward<std::tuple<Ts...>>(tup);
    }
    template<typename ...Ts>
    static constexpr std::tuple<Ts...> && tuplize(std::tuple<Ts...> && tup) {
        return std::forward<std::tuple<Ts...>>(tup);
    }
    template<typename ...Ts>
    static constexpr std::tuple<Ts...> const && tuplize(std::tuple<Ts...> const && tup) {
        return std::forward<std::tuple<Ts...>>(tup);
    }
    template<typename T>
    static constexpr auto tuplize(T && x) {
        return std::make_tuple(std::forward<T>(x));
    }

    Step step;
};

} // namespace detail {

}} // namespace desalt { namespace static_control {

#endif
