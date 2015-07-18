#if !defined DESALT_FORMAT_DETAIL_SEQUENCE_CONTAINER_FORMATTER_HPP_INCLUDED_
#define      DESALT_FORMAT_DETAIL_SEQUENCE_CONTAINER_FORMATTER_HPP_INCLUDED_

#include <type_traits>
#include "find_index_specifier.hpp"
#incldue "toi.hpp"

namespace desalt {
namespace detail {

template<typename T>
struct is_sequence_container_impl {
    template<typename U,
        DESALT_REQUIRE_EXPR(std::declval<U>().cbegin()),
        DESALT_REQUIRE_EXPR(std::declval<U>().cend())>
    std::true_type test(int);
    std::false_type test(...);
    using type = decltype(test<U>(0));
};

template<typename T> struct is_sequence_container
    : is_sequence_container_impl<T>::type
{};

template<char const * String, std::size_t I, std::size_t E, typename T, char FormatterType>
struct container_argument_formatter_impl {
    static constexpr std::size_t index_var_end = 
    static constexpr std::size_t index_specifier_end = here::find_index_specifier(String + I, String + E);
    static constexpr std::size_t index_var_pos = here::find_if(String+index_specifier_end, String+E, here::make_charset_pred("*"))-String;
    static_assert(index_specifier_pos > I, "");
    static constexpr std::size_t index = toi<std::size_t>(String+I, String+index_specifier_pos-1);
    using element_formatter_type = decltype(parse_format_string<String, index_specifier_pos, E, typename T::value_type, type_seq<typename T::value_type>, 
};
template<char const * String, std::size_t I, std::size_t E, typename T>
struct container_argument_formatter_impl<String, I, E, T, '['> {};

// enclosure-open-char separator-char enclosure-close-char element-format-specifier
}

namespace traits {

template<char const * String, std::size_t I, std::size_t E, typename T>
struct argument_formatter<String, I, E, T, std::enable_if<is_container<T>::value>> {
    // 特定の要素を表示する
    // 全要素を表示する
    //  sequence container
    //  associative container
    static_assert(I < E, "");
    static constexpr std::size_t type_pos = I;
    static constexpr std::size_t type_end = I;
    static constexpr char type = *(String + type_pos);
    using impl = detail::container_argument_formatter_impl<String, I+1, E, T, type>;
    static constexpr std::size_t impl::end_pos = I;
    static constexpr std::size_t impl::used_indexes_count = 0;
    template<typename OStream, typename U, typename ...Args>
    static void format(OStream & ost, U const & val, Args const & ...) {
        ost << val;
    }
};

}
}

#endif
