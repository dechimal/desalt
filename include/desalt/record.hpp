#include <utility>
#include <type_traits>
#include <tuple>

#define DESALT_MEMBER(name) DESALT_MEMBER_I(name, DESALT_PP_CAT(value_type, name), DESALT_PP_CAT(holder, name))
#define DESALT_MEMBER_I(name, value_type, holder) DESALT_MEMBER_II(name, value_type, holder)
#define DESALT_MEMBER_II(name, value_type, holder) \
    (::desalt::detail::record::member_id { \
        [] () -> auto const & { return DESALT_PP_STR(name); }, \
        [] (auto && m) { return ::desalt::detail::record::memptr<&std::decay_t<decltype(m)>::name>{}; }, \
        [] (auto && v) { \
            struct holder; \
            using value_type = ::desalt::detail::record::make_value_type<std::decay_t<decltype(v)>, struct holder>; \
            struct holder { value_type name; }; \
            return std::conditional<true, holder, void>{}; \
        } \
    })

#ifndef DESALT_PP_CAT
#define DESALT_PP_CAT(x, y) DESALT_PP_CAT_I(x, y)
#define DESALT_PP_CAT_I(x, y) x ## y
#endif

#ifndef DESALT_PP_STR
#define DESALT_PP_STR(x) DESALT_PP_STR_I(x)
#define DESALT_PP_STR_I(x) #x
#endif

