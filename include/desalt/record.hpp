#include <utility>
#include <type_traits>
#include <tuple>

#define DESALT_MEMBER(name) DESALT_MEMBER_I(name, DESALT_PP_CAT(slice, name))
#define DESALT_MEMBER_I(name, slice) DESALT_MEMBER_II(name, slice)
#define DESALT_MEMBER_II(name, slice) \
    (::desalt::detail::record::member_id { \
        [] () -> auto const & { return DESALT_PP_STR(name); }, \
        [] (auto v, auto sym) { \
            struct slice { \
                typename decltype(v)::type name; \
                constexpr static auto memptr(decltype(sym)) { return &slice::name; }; \
            }; \
            return ::desalt::detail::record::idtype<slice>{}; \
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

template<auto ...> struct symbol {};
struct raw_value_tag {};
template<typename T> struct idtype { using type = T; };

template<typename ...Defs> struct record;

template<typename Symbol, typename Cons, typename Value, bool Fun> struct member_definition;
template<typename Def> using member_symbol = typename Def::symbol_type;
template<typename Def> using member_raw_value_type = typename Def::raw_value_type;
template<typename Def, typename Rec> using member_slice = typename Def::template slice<Rec>;
template<typename Def, typename Rec> static constexpr std::ptrdiff_t member_offset = Def::template member_offset<Rec>;

template<typename ...Defs> constexpr std::tuple<member_symbol<Defs>...> record_members(record<Defs...> const &) { return {}; }
template<typename ...Defs> constexpr std::tuple<Defs...> record_member_defs_impl(idtype<record<Defs...>>);
template<typename Rec> using record_member_defs = decltype(here::record_member_defs_impl(idtype<std::decay_t<Rec>>{}));
template<typename Rec> constexpr auto record_member_var_defs_impl(idtype<Rec>);
template<typename Rec> using record_member_var_defs = decltype(here::record_member_var_defs_impl(idtype<std::decay_t<Rec>>{}));
template<typename Def1, typename ...Defs, typename Def2> constexpr auto record_member_def_impl(idtype<std::tuple<Def1, Defs...>>, Def2);
template<typename Rec, typename Def> using record_member_def = decltype(here::record_member_def_impl(idtype<record_member_defs<Rec>>{}, Def{}));

template<typename ...Defs1, typename Rec2> constexpr bool record_swappable_impl(idtype<std::tuple<Defs1...>>, idtype<Rec2>);
template<typename Rec1, typename Rec2> static constexpr bool record_swappable =
    here::record_swappable_impl(idtype<record_member_defs<std::decay_t<Rec1>>>{}, idtype<std::decay_t<Rec2>>{});
template<typename ...Defs1, typename ...Defs2,
    bool C = here::record_swappable<record<Defs1...>, record<Defs2...>>,
    typename = std::enable_if_t<C>
> constexpr void swap(record<Defs1...> &, record<Defs2...> &);
template<typename Rec1, typename Rec2, typename ...Defs> constexpr void swap_impl(Rec1 & a, Rec2 & b, idtype<std::tuple<Defs...>>);

template<typename ...Recs> constexpr auto combine(Recs && ...rs);
template<typename Rec> struct combine_helper;

template<typename Def, typename Value> struct record_initializer { Value value; };
template<typename Def, typename Value> constexpr auto make_record_initializer(Value && value);

template<typename Symbol, typename Cons> struct member_id;
template<typename Def, typename Rec, typename F> struct memfun;
template<typename Class, typename Base, typename T> constexpr std::ptrdiff_t offset_of(T Base::*);
template<typename F> auto make_symbol(F f);
template<std::size_t ...Is, typename F> auto make_symbol_impl(std::index_sequence<Is...>, F f);

template<typename Pred, typename T> struct filter_helper;
template<typename Pred, typename ...Ts> constexpr auto filter_impl(Pred &&, idtype<std::tuple<Ts...>>);
template<typename Pred, typename Tup> using filter = typename decltype(here::filter_impl(std::declval<Pred>(), idtype<Tup>{}))::type;

// definitions

// record
template<typename ...Defs>
struct record: member_slice<Defs, record<Defs...>>... {
    using member_slice<Defs, record<Defs...>>::memptr...;

    template<typename ...Inits>
    constexpr record(Inits && ...inits): member_slice<Defs, record>{std::forward<decltype(inits.value)>(inits.value)}... {}

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

    template<typename Def, typename = std::enable_if_t<!std::is_same_v<record_member_def<record, Def>, void>>>
    constexpr auto        & operator[](Def)            & { return this->*record::memptr(member_symbol<Def>{}); }
    template<typename Def, typename = std::enable_if_t<!std::is_same_v<record_member_def<record, Def>, void>>>
    constexpr auto const  & operator[](Def def) const  & { return const_cast<record &>(*this)[def]; }
    template<typename Def, typename = std::enable_if_t<!std::is_same_v<record_member_def<record, Def>, void>>>
    constexpr auto       && operator[](Def def)       && { return std::move((*this)[def]); }
    template<typename Def, typename = std::enable_if_t<!std::is_same_v<record_member_def<record, Def>, void>>>
    constexpr auto const && operator[](Def def) const && { return std::move((*this)[def]); }

    template<typename ...Defs2, typename = std::enable_if_t<here::record_swappable<record, record<Defs2...>>()>>
    constexpr void swap(record<Defs2...> & rec2) {
        here::swap_impl(*this, rec2, idtype<record_member_var_defs<record>>{});
    }
};
template<typename ...Defs, typename ...Values> record(record_initializer<Defs, Values>...) -> record<Defs...>;
template<typename Def, typename Rec, typename = std::enable_if_t<!std::is_same_v<record_member_def<Rec, Def>, void>>>
constexpr auto && get(Def def, Rec && rec) {
    auto mp = std::decay_t<Rec>::memptr(def);
    return std::forward<Rec>(rec).*mp;
}

// swap
template<typename ...Defs1, typename ...Defs2, bool, typename>
constexpr void swap(record<Defs1...> & a, record<Defs2...> & b) {
    here::swap_impl(a, b, idtype<record_member_var_defs<record<Defs1...>>>{});
}
template<typename Rec1, typename Rec2, typename ...Defs>
constexpr void swap_impl(Rec1 & a, Rec2 & b, idtype<std::tuple<Defs...>>) {
    using std::swap;
    ((void)swap(a[Defs{}], b[Defs{}]), ...);
}
template<typename ...Defs, typename Rec2>
constexpr bool record_swappable_impl(idtype<std::tuple<Defs...>>, idtype<Rec2>) {
    constexpr std::size_t nmem_var_rec2 = std::tuple_size_v<record_member_var_defs<Rec2>>;
    if (nmem_var_rec2 != sizeof...(Defs)) return false;
    return (std::is_swappable_with_v<
        member_raw_value_type<Defs> &,
        member_raw_value_type<record_member_def<Rec2, Defs>> &
    > && ...);
}

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

// combine
template<typename ...Recs>
constexpr auto combine(Recs && ...rs) {
    return std::apply([] (auto && ...inits) {
        auto make_init = [] (auto def, auto && rec) {
            using def_type = decltype(def);
            if constexpr (decltype(def)::is_memfun) {
                return here::make_record_initializer<def_type>(
                    std::forward<decltype(rec)>(rec)[def].f
                );
            } else {
                return here::make_record_initializer<def_type>(
                    std::forward<decltype(rec)>(rec)[def]
                );
            }
        };
        return record {
            std::apply(make_init, std::forward<decltype(inits)>(inits))...
        };
    }, (combine_helper<Recs &&>{std::forward<Recs>(rs)} + ... + std::tuple{}));
}
template<typename Rec>
struct combine_helper {
    template<typename ...Defs, typename ...Recs>
    constexpr auto operator+(std::tuple<std::tuple<Defs, Recs>...> && result) const {
        auto pred = [] (auto def) {
            using def_type = typename decltype(def)::type;
            return std::bool_constant<!(std::is_same_v<member_symbol<def_type>, member_symbol<Defs>> || ...)>{};
        };
        using defs = filter<decltype(pred), record_member_defs<decltype(rec)>>;
        auto inits = std::apply([&rec=rec] (auto ...defs) {
            return std::make_tuple(std::tuple<decltype(defs), Rec>(defs, std::forward<Rec>(rec))...);
        }, defs{});
        return std::tuple_cat(std::move(inits), std::move(result));
    }
    Rec rec;
};

// memfun
template<typename Def, typename Rec, typename F>
struct memfun {
    template<typename G>
    constexpr memfun(G && f): f{std::forward<G>(f)} {}
    template<typename ...Args>
    decltype(auto) operator()(Args && ...args)       {
        return f(self(), std::forward<Args>(args)...);
    }
    template<typename ...Args>
    decltype(auto) operator()(Args && ...args) const {
        return f(self(), std::forward<Args>(args)...);
    }
    template<typename ...Recs> friend constexpr auto combine(Recs && ...);
private:
    F f;
    Rec & self() {
        return *reinterpret_cast<Rec *>(reinterpret_cast<char *>(this) - member_offset<Def, Rec>);
    }
    Rec const & self() const { return const_cast<memfun *>(this)->self(); }
};

// offset_of
template<typename Class, typename Base, typename T>
constexpr std::ptrdiff_t offset_of(T Base::* mp) {
    union aligner {
        char addr_range[sizeof(Class)];
        Class obj;
        constexpr aligner(): addr_range{} {};
    };
    aligner a;
    void * target = static_cast<void *>(&(a.obj.*mp));
    char * addr = a.addr_range;
    while (addr != target) ++addr;
    return addr - a.addr_range;
}

// member_definition
template<typename Symbol, typename Cons, typename Value, bool Fun>
struct member_definition: Symbol {
    using symbol_type = Symbol;
    using raw_value_type = Value;
    static constexpr bool is_memfun = Fun;
    template<typename Rec>
    using slice = typename decltype(
        std::declval<Cons>()(std::conditional<Fun, memfun<member_definition, Rec, Value>, Value>{}, Symbol{})
    )::type;
    template<typename Rec>
    static constexpr std::ptrdiff_t member_offset = here::offset_of<Rec>(Rec::memptr(Symbol{}));
};

// member_id
template<typename Symbol, typename Cons>
struct member_id: Symbol {
    using symbol_type = Symbol;
    template<typename ...Args> constexpr member_id(Args && ...) {}
    template<typename Value, bool Fun = false>
    constexpr auto operator=(Value && x) && {
        using def = member_definition<Symbol, Cons, std::decay_t<Value>, Fun>;
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
template<typename Symbol, typename Cons>
member_id(Symbol sym, Cons) -> member_id<decltype(here::make_symbol(sym)), Cons>;

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
template<typename Def> using record_member_var_defs_helper = std::bool_constant<Def::is_memfun>;
template<typename Rec>
constexpr auto record_member_var_defs_impl(idtype<Rec>) {
    auto pred = [] (auto def) {
        return std::bool_constant<!decltype(def)::type::is_memfun>{};
    };
    return filter<decltype(pred), record_member_defs<Rec>>{};
}

// record_member_def_impl
template<typename Def1, typename ...Defs, typename Def2>
constexpr auto record_member_def_impl(idtype<std::tuple<Def1, Defs...>>, Def2) {
    if constexpr (std::is_same_v<member_symbol<Def1>, member_symbol<Def2>>) {
        return Def1{};
    } else if constexpr (sizeof...(Defs) > 0) {
        return here::record_member_def_impl(idtype<std::tuple<Defs...>>{}, Def2{});
    } else {
        return;
    }
}

// filter
template<typename Pred, typename ...Ts>
constexpr auto filter_impl(Pred && p, idtype<std::tuple<Ts...>>) {
    return (filter_helper<Pred, Ts>{} + ... + idtype<std::tuple<>>{});
}
template<typename Pred, typename T>
struct filter_helper {
    template<typename ...Ts>
    constexpr auto operator+(idtype<std::tuple<Ts...>>) const {
        if constexpr (decltype(std::declval<Pred>()(idtype<T>{}))::value) {
            return idtype<std::tuple<T, Ts...>>{};
        } else {
            return idtype<std::tuple<Ts...>>{};
        }
    }
};

} // namespace detail::record {

namespace record {
using detail::record::record, detail::record::combine, detail::record::swap;
using detail::record::operator==, detail::record::operator!=;
}

} // namespace desalt {
