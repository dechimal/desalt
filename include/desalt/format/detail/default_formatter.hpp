#if !defined DESALT_FORMAT_DETAIL_DEFAULT_FORMATTER_HPP_INCLUDED_
#define      DESALT_FORMAT_DETAIL_DEFAULT_FORMATTER_HPP_INCLUDED_

namespace desalt { namespace format {

namespace detail {

template<std::size_t I>
struct default_formatter {
    static constexpr std::size_t end_pos = I;
    static constexpr std::size_t used_indexes_count = 0;
    template<typename T, typename ...Args>
    static void format(std::ostream & ost, T const & val, Args const & ...) {
        ost << val;
    }
};

}

namespace traits {

template<char const *, std::size_t I, std::size_t>
detail::default_formatter<I> argument_formatter(...);

}


}}

#endif