namespace desalt {
namespace detail::record {
namespace here = record;

template<typename F>
constexpr auto fix(F f) {
    return [f] (auto && ...args) {
        return f(fix(f), std::forward<decltype(args)>(args)...);
    };
}

struct skip_one {};
struct skip_all {};

template<typename F, typename ...Args> auto valid_expr(F f, Args && ...args) -> decltype((void)f(std::forward<Args>(args)...), std::true_type{}) { return {}; }
std::false_type valid_expr(...) { return {}; }

template<typename ...Fs>
constexpr decltype(auto) ftie(Fs && ...fs) {
    auto impl = [] (auto & fs, auto && ...args) -> decltype(auto) {
        return std::apply(here::fix([&] (auto && ftie_impl, auto && g, auto && ...gs) -> decltype(auto) {
            if constexpr (!sizeof...(gs) || decltype(here::valid_expr(g, std::forward<decltype(args)>(args)...)){}) {
                using result_type = decltype(g(std::forward<decltype(args)>(args)...));
                if constexpr (std::is_same_v<result_type, skip_one>) {
                    return ftie_impl(std::forward<decltype(gs)>(gs)...);
                } else if constexpr (std::is_same_v<result_type, skip_all>) {
                    return skip_all{};
                } else {
                    return g(std::forward<decltype(args)>(args)...);
                }
            } else {
                return ftie_impl(std::forward<decltype(gs)>(gs)...);
            }
        }), std::move(fs));
    };
    auto gs = std::tuple<Fs...>{std::forward<Fs>(fs)...};
    return [impl, gs=std::move(gs)] (auto && ...args) -> std::enable_if_t<
        !std::is_same_v<decltype(impl(gs, std::forward<decltype(args)>(args)...)), skip_all>,
        decltype(impl(gs, std::forward<decltype(args)>(args)...))
    > {
        return impl(gs, std::forward<decltype(args)>(args)...);
    };
}

template<typename ...Defs> struct record;

template<typename Symbol, typename Access, typename Cons, typename Value, bool Fun> struct member_definition;
template<typename Def> using member_symbol = typename Def::symbol;
template<typename Def> using member_raw_value_type = typename Def::raw_value_type;
template<typename Def, typename Rec> using record_slice = typename Def::template record_slice<Rec>;
template<typename Def> using member_access = typename Def::member_access;
template<typename F, typename Holder> using make_value_type = typename F::template make_value_type_impl<Holder>;

template<typename ...Defs> constexpr std::tuple<member_symbol<Defs>...> record_members(record<Defs...> const &) { return {}; }
template<typename ...Defs> constexpr std::tuple<Defs...> record_member_defs(record<Defs...> const &) { return {}; }
template<typename ...Defs> constexpr auto record_member_var_defs_impl(record<Defs...> const &);
template<typename Rec> using record_member_var_defs = decltype(here::record_member_var_defs_impl(std::declval<Rec>()));

template<typename ...Defs> void valid_if_record(record<Defs...> const &);
template<typename ...Defs> std::true_type is_record(record<Defs...> const &);
template<typename T> std::false_type is_record(T const &);

template<typename Def, typename Holder> struct slice;
template<typename Def, typename Value> struct record_initializer { Value value; };
template<typename Def, typename Value> constexpr auto make_record_initializer(Value && value);

template<typename Symbol, typename Access, typename Cons> struct member_id;
template<typename Rec, typename Self, typename F> struct memfun;
template<typename Access> struct member_access_fun;
template<typename F> auto make_symbol(F f);
template<std::size_t ...Is, typename F> auto make_symbol_impl(std::index_sequence<Is...>, F f);

template<auto ...> struct symbol {};
template<auto x> struct memptr { static constexpr decltype(x) value = x; };
struct raw_value_tag {};

// definitions

// filter
constexpr auto filter = [] (auto && p, auto && t) {
    return std::apply(here::fix([&] (auto && filter_impl, auto && ...results) {
        return here::ftie([&] {
            return std::tuple{std::forward<decltype(results)>(results)...};
        }, [&] (auto && x, auto && ...xs) {
            if constexpr (decltype(p(std::forward<decltype(x)>(x))){}) {
                return filter_impl(std::forward<decltype(results)>(results)..., std::forward<decltype(x)>(x))(std::forward<decltype(xs)>(xs)...);
            } else {
                return filter_impl(std::forward<decltype(results)>(results)...)(std::forward<decltype(xs)>(xs)...);
            }
        });
    })(), std::forward<decltype(t)>(t));
};

// merge

constexpr auto merge = here::ftie([] (auto && ...rs) -> std::enable_if_t<!(decltype(here::is_record(rs)){} && ...)> {
    return skip_all{};
}, [] (auto && ...rs) {
    return here::fix([] (auto && merge_impl, auto && ...inits) {
        return here::ftie([&] {
            return record { std::forward<decltype(inits)>(inits)... };
        }, [&] (auto && rec, auto && ...recs) {
            auto filtered = std::apply([] (auto ...defs) {
                return here::filter([] (auto def) {
                    constexpr bool b = (std::is_same_v<member_symbol<decltype(def)>, member_symbol<decltype(defs)>> || ...);
                    return std::bool_constant<!b>{};
                }, decltype(here::record_member_defs(rec)){});
            }, std::tuple_cat(decltype(here::record_member_defs(recs)){}...));
            return std::apply([&] (auto ...defs) {
                return merge_impl(
                    std::forward<decltype(inits)>(inits)...,
                    here::make_record_initializer<decltype(defs)>(
                        member_access<decltype(defs)>{}(std::forward<decltype(rec)>(rec), raw_value_tag{})
                    )...)(std::forward<decltype(recs)>(recs)...);
            }, decltype(filtered){});
        });
    })()(std::forward<decltype(rs)>(rs)...);
});

// swap
namespace swap_detail {
using std::swap;
constexpr auto swap_impl = here::ftie([] (auto & a, auto & b, auto && ...defs) {
    if constexpr (std::tuple_size_v<record_member_var_defs<std::decay_t<decltype(a)>>> != std::tuple_size_v<record_member_var_defs<std::decay_t<decltype(b)>>>) {
        return skip_all{};
    } else if constexpr (auto pred = [&] () -> decltype(((void)swap(a[defs], b[defs]), ...)) {}; !decltype(valid_expr(pred)){}) {
        return skip_all{};
    } else {
        ((void)swap(a[defs], b[defs]), ...);
    }
});
}
template<typename Rec1, typename Rec2>
constexpr auto swap(Rec1 & a, Rec2 & b) -> decltype(
    std::apply(swap_detail::swap_impl, std::tuple_cat(std::forward_as_tuple(a, b), record_member_var_defs<Rec1>{}))
) {
    std::apply(swap_detail::swap_impl, std::tuple_cat(std::forward_as_tuple(a, b), record_member_var_defs<Rec1>{}));
}

// record
template<typename ...Defs>
struct record: record_slice<Defs, record<Defs...>>... {
    using record_slice<Defs, record>::operator[]...;

