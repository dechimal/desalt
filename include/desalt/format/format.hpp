#if !defined DESALT_FORMAT_FORMAT_HPP_INCLUDED_
#define      DESALT_FORMAT_FORMAT_HPP_INCLUDED_

#include <iostream>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <utility>
#include <boost/preprocessor/config/limits.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/facilities/intercept.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/variadic/size.hpp>
#include <desalt/require.hpp>
#include <desalt/parameter_pack.hpp>
#include <desalt/format/constexpr/algorithm.hpp>
#include <desalt/format/constexpr/array.hpp>
#include <desalt/format/detail/toi.hpp>
#include <desalt/format/detail/find_placeholder.hpp>
#include <desalt/static_control/static_if.hpp>

namespace desalt { namespace format { namespace traits {

template<char const *, std::size_t I, std::size_t, typename T, typename = void>
struct argument_formatter {
    static constexpr std::size_t end_pos = I;
    static constexpr std::size_t used_indexes_count = 0;
    template<typename OStream, typename U, typename ...Args>
    static void format(OStream & ost, U const & val, Args const & ...) {
        ost << val;
    }
};

}}}

#include <desalt/format/detail/numeric_formatter.hpp>

#define DESALT_FORMAT_STRING(...) DESALT_FORMAT(string, __VA_ARGS__)
#define DESALT_FORMAT_FORMAT(...) DESALT_FORMAT(format, __VA_ARGS__)
#define DESALT_FORMAT_PRINT(...) DESALT_FORMAT(print, __VA_ARGS__)

#define DESALT_FORMAT(f, ...) DESALT_FORMAT_I(f, __VA_ARGS__ BOOST_PP_COMMA_IF(BOOST_PP_EQUAL(BOOST_PP_VARIADIC_SIZE(__VA_ARGS__), 1)))
#define DESALT_FORMAT_I(...) DESALT_FORMAT_II(__VA_ARGS__)
#define DESALT_FORMAT_II(f, format_string, ...) \
    ([&] { struct type_encoded_format_string { BOOST_PP_REPEAT(BOOST_PP_LIMIT_REPEAT, DESALT_FORMAT_M, format_string) }; \
          return desalt::format::detail::make_formatter<type_encoded_format_string>::f(__VA_ARGS__); }())
#define DESALT_FORMAT_M(z, i, format_string) \
    using BOOST_PP_CAT(t, i) = desalt::format::detail::char_type<(i < sizeof(format_string) / sizeof(*format_string) ? format_string[i] : '\0')>;

