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

#define ASSERT_EQ(x, y, ...) ASSERT_BIN((x), (y), #x, #y, == __VA_OPT__(,) __VA_ARGS__)
#define ASSERT_NE(x, y, ...) ASSERT_BIN((x), (y), #x, #y, != __VA_OPT__(,) __VA_ARGS__)

#define ASSERT(x, ...) \
    ASSERT_IMPL( \
        (x), \
        #x, \
        ("  value: {}\n", qcttest::fmt_wrap{x}), \
        __VA_OPT__(std::println(__VA_ARGS__)) \
    ) \
    //
#define ASSERT_BIN(x, y, xstr, ystr, op, ...) \
    ASSERT_IMPL( \
        (x op y), \
        xstr " " #op " " ystr, \
        ("  {}: {}\n  {}: {}", xstr, qcttest::fmt_wrap{x}, ystr, qcttest::fmt_wrap{y}), \
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
        std::println("FAILED: " __FILE__ ":" QCT_LINE_STR ": case \"{}\"\n  expr: {}", qcttest::current_name, estr); \
        std::println diag_msg; \
        user_msg; \
        qcttest::current_failure = true; \
        return; \
    } while (0) \
    //

namespace qcttest {

struct test_link {
    void (*test)();
    char const * name = nullptr;
    test_link * next = nullptr;
};
static test_link * first, * last;
static bool current_failure = false;
static char const * current_name = nullptr;

template<typename T>
struct fmt_wrap {
    fmt_wrap(T const & t): ptr(&t) {}
    T const * ptr;
};

}

namespace std {

template<typename CharT, std::formattable<CharT> T>
    requires (!std::ranges::input_range<T>)
struct formatter<qcttest::fmt_wrap<T>, CharT>: std::formatter<T, CharT> {
    auto format(qcttest::fmt_wrap<T> const & x, auto & fc) const {
        return std::formatter<T, CharT>::format(*x.ptr, fc);
    }
};

template<typename T>
struct formatter<qcttest::fmt_wrap<T *>> {
    constexpr auto parse(auto & pc) {
        return pc.begin();
    }
    auto format(qcttest::fmt_wrap<T *> x, auto & fc) const {
        char buf[21];
        auto [ptr, ec] = std::to_chars(buf, buf + sizeof(buf), reinterpret_cast<std::uintptr_t>(x.ptr), 16);
        auto out = std::copy_n("*0x", 3, fc.out());
        return std::copy(buf, ptr, out);
    }
};

template<typename T, typename CharT>
    requires requires (std::basic_ostream<CharT> & ost, T t) { ost << t; }
        && (!std::formattable<T, CharT>)
        && (!(std::ranges::input_range<T> && std::formattable<std::ranges::range_value_t<T>, CharT>))
struct formatter<qcttest::fmt_wrap<T>, CharT> {
    constexpr auto parse(auto & pc) {
        return pc.begin();
    }
    auto format(qcttest::fmt_wrap<T> x, auto & fc) const {
        std::basic_stringstream<CharT> ss;
        ss << *x.ptr;
        auto s = std::move(ss.str());
        return std::copy(s.begin(), s.end(), fc.out());
    }
};

#if __GNUC__ <= 14

template<std::ranges::input_range R, typename CharT>
    requires std::formattable<std::ranges::range_value_t<R>, CharT>
struct formatter<qcttest::fmt_wrap<R>, CharT> {
    constexpr auto parse(auto & pc) {
        return pc.begin();
    }
    auto format(qcttest::fmt_wrap<R> const & r, auto & fc) const {
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

}

#define TEST(test_case_name, ...) TEST_I(QCT_CAT(qcttest_, __COUNTER__), test_case_name __VA_OPT__(,) __VA_ARGS__)
#define TEST_I(class_name, test_case_name, ...) \
    struct class_name __VA_OPT__(:) __VA_ARGS__ { \
        void _qct_test_case(); \
        static void _qct_run() { \
            qcttest::current_failure = false; \
            qcttest::current_name = test_case_name; \
            try { \
                class_name()._qct_test_case(); \
            } catch (...) { \
                qcttest::current_failure = true; \
            } \
        } \
        __attribute__((constructor)) static void _qct_init() { \
            static qcttest::test_link link{_qct_run, test_case_name}; \
            if (!qcttest::first) qcttest::first = qcttest::last = &link; \
            else qcttest::last = qcttest::last->next = &link; \
        } \
    }; \
    void class_name::_qct_test_case() \
    //

int main(int argc, char * argv[]) {
    std::vector<std::regex> pats;
    if (argc < 2) {
        pats.emplace_back(".*");
    } else {
        for (int i = 1; i < argc; ++i) pats.emplace_back(argv[i]);
    }

    bool r = false;
    for (qcttest::test_link * link = qcttest::first; link; link = link->next) {
        for (auto & pat: pats) {
            if (std::regex_match(link->name, pat)) {
                link->test();
                break;
            }
        }
        r |= qcttest::current_failure;
    }
    return r;
}

#endif
