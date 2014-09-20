#include <desalt/format/format.hpp>
#include <array>
#include <cassert>
#include <cstdio>
#include <string>
#include <iosfwd>

#include <attotest/attotest.cpp>

struct a {};
template<typename CharT>
std::basic_ostream<CharT> & operator<<(std::basic_ostream<CharT> & ost, a) {
    ost << "It works!";
    return ost;
}

template<typename ...Args>
auto printf(char const * format, Args ...args) {
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
    static_assert(*(String + I + 0) == 'h', "");
    static_assert(*(String + I + 1) == 'o', "");
    static_assert(*(String + I + 2) == 'g', "");
    static_assert(*(String + I + 3) == 'e', "");
    static constexpr std::size_t end_pos = I + 4;
    static constexpr std::size_t used_indexes_count = 0;
    template<typename ...Args>
    static void format(std::ostream & ost, my_int i, Args const & ...) {
        ost << i.a * 2;
    }
};

}}}

#define TEST(format, ...) ATTOTEST_ASSERT_EQUAL(::printf(format, __VA_ARGS__), DESALT_FORMAT_STRING(format, __VA_ARGS__))

ATTOTEST_CASE(format_test) {
    namespace f = desalt::format::detail;
    std::array<int, 5> const hoge{1, 2, 3, 4, 5};
    ATTOTEST_ASSERT_EQUAL(f::find(hoge.begin(), hoge.end(), 5), std::prev(hoge.end()));

    TEST("%d", 1);
    TEST("%d %d", 1, 42);
    TEST("%d %% %d", 1, 42);
    TEST("%3d", 1);
    TEST("%+7.3d", 42);
    TEST("%+7.3d", -42);
    TEST("% 7.3d", 42);
    TEST("%07.3d", 42);
    TEST("%o", 42);
    TEST("%7o", 42);
    TEST("%#.2o", 42);
    TEST("%#.7o", 42);
    TEST("%#7o", 42);
    TEST("%#07o", 42);
    TEST("%#2o", 42);
    TEST("%x", 42);
    TEST("%X", 42);
    TEST("%#7.3x", 42);
    TEST("%#7.3x", 0);
    TEST("%7.3u", 42);
    TEST("%1$7.3u", 42, 24);
    TEST("%2$7.3u", 42, 24);
    ATTOTEST_ASSERT_EQUAL(DESALT_FORMAT_STRING("%d %d", a{}, 42), "It works!d 42");
    ATTOTEST_ASSERT_EQUAL(DESALT_FORMAT_STRING("%hoge", my_int{42}), "84");
}
