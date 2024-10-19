#include <desalt/format/format.hpp>
#include <desalt/quectotest.hpp>

#include <array>
#include <cstdio>
#include <string>
#include <iosfwd>
#include <charconv>

struct a {};

template<typename CharT>
std::basic_ostream<CharT> & operator<<(std::basic_ostream<CharT> & ost, a) {
    ost << "It works!";
    return ost;
}

template<typename ...Args>
auto ref_printf(char const * format, Args ...args) {
    char buf[128];
    std::snprintf(buf, 128, format, args...);
    return std::string(buf);
}

struct my_int {
    int a;
};

namespace desalt { namespace format { namespace traits {

template<char const * String, std::size_t I, std::size_t E>
struct argument_formatter<String, I, E, my_int> {
    static_assert(E - I >= 4, "");
    static_assert(String[I+0] == 'h', "");
    static_assert(String[I+1] == 'o', "");
    static_assert(String[I+2] == 'g', "");
    static_assert(String[I+3] == 'e', "");
    static constexpr std::size_t end_pos = I + 4;
    static constexpr std::size_t used_indexes_count = 0;
    template<typename ...Args>
    static void format(std::ostream & ost, my_int i, Args const & ...) {
        ost << i.a * 2;
    }
};

}}}

template<>
struct std::formatter<my_int> {
    constexpr auto parse(auto & pc) { return pc.begin(); }
    auto format(my_int x, auto & fc) const {
        auto n = x.a * 2;
        char buf[11];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), n);
        return std::copy(buf, ptr, fc.out());
    }
};

#define FMT_ASSERT(format, ...) ASSERT_EQ(::ref_printf(format, __VA_ARGS__), DESALT_FORMAT_STRING(format, __VA_ARGS__))


TEST("format test") {
    namespace f = desalt::format::detail;
    std::array<int, 5> const hoge{{1, 2, 3, 4, 5}};
    ASSERT_EQ(f::find(hoge.begin(), hoge.end(), 5), std::prev(hoge.end()));

    FMT_ASSERT("%d", 1);
    FMT_ASSERT("%d %d", 1, 42);
    FMT_ASSERT("%d %% %d", 1, 42);
    FMT_ASSERT("%3d", 1);
    FMT_ASSERT("%+7.3d", 42);
    FMT_ASSERT("%+7.3d", -42);
    FMT_ASSERT("% 7.3d", 42);
    FMT_ASSERT("%07.3d", 42);
    FMT_ASSERT("%o", 42);
    FMT_ASSERT("%7o", 42);
    FMT_ASSERT("%#.2o", 42);
    FMT_ASSERT("%#.7o", 42);
    FMT_ASSERT("%#7o", 42);
    FMT_ASSERT("%#07o", 42);
    FMT_ASSERT("%#2o", 42);
    FMT_ASSERT("%x", 42);
    FMT_ASSERT("%X", 42);
    FMT_ASSERT("%#7.3x", 42);
    FMT_ASSERT("%#7.3x", 0);
    FMT_ASSERT("%7.3u", 42);
    FMT_ASSERT("%1$7.3u", 42, 24);
    FMT_ASSERT("%2$7.3u", 42, 24);
    ASSERT_EQ(DESALT_FORMAT_STRING("%d %d", a{}, 42), "It works!d 42");
    ASSERT_EQ(DESALT_FORMAT_STRING("%hoge", my_int{42}), "84");
}