    template<typename ...Inits>
    constexpr record(Inits && ...inits): record_slice<Defs, record>{std::forward<decltype(std::move(inits).value)>(inits.value)}... {}

    template<typename ...Assigns>
    constexpr record & operator=(record<Assigns...> const & assigns) {
        ((void)(operator[](Assigns{}) = assigns[Assigns{}]), ...);
        return *this;
    }
    template<typename ...Assigns>
    constexpr record & operator=(record<Assigns...> && assigns) {
        ((void)(operator[](Assigns{}) = std::move(assigns)[Assigns{}]), ...);
        return *this;
    }

    template<typename Rec2>
    constexpr auto swap(Rec2 & rec2) -> decltype(here::swap(*this, rec2)) {
        here::swap(*this, rec2);
    }
};
template<typename ...Defs, typename ...Values> record(record_initializer<Defs, Values>...) -> record<Defs...>;

// ==
template<typename ...Defs1, typename ...Defs2,
    typename = std::enable_if_t<sizeof...(Defs1) == sizeof...(Defs2)>
>
constexpr auto operator==(record<Defs1...> const & rec1, record<Defs2...> const & rec2) -> decltype(
    ((rec1[Defs1{}] == rec2[Defs1{}]) && ...)
) {
    return ((rec1[Defs1{}] == rec2[Defs1{}]) && ...);
}

// !=
template<typename ...Defs1, typename ...Defs2>
constexpr auto operator!=(record<Defs1...> const & rec1, record<Defs2...> const & rec2) -> decltype(static_cast<bool>(rec1 == rec2)) {
    return !(rec1 == rec2);
}

// slice
template<typename Def, typename Holder>
struct slice: Holder {
    template<typename V, bool C = !Def::is_memfun, typename = std::enable_if_t<C>>
    constexpr slice(V && v): Holder{std::forward<V>(v)} {}
    template<typename V, bool C = Def::is_memfun && std::is_standard_layout_v<member_raw_value_type<Def>>, typename = std::enable_if_t<C>, typename = void>
    constexpr slice(V && v): Holder{{std::forward<V>(v)}} {}
    template<typename V, bool C = Def::is_memfun && !std::is_standard_layout_v<member_raw_value_type<Def>>, typename = std::enable_if_t<C>, typename = void, typename = void>
    slice(V && v): Holder{{std::forward<V>(v), this}} {}
    constexpr auto        & operator[](member_symbol<Def>)        & { return member_access<Def>{}(*this); }
    constexpr auto const  & operator[](member_symbol<Def>) const  & { return member_access<Def>{}(*this); }
    constexpr auto       && operator[](member_symbol<Def>)       && { return member_access<Def>{}(std::move(*this)); }
    constexpr auto const && operator[](member_symbol<Def>) const && { return member_access<Def>{}(std::move(*this)); }
};

// memfun
template<typename Rec, typename Self, typename F>
struct memfun {
    template<typename ...Args>
    constexpr memfun(Args && ...args): data{std::forward<Args>(args)...} {}
    template<typename ...Args>
    decltype(auto) operator()(Args && ...args)        & {
        return std::get<0>(data)(self(), std::forward<Args>(args)...);
    }
    template<typename ...Args>
    decltype(auto) operator()(Args && ...args) const  & {
        return std::get<0>(data)(self(), std::forward<Args>(args)...);
    }
    template<typename ...Args>
    decltype(auto) operator()(Args && ...args)       && {
        return std::get<0>(data)(std::move(self()), std::forward<Args>(args)...);
    }
    template<typename ...Args>
    decltype(auto) operator()(Args && ...args) const && {
        return std::get<0>(data)(std::move(self()), std::forward<Args>(args)...);
    }
    template<typename Access> friend struct member_access_fun;
private:
    static constexpr bool stdlayout = std::is_standard_layout_v<F>;
    std::conditional_t<stdlayout, std::tuple<F>, std::tuple<F, Self>> data;
    Rec       & self()       { return *static_cast<Rec       *>(_self()); }
    Rec const & self() const { return *static_cast<Rec const *>(_self()); }
    Self * _self() {
        if constexpr (stdlayout) return reinterpret_cast<Self *>(this);
        else return std::get<1>(data);
    }
    Self const * _self() const {
        if constexpr (stdlayout) return reinterpret_cast<Self const *>(this);
        else return std::get<1>(data);
    }
};

// member_definition
template<typename Symbol, typename Access, typename Cons, typename Value, bool Fun>
struct member_definition: Symbol {
    using symbol = Symbol;
    using raw_value_type = Value;
    using member_access = Access;
    static constexpr bool is_memfun = Fun;
    template<typename Rec>
    struct make_value_type {
        template<typename Holder>
        using make_value_type_impl = std::conditional_t<
            Fun,
            memfun<Rec, Holder, Value>,
            Value
        >;
    };
    template<typename Rec>
    using record_slice = slice<member_definition, typename decltype(std::declval<Cons>()(make_value_type<Rec>{}))::type>;
};

// member_id
template<typename Symbol, typename Access, typename Cons>
struct member_id: Symbol {
    template<typename ...Args> constexpr member_id(Args && ...) {}
    template<typename Value, bool Fun = false>
    constexpr auto operator=(Value && x) && {
        using def = member_definition<Symbol, Access, Cons, std::decay_t<Value>, Fun>;
        return here::make_record_initializer<def>(std::forward<Value>(x));
    }
    constexpr member_id() = default;
private:
    struct fun_helper {
        template<typename Value>
        constexpr auto operator=(Value && v) && {
            return member_id{}.operator=<Value, true>(std::forward<Value>(v));
        }
    private:
        constexpr fun_helper() = default;
        constexpr fun_helper(fun_helper const &) = default;
        constexpr fun_helper(fun_helper &&) = default;
        constexpr fun_helper & operator=(fun_helper const &) = default;
        constexpr fun_helper & operator=(fun_helper &&) = default;
        friend struct member_id;
    };
public:
    fun_helper fun;
};
template<typename Symbol, typename Access, typename Cons>
member_id(Symbol sym, Access, Cons) -> member_id<decltype(here::make_symbol(sym)), member_access_fun<Access>, Cons>;

// member_access_fun
template<typename ...Args> std::true_type is_memfun_value(memfun<Args...> const &);
template<typename T> std::false_type is_memfun_value(T const &);
template<typename Access>
struct member_access_fun {
    template<typename Rec>
    constexpr auto && operator()(Rec && r) const {
        return std::forward<Rec>(r).*decltype(std::declval<Access>()(r))::value;
    }
    template<typename Rec>
    constexpr auto && operator()(Rec && r, raw_value_tag) const {
        auto && v = operator()(std::forward<Rec>(r));
        if constexpr (decltype(here::is_memfun_value(v)){}) {
            return std::get<0>(std::forward<decltype(v)>(v).data);
        } else {
            return std::forward<decltype(v)>(v);
        }
    }
};

// make_symbol
template<typename F>
auto make_symbol(F f) {
    return here::make_symbol_impl(std::make_index_sequence<sizeof(f())>{}, f);
}
template<std::size_t ...Is, typename F>
auto make_symbol_impl(std::index_sequence<Is...>, F f) {
    return symbol<f()[Is]...>{};
}

// make_record_initializer
template<typename Def, typename Value>
constexpr auto make_record_initializer(Value && value) {
    return record_initializer<Def, Value &&>{std::forward<Value>(value)};
}

// record_member_var_defs_impl
template<typename ...Defs>
constexpr auto record_member_var_defs_impl(record<Defs...> const & r) {
    return here::filter([] (auto def) {
        return std::bool_constant<!decltype(def)::is_memfun>{};
    }, here::record_member_defs(r));
}

} // namespace detail::record {

namespace record {
using detail::record::record, detail::record::merge, detail::record::swap;
using detail::record::operator==, detail::record::operator!=;
}

} // namespace desalt {
