#if !defined DESALT_FORMAT_FORMAT_HPP_INCLUDED_
#define      DESALT_FORMAT_FORMAT_HPP_INCLUDED_

#include <iostream>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <utility>
#include <boost/preprocessor/config/limits.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/variadic/size.hpp>
#include <desalt/require.hpp>
#include <desalt/parameter_pack.hpp>
#include <desalt/format/constexpr/algorithm.hpp>
#include <desalt/format/constexpr/array.hpp>
#include <desalt/format/detail/toi.hpp>
#include <desalt/format/detail/find_placeholder.hpp>

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
#define DESALT_FORMAT_II(f, str, ...) \
    (desalt::format::detail::make_formatter<desalt::format::detail::to_str<std::integer_sequence<char, BOOST_PP_ENUM(BOOST_PP_LIMIT_REPEAT, DESALT_FORMAT_M, str)>>>::f(__VA_ARGS__))
#define DESALT_FORMAT_M(z, i, format) \
    (i < sizeof(format) ? format[i] : '\0')

namespace desalt { namespace format {

namespace detail {
namespace here = detail;
namespace pp = desalt::parameter_pack;
using pp::type_seq;
using pp::at_c;

// forward declarations
template<typename, typename> struct composite_formatter;
template<char const *, std::size_t, std::size_t> struct text;
template<char const *, std::size_t, std::size_t, typename, std::size_t> struct placeholder;
template<typename> struct ostream_adaptor;

template<typename F1, typename F2> composite_formatter<F1, F2> compose(F1, F2);
template<typename F> ostream_adaptor<F> make_ostream_adaptor(F);

// parse_format_string
template<char const * String, std::size_t I, std::size_t E, typename Seq, unsigned PlaceholderIndex,
         DESALT_REQUIRE_C(here::find_placeholder(String+I, String+E) == String+E)>
auto parse_format_string() {
    return text<String, I, E>{};
}
template<char const * String, std::size_t I, std::size_t E, typename Seq, unsigned PlaceholderIndex,
         std::size_t PlaceholderPos = here::find_placeholder(String+I, String+E)-String,
         DESALT_REQUIRE_C(PlaceholderPos != E)>
auto parse_format_string() {
    using index_specifier_pos = std::integral_constant<std::size_t, here::find_index_specifier(String+PlaceholderPos+1, String+E)-String>;
    using has_index = std::integral_constant<bool, index_specifier_pos::value != PlaceholderPos+1>;
    using index = std::integral_constant<unsigned, has_index::value ? here::toi<unsigned>(String+PlaceholderPos+1, String+index_specifier_pos::value-1)-1 : PlaceholderIndex>;
    using placeholder = here::placeholder<String, index_specifier_pos::value, E, typename at_c<Seq, index::value>::type, index::value>;
    return here::compose(text<String, I, PlaceholderPos>{},
                         here::compose(placeholder{},
                                       here::parse_format_string<String, placeholder::end_pos, E, Seq, PlaceholderIndex+!has_index::value+placeholder::used_indexes_count>()));
}

// make_formatter
template<typename T>
struct make_formatter {
    template<typename ...Args>
    using formatter = decltype(here::parse_format_string<T::str, 0, here::find(T::str, T::str + sizeof(T::str), '\0') - T::str, type_seq<Args...>, 0>());
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

template<typename T> struct to_str;
template<typename CharT, CharT ...Chars>
struct to_str<std::integer_sequence<CharT, Chars...>> {
    static constexpr CharT const str[] = { Chars ... };
};

}}}

#endif
