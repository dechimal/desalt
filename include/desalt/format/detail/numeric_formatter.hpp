#if !defined DESALT_FORMAT_DETAIL_NUMERIC_FORMATTER_HPP_INCLUDED_
#define      DESALT_FORMAT_DETAIL_NUMERIC_FORMATTER_HPP_INCLUDED_

#include <type_traits>
#include <desalt/require.hpp>
#include <desalt/format/constexpr/algorithm.hpp>
#include <desalt/format/detail/make_charset_pred.hpp>
#include <desalt/format/detail/find_index_specifier.hpp>
#include <desalt/format/detail/toi.hpp>
#include <iterator>

namespace desalt { namespace format {

namespace detail {

template<typename I, typename T>
constexpr bool contains(I f, I l, T x) {
    return here::find(f, l, x) != l;
}
template<typename I, typename P>
constexpr bool any(I f, I l, P p) {
    return here::find_if(f, l, p) != l;
}

template<char const * String, std::size_t I, std::size_t E, typename T>
struct integer_formatter {
    static constexpr std::size_t flags_end = here::find_if(String+I, String+E, here::make_charset_pred("#0- +"))-String;
    static constexpr std::size_t width_digits_end = here::find_if(String+flags_end, String+E, here::make_charset_pred("0123456789"))-String;
    static constexpr std::size_t width_var_end = here::find_if(String+flags_end, String+E, here::make_charset_pred("*"))-String;
    static constexpr std::size_t width_end =
        width_digits_end != flags_end ? width_digits_end
                                      : width_var_end != flags_end ? width_var_end
                                                                   : flags_end;
    static constexpr std::size_t precision_sep_end = here::find_if(String+width_end, String+E, here::make_charset_pred("."))-String;
    static constexpr std::size_t precision_digits_end = here::find_if(String+precision_sep_end, String+E, here::make_charset_pred("0123456789"))-String;
    static constexpr std::size_t precision_var_end = here::find_if(String+precision_sep_end, String+E, here::make_charset_pred("*"))-String;
    static constexpr std::size_t precision_end =
        precision_digits_end != precision_sep_end ? precision_digits_end
                                                  : precision_var_end != precision_sep_end ? precision_var_end
                                                                                           : precision_sep_end;
    static constexpr std::size_t type_end = here::find_if(String+precision_end, String+E, here::make_charset_pred("diouxX"))-String;
    static_assert(type_end != precision_end, "the numeric format placeholder is not valid.");