namespace desalt { namespace format {

namespace detail {
namespace here = detail;
namespace pp = desalt::parameter_pack;
using pp::type_seq;
using pp::at_c;
using static_control::static_if;

// forward declarations
template<typename T> struct make_formatter;
template<char const * String, std::size_t I, std::size_t E, typename Seq, unsigned PlaceholderIndex, unsigned = here::find_placeholder(String+I, String+E)-String> auto parse_format_string();
template<typename, typename> struct composite_formatter;
template<typename F1, typename F2> composite_formatter<F1, F2> compose(F1, F2);
template<char const *, std::size_t, std::size_t> struct text;
template<char const *, std::size_t, std::size_t, typename, std::size_t> struct placeholder;
template<typename CharT> constexpr std::size_t strlen(CharT const * str, std::size_t i = 0);
template<typename> struct ostream_adaptor;
template<typename F> ostream_adaptor<F> make_ostream_adaptor(F);
template<char C> using char_type = std::integral_constant<char, C>;

// make_formatter
template<typename T>
struct make_formatter {
    static constexpr char buf[] = { BOOST_PP_ENUM_BINARY_PARAMS(BOOST_PP_LIMIT_REPEAT, T::t, ::value BOOST_PP_INTERCEPT) };
    template<typename ...Args>
    using formatter = decltype(here::parse_format_string<buf, 0, here::strlen(buf), type_seq<Args...>, 0>());
    template<typename ...Args>
    static std::string string(Args const & ...args) {
        std::stringstream ss;
        formatter<Args...>().write(ss, args...);
        return ss.str();
    }
    template<typename OStream, typename ...Args>
    static auto format(Args const & ...args) {
        return here::make_ostream_adaptor([&] (OStream & ost) {
                formatter<Args...>().write(ost, args...);
            });
    }
    template<typename ...Args>
    static void print(Args const & ...args) {
        formatter<Args...>().write(std::cout, args...);
    }
};
template<typename T>
constexpr char make_formatter<T>::buf[];

// parse_format_string
template<char const * String, std::size_t I, std::size_t E, typename Seq, unsigned PlaceholderIndex,
    unsigned placeholder_head>
auto parse_format_string() {
    return here::static_if([&] (auto dep, std::enable_if_t<decltype(dep){}(placeholder_head) == E>* = nullptr) {
        return text<String, I, dep(placeholder_head)>{};
    }, [&] (auto dep) {
        constexpr auto head = decltype(dep){}(placeholder_head);
        constexpr auto index_specifier_pos = here::find_index_specifier(String+head+1, String+E)-String;
        constexpr auto has_index = index_specifier_pos != head+1;
        constexpr auto index = has_index ? here::toi<unsigned>(String+head+1, String+index_specifier_pos-1)-1 : PlaceholderIndex;
        using placeholder = here::placeholder<String, index_specifier_pos, E, typename at_c<Seq, index>::type, index>;
        constexpr auto next_placeholder_index = PlaceholderIndex+!has_index+placeholder::used_indexes_count;
        return here::compose(text<String, I, head>{},
                             here::compose(placeholder{},
                                           here::parse_format_string<String, placeholder::end_pos, E, Seq, next_placeholder_index>()));
    });
}

// ostream_adaptor
template<typename F> struct ostream_adaptor { F f; };
template<typename F> ostream_adaptor<F> make_ostream_adaptor(F f) { return ostream_adaptor<F>(f); }
template<typename OStream, typename F>
OStream & operator<<(OStream & ost, ostream_adaptor<F> adaptor) {
    adaptor.f(ost);
    return ost;
}

// unescape
template<char const * String, std::size_t I, std::size_t E, typename CharT, char ...Chars,
         DESALT_REQUIRE_C(I == E)>
auto unescape(type_seq<std::integral_constant<char, Chars>...>) {
    return make_array<CharT>(static_cast<CharT>(Chars)...);
}
template<char const * String, std::size_t I, std::size_t E, typename CharT, char ...Chars,
         DESALT_REQUIRE_C(I != E)>
auto unescape(type_seq<std::integral_constant<char, Chars>...>) {
    return here::unescape<String, String[I] == '%' ? I+2 : I+1, E, CharT>(type_seq<std::integral_constant<char, Chars>..., std::integral_constant<char, String[I]>>{});
}

// text
template<char const * String, std::size_t I, std::size_t E>
struct text {
    template<typename OStream, typename ...Args>
    void write(OStream & ost, Args const & ...) const {
        auto s = here::unescape<String, I, E, typename OStream::char_type>(type_seq<>{});
        write_impl(ost, s);
    }
    template<typename OStream, std::size_t N>
    static void write_impl(OStream & ost, array<typename OStream::char_type, N> const & s) {
        ost.write(s.data(), N);
    }
    template<typename OStream>
    static void write_impl(OStream &, array<typename OStream::char_type, 0> const &) {}
};

// placeholder
template<char const * String, std::size_t I, std::size_t E, typename T, std::size_t PlaceholderIndex>
struct placeholder {
    using argument_formatter_type = traits::argument_formatter<String, I, E, T>;
    static constexpr std::size_t end_pos = argument_formatter_type::end_pos;
    static constexpr std::size_t used_indexes_count = argument_formatter_type::used_indexes_count;
    template<typename OStream, typename ...Args>
    void write(OStream & ost, Args const & ...args) const {
        argument_formatter_type::format(ost, std::get<PlaceholderIndex>(std::tie(args...)), args...);
    }
};

// composite_formatter
template<typename F1, typename F2>
struct composite_formatter {
    template<typename OStream, typename ...Args>
    void write(OStream & ost, Args const & ...args) const {
        f1.write(ost, args...);
        f2.write(ost, args...);
    }
    F1 f1;
    F2 f2;
};
template<typename F1, typename F2>
composite_formatter<F1, F2> compose(F1, F2) {
    return composite_formatter<F1, F2>();
}

template<typename CharT>
constexpr std::size_t strlen(CharT const * str, std::size_t i) {
    return *(str + i) == static_cast<CharT>('\0') ? i : here::strlen(str, i+1);
}

}}}

#endif
