#ifndef QUECTOTEST_HPP
#define QUECTOTEST_HPP

#include <algorithm>
#include <array>
#include <charconv>
#include <print>
#include <regex>
#include <string>
#include <vector>

#if __GNUC__ <= 14
#include <ranges>
#endif

#define TEST(test_case_name, ...) TEST_I(QCT_CAT(qcttest_, __COUNTER__), test_case_name __VA_OPT__(,) __VA_ARGS__)
#define TEST_I(class_name, test_case_name, ...) \
    struct class_name __VA_OPT__(:) __VA_ARGS__ { \
        void _qctst_test_case(); \
        static inline constexpr char _qctst_test_case_name[] = test_case_name; \
        __attribute__((constructor)) static void _qctst_init() { \
            ::desalt::qcttest::_qctst_init<class_name>(); \
        } \
    }; \
    void class_name::_qctst_test_case() \
    //

#define ASSERT_EQ(x, y, ...) ASSERT_BIN((x), (y), #x, #y, == __VA_OPT__(,) __VA_ARGS__)
#define ASSERT_NE(x, y, ...) ASSERT_BIN((x), (y), #x, #y, != __VA_OPT__(,) __VA_ARGS__)

#define ASSERT(x, ...) \
    ASSERT_IMPL( \
        (x), \
        #x, \
        ("  value: {}\n", ::desalt::qcttest::fmt_wrap{x}), \
        __VA_OPT__(std::println(__VA_ARGS__)) \
    ) \
    //
#define ASSERT_BIN(x, y, xstr, ystr, op, ...) \
    ASSERT_IMPL( \
        (x op y), \
        xstr " " #op " " ystr, \
        ("  {}: {}\n  {}: {}", xstr, ::desalt::qcttest::fmt_wrap{x}, ystr, ::desalt::qcttest::fmt_wrap{y}), \
        __VA_OPT__(std::println(__VA_ARGS__)) \
    ) \
    //

#define QCT_CAT(x, y) QCT_CAT_I(x, y)
#define QCT_CAT_I(x, y) x ## y
#define QCT_STR(x) QCT_STR_I(x)
#define QCT_STR_I(x) #x
#define QCT_LINE_STR QCT_STR(__LINE__)
#define ASSERT_IMPL(e, estr, diag_msg, user_msg) \
    do if (!(e)) { \
        std::println("FAILED: " __FILE__ ":" QCT_LINE_STR ": case \"{}\"\n  expr: {}", ::desalt::qcttest::current_name, estr); \
        std::println diag_msg; \
        user_msg; \
        ::desalt::qcttest::current_failure = true; \
        return; \
    } while (0) \
    //

namespace desalt::qcttest {
namespace here = qcttest;

struct test_link {
    void (*test)();
    char const * name;
    test_link * next;
};
static constinit test_link * first, * last;
static constinit bool current_failure;
static constinit char const * current_name;

template<typename Derived>
static void _qctst_init() {
    static constinit test_link link = test_link {
        [] { Derived()._qctst_test_case(); },
        Derived::_qctst_test_case_name,
        nullptr,
    };
    if (!here::first) here::first = here::last = &link;
    else here::last = here::last->next = &link;
}

template<typename T>
struct fmt_wrap {
    fmt_wrap(T const & t): ptr(&t) {}
    T const * ptr;
};

template<typename, typename> struct is_string_family: std::false_type {};
template<typename T, typename CharT> struct is_string_family<T &, CharT>: is_string_family<T, CharT> {};
template<typename CharT, typename Traits, typename A>
struct is_string_family<std::basic_string<CharT, Traits, A>, CharT>: std::true_type {};
template<typename CharT, typename Traits>
struct is_string_family<std::basic_string_view<CharT, Traits>, CharT>: std::true_type {};
template<typename CharT> struct is_string_family<CharT *, CharT>: std::true_type {};
template<typename CharT> struct is_string_family<CharT const *, CharT>: std::true_type {};
template<typename CharT, std::size_t N> struct is_string_family<CharT [N], CharT>: std::true_type {};
template<typename CharT, std::size_t N> struct is_string_family<CharT const [N], CharT>: std::true_type {};

template<typename T, typename CharT>
concept string_family = is_string_family<T, CharT>::value;

template<typename T, typename CharT> struct formatter;

template<typename CharT, std::formattable<CharT> T>
    requires (!std::ranges::input_range<T>) || string_family<T, CharT>
struct formatter<T, CharT>: std::formatter<T, CharT> {
    template<typename FC>
    typename FC::iterator format(fmt_wrap<T> const & x, FC & fc) const {
        return std::formatter<T, CharT>::format(*x.ptr, fc);
    }
};

template<typename T, typename CharT>
struct formatter<T *, CharT>: std::formatter<void const *, CharT> {
    template<typename FC>
    typename FC::iterator format(fmt_wrap<T *> const & x, FC & fc) const {
        return std::formatter<void const *, CharT>::format(*x.ptr, fc);
    }
};

template<typename T, typename CharT>
    requires
        requires (std::basic_ostream<CharT> & ost, T t) { ost << t; }
        && (!std::formattable<T, CharT>)
        && (!std::ranges::input_range<T> || !std::formattable<std::ranges::range_value_t<T>, CharT>)
struct formatter<T, CharT> {
    constexpr auto parse(auto & pc) {
        return pc.begin();
    }
    template<typename FC>
    typename FC::iterator format(fmt_wrap<T> x, FC & fc) const {
        std::basic_stringstream<CharT> ss;
        ss << *x.ptr;
        auto s = std::move(ss.str());
        return std::copy(s.begin(), s.end(), fc.out());
    }
};

#if __GNUC__ <= 14

template<std::ranges::input_range R, typename CharT>
    requires std::formattable<std::ranges::range_value_t<R>, CharT>
        && (!string_family<R, CharT>)
struct formatter<R, CharT> {
    constexpr auto parse(auto & pc) {
        return pc.begin();
    }
    auto format(fmt_wrap<R> const & r, auto & fc) const {
        auto out = fc.out();
        *out++ = '[';
        bool trailing = false;
        for (auto & x: *r.ptr) {
            if (trailing) std::format_to(out, ", {}", x);
            else std::format_to(out, "{}", x);
            trailing = true;
        }
        *out++ = ']';
        return out;
    }
};

#endif

} // namespace desalt::qcttest

namespace std {

template<typename T, typename CharT>
struct formatter<desalt::qcttest::fmt_wrap<T>, CharT>
    : desalt::qcttest::formatter<T, CharT>
{};

} // namespace std

int main(int argc, char * argv[]) {
    namespace qct = desalt::qcttest;
    std::vector<std::regex> pats;
    if (argc < 2) {
        pats.emplace_back(".*");
    } else {
        for (int i = 1; i < argc; ++i) pats.emplace_back(argv[i]);
    }

    bool r = false;
    for (qct::test_link * link = qct::first; link; link = link->next) {
        for (auto & pat: pats) {
            if (std::regex_match(link->name, pat)) goto found;
        }
        continue;
    found:
        qct::current_failure = false;
        qct::current_name = link->name;
        try {
            link->test();
        } catch (...) {
            std::println("FAILED: exception throw from test case \"{}\"", qct::current_name);
            qct::current_failure = true;
        }
        r |= qct::current_failure;
    }
    return r;
}

#endif