    static constexpr char type = String[type_end-1];
    static constexpr bool alternative = here::contains(String+I, String+E, '#');
    static_assert(!alternative || type != 'd' && type != 'i' && type != 'u', "the numeric format placeholder is not valid.");
    static constexpr bool has_precision = precision_sep_end != width_end;
    static constexpr bool has_variable_precision = precision_var_end != precision_sep_end;
    static constexpr bool has_width = String+flags_end != String+width_end;
    static constexpr unsigned width = here::toi<unsigned>(String+flags_end, String+width_end);
    static constexpr unsigned precision = !has_precision || has_variable_precision ? 0 : here::toi<unsigned>(String+precision_sep_end, String+precision_end);
    static constexpr bool left_align = here::contains(String+I, String+flags_end, '-');
    static constexpr bool zerofill = here::contains(String+I, String+flags_end, '0') && !(left_align || has_precision);
    static constexpr bool sign = here::contains(String+I, String+flags_end, '+') && type != 'u';
    static constexpr bool space = here::contains(String+I, String+flags_end, ' ') && !sign && type != 'u';
    static constexpr unsigned base = type == 'o' ? 8 : type == 'd' || type == 'i' || type == 'u' ? 10 : 16;
    static constexpr unsigned prefix_size = alternative && type == 'x' || type == 'X' ? 2 : 1;
    static constexpr std::size_t end_pos = type_end;
    static constexpr std::size_t used_indexes_count = has_width + has_precision;
    using this_type = integer_formatter;
    template<typename CharT, typename ...Args>
    static void format(std::basic_ostream<CharT> & ost, T const & val, Args const & ...) {
        constexpr unsigned upper_bound_size = 1 + prefix_size + here::max(precision, width, integer_formatter::upper_bound_size(sizeof(val)*8, base));
        std::array<CharT, upper_bound_size + 1> buf;
        buf.back() = '\0';
        auto f = buf.rbegin() + 1;
        auto precision_pos = f + precision;
        auto i = f;
        this_type::format_impl<type>(val, i, precision_pos);

        constexpr char fill_char = zerofill ? '0' : ' ';
        auto width_pos = f + width;
        while (i < width_pos) *i++ = fill_char;

        ost << buf.data() + (buf.rend() - i);
    }
    template<char Type, typename It, DESALT_REQUIRE_C(Type == 'd' || Type == 'i')>
    static void format_impl(T const & val, It & i, It precision_pos) {
        auto abs = val < 0 ? -val : val;
        while (abs) {
            *i++ = abs % 10 + '0';
            abs /= 10;
        }
        while (i < precision_pos) *i++ = '0';
        if (val < 0) *i++ = '-';
        else if (sign) *i++ = '+';
        else if (space) *i++ = ' ';
    }
    template<char Type, typename It, DESALT_REQUIRE_C(Type == 'u')>
    static void format_impl(T const & val, It & i, It precision_pos) {
        auto uval = static_cast<typename std::make_unsigned<T>::type>(val);
        while (uval) {
            *i++ = uval % 10 + '0';
            uval /= 10;
        }
        while (i < precision_pos) *i++ = '0';
    }
    template<char Type, typename It, DESALT_REQUIRE_C(Type == 'o')>
    static void format_impl(T const & val, It & i, It precision_pos) {
        auto uval = static_cast<typename std::make_unsigned<T>::type>(val);
        while (uval) {
            *i++ = (uval & 7) + '0';
            uval >>= 3;
        }
        while (i < precision_pos) *i++ = '0';
        if (alternative && *std::prev(i) != '0') *i++ = '0';
    }
    template<char Type, typename It, DESALT_REQUIRE_C(Type == 'x' || Type == 'X')>
    static void format_impl(T const & val, It & i, It precision_pos) {
        auto uval = static_cast<typename std::make_unsigned<T>::type>(val);
        constexpr char const * table = type == 'x' ? "0123456789abcdef" : "0123456789ABCDEF";
        while (uval) {
            *i++ = table[uval & 0xf];
            uval >>= 4;
        }
        while (i < precision_pos) *i++ = '0';
        if (alternative && val != 0) {
            *i++ = type;
            *i++ = '0';
        }
    }

    // n進k桁でn^k種類の数を表現できる
    // 2^bits種類の数を表わすには何桁あれば十分か
    // (n, bits は非負整数)
    // n^k>2^bits
    // log_n(n^k)>log_n(2^bits)
    // k>log_n(2^bits)
    // k>bits*log_n(2)
    // k>bits/log_2(n)
    // <==
    // k>bits/log_2_int(n)
    // (log_2_int(n)をlog_2(n)の整数部分とする．
    //  log_2(n) >= log_2_int(n)
    //  であるので
    //  bits/log_2(n) <= bits/log_2_int(n)
    //  がいえる)
    static constexpr auto upper_bound_size(unsigned bits, unsigned base) {
        return bits / integer_formatter::log_2_int(base);
    }
    static constexpr auto log_2_int(unsigned n) {
        struct t {
            static constexpr unsigned impl(unsigned n, unsigned ret) {
                return n != 0 ? t::impl(n>>1, ret+1) : ret;
            }
        };
        return !n ? 0 : t::impl(n, 0)-1;
    }
};

}

namespace traits {

template<char const * String, std::size_t I, std::size_t E, typename T>
struct argument_formatter<String, I, E, T, typename std::enable_if<std::is_integral<T>::value>::type> : detail::integer_formatter<String, I, E, T>
{
    using base = detail::integer_formatter<String, I, E, T>;
    using base::end_pos;
    using base::used_indexes_count;
    using base::format;
};

}


}}

#endif
