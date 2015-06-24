#if !defined ATTOTEST_INCLUDED_
#define      ATTOTEST_INCLUDED_

#include <string>
#include <type_traits>
#include <functional>
#include <sstream>
#include <boost/fusion/include/is_sequence.hpp>
#include <boost/fusion/include/fold.hpp>
#include <boost/preprocessor/cat.hpp>

#define ATTOTEST_CASE(name) ATTOTEST_CASE_I(name, BOOST_PP_CAT(attotest_test, name))
#define ATTOTEST_CASE_I(name, type) \
    struct type { \
        static int init() { \
            attotest::add_test(test, ATTOTEST_TO_STR(name)); \
            return 0; \
        } \
        static int x; \
        static void test(); \
    }; \
    int type::x = type::init(); \
    void type::test()

#define ATTOTEST_CASE_PARAM(name, ...) ATTOTEST_CASE_PARAM_I(name, BOOST_PP_CAT(attotest_testcase, name), BOOST_PP_CAT(attotest_testfun, name), ATTOTEST_GET_COUNTER(__LINE__)::value, name(__VA_ARGS__))
#define ATTOTEST_CASE_PARAM_I(name, type, testfun, counter, call) \
    template<std::size_t, std::size_t> struct type; \
    template<> struct type<__LINE__, counter> { \
        static int init() { \
            attotest::add_test(testfun, ATTOTEST_TO_STR(call)); \
            return 0; \
        } \
        static int x; \
        static void testfun(); \
    }; \
    int type<__LINE__, counter>::x = type::init(); \
    void type<__LINE__, counter>::testfun() { \
        call; \
    } \
    ATTOTEST_INC_COUNTER(__LINE__)

#define ATTOTEST_ASSERT_BASE(expr, msg) \
    attotest::assert_(expr, __FILE__, __LINE__, [&] { return msg; })

#define ATTOTEST_ASSERT_BINARY_OP(op, x, y) \
    ATTOTEST_ASSERT_BASE(x op y, ATTOTEST_TO_STR(x op y) " | " + ATTOTEST_SHOW(x) + ", " + ATTOTEST_SHOW(y))

#define ATTOTEST_ASSERT_BINARY_FUN(f, x, y) \
    ATTOTEST_ASSERT_BASE(f(x, y), ATTOTEST_TO_STR(f(x, y)) " | " + ATTOTEST_SHOW(x) + ", " + ATTOTEST_SHOW(y))

#define ATTOTEST_ASSERT(x) ATTOTEST_ASSERT_BASE(bool(x), "expected true | " + ATTOTEST_SHOW(x))
#define ATTOTEST_ASSERT_NOT(x) ATTOTEST_ASSERT_BASE(!bool(x), "expected false | " + ATTOTEST_SHOW(x))
#define ATTOTEST_ASSERT_EQUAL(x, y) ATTOTEST_ASSERT_BINARY_OP(==, x, y)
#define ATTOTEST_ASSERT_NOT_EQUAL(x, y) ATTOTEST_ASSERT_BINARY_OP(!=, x, y)
#define ATTOTEST_ASSERT_FAIL() ATTOTEST_ASSERT_BASE(false, "reached to failure code path")

#define ATTOTEST_SHOW(expr) (ATTOTEST_TO_STR(expr) ": " + attotest::to_string_(expr))

#define ATTOTEST_TO_STR(...) ATTOTEST_TO_STR_I(__VA_ARGS__)
#define ATTOTEST_TO_STR_I(...) # __VA_ARGS__

#define ATTOTEST_REQUIRE(...) \
    typename = typename std::enable_if<(__VA_ARGS__)>::type

namespace attotest {
namespace here = ::attotest;

using boost::fusion::traits::is_sequence;

void add_test(void(*)(), char const *);
void assert_(bool cond, std::string const & file, std::size_t line, std::function<std::string()> msg);

template<std::size_t N>
using size_t = std::integral_constant<std::size_t, N>;

#define ATTOTEST_GET_COUNTER(id) decltype(attotest_counter(attotest::size_t<id>{}, attotest::max_count{}))
#define ATTOTEST_INC_COUNTER(id) attotest::succ<ATTOTEST_GET_COUNTER(id)> attotest_counter(attotest::size_t<id>, attotest::succ<ATTOTEST_GET_COUNTER(id)>)

template<std::size_t N>
struct count : count<N-1> {
    static constexpr std::size_t value = N;
};
template<>
struct count<0> {
    static constexpr std::size_t value = 0;
};

using max_count = count<256>;

template<typename N>
using succ = count<N::value+1>;

namespace traits {
template<typename T, typename Enable = void> struct to_string : std::false_type {};
}


template<typename T> struct sfinae_test : decltype(T::test(0)) {};

template<typename T>
struct has_begin_end_test {
    template<typename U = T>
    static auto test(int, U * p = nullptr) -> decltype(std::begin(*p), (void)0, std::end(*p), (void)0, std::true_type{});
    static auto test(...) -> std::false_type;
};
template<typename T> using has_begin_end = sfinae_test<has_begin_end_test<T>>;

template<typename T>
struct has_adl_begin_end_test {
    template<typename U = T>
    static auto test(int, U * p = nullptr) -> decltype(begin(*p), (void)0, end(*p), (void)0, std::true_type{});
    static auto test(...) -> std::false_type;
};

template<typename T> using has_adl_begin_end = sfinae_test<has_adl_begin_end_test<T>>;

template<typename T>
struct has_to_string_test {
    template<typename U = T>
    static auto test(int, U * p = nullptr) -> decltype(p->to_string(), (void)0, std::true_type{});
    static auto test(...) -> std::false_type;
};
template<typename T> using has_to_string = sfinae_test<has_to_string_test<T>>;
template<typename T>
struct has_adl_to_string_test {
    template<typename U = T>
    static auto test(int, U * p = nullptr) -> decltype(to_string(*p), (void)0, std::true_type{});
    static auto test(...) -> std::false_type;
};
template<typename T> using has_adl_to_string = sfinae_test<has_adl_to_string_test<T>>;

template<typename T>
struct is_output_streamable_test {
    template<typename U = T>
    static auto test(int, std::ostream * o = nullptr, U const * p = nullptr) -> decltype(*o << *p, (void)0, std::true_type{});
    static auto test(...) -> std::false_type;
};
template<typename T> using is_output_streamable = sfinae_test<is_output_streamable_test<T>>;

std::string to_string_(int);
std::string to_string_(unsigned int);
std::string to_string_(long);
std::string to_string_(unsigned long);
std::string to_string_(long long);
std::string to_string_(unsigned long long);
std::string to_string_(double);
std::string to_string_(bool);
std::string to_string_(std::string const &);
std::string to_string_(std::string &&);
std::string to_string_(char const *);
template<typename ...Elems>
std::string to_string_(std::tuple<Elems...> const &);
template<typename T, typename U>
std::string to_string_(std::pair<T, U> const &);
template<typename T>
std::string to_string_(T const &);

template<typename T, ATTOTEST_REQUIRE(traits::to_string<T>::value)>
std::string to_string_traits(T const & x);
template<typename T, ATTOTEST_REQUIRE(!traits::to_string<T>::value), typename = void>
std::string to_string_traits(T const & x);

template<typename T, ATTOTEST_REQUIRE(is_output_streamable<T>::value)>
std::string to_string_ostream(T const &);
template<typename T, ATTOTEST_REQUIRE(!is_output_streamable<T>::value), typename = void>
std::string to_string_ostream(T const &);

template<typename T, ATTOTEST_REQUIRE(has_to_string<T>::value)>
std::string to_string_memfun(T const &);
template<typename T, ATTOTEST_REQUIRE(!has_to_string<T>::value), typename = void>
std::string to_string_memfun(T const &);

template<typename T, ATTOTEST_REQUIRE(has_adl_to_string<T>::value)>
std::string to_string_adl(T const &);
template<typename T, ATTOTEST_REQUIRE(!has_adl_to_string<T>::value), typename = void>
std::string to_string_adl(T const &);

template<typename Range, ATTOTEST_REQUIRE((has_begin_end<Range>::value || has_adl_begin_end<Range>::value) && !is_sequence<Range>::value)>
std::string to_string_range(Range const &);
template<typename T, ATTOTEST_REQUIRE(!(has_begin_end<T>::value || has_adl_begin_end<T>::value) || is_sequence<T>::value), typename = void>
std::string to_string_range(T const &);

template<typename Seq, ATTOTEST_REQUIRE(is_sequence<Seq>::value)>
std::string to_string_fuseq(Seq const & x);
template<typename T, ATTOTEST_REQUIRE(!is_sequence<T>::value), typename = void>
std::string to_string_fuseq(T const & x);

std::string to_string_fallback();

template<typename Tuple, typename Index, typename Size, ATTOTEST_REQUIRE(!Index::value && Index::value < Size::value)>
std::string to_string_tuple(std::string &&, Tuple const &, Index, Size);
template<typename Tuple, typename Index, typename Size, ATTOTEST_REQUIRE(Index::value && Index::value < Size::value), typename = void>
std::string to_string_tuple(std::string &&, Tuple const &, Index, Size);
template<typename Tuple, typename Index, typename Size, ATTOTEST_REQUIRE(!(Index::value < Size::value)), typename = void, typename = void>
std::string to_string_tuple(std::string &&, Tuple const &, Index, Size);

template<typename ...Elems>
std::string to_string_(std::tuple<Elems...> const & tup) {
    return here::to_string_tuple("(", tup, here::size_t<0>{}, std::tuple_size<std::tuple<Elems...>>{});
}
template<typename T, typename U>
std::string to_string_(std::pair<T, U> const & p) {
    return here::to_string_tuple("(", p, here::size_t<0>{}, std::tuple_size<std::pair<T, U>>{});
}
template<typename T>
std::string to_string_(T const & x) {
    return here::to_string_traits(x);
}

template<typename T, typename>
std::string to_string_traits(T const & x) {
    return traits::to_string<T>()(x);
}
template<typename T, typename, typename>
std::string to_string_traits(T const & x) {
    return here::to_string_ostream(x);
}

template<typename T, typename>
std::string to_string_ostream(T const & x) {
    std::ostringstream oss;
    oss << x;
    return oss.str();
}
template<typename T, typename, typename>
std::string to_string_ostream(T const & x) {
    return here::to_string_memfun(x);
}

template<typename T, typename>
std::string to_string_memfun(T const & x) {
    return x.to_string();
}
template<typename T, typename, typename>
std::string to_string_memfun(T const & x) {
    return here::to_string_range(x);
}

template<typename Range, typename>
std::string to_string_range(Range const & range) {
    std::string str("[");
    bool trailing = false;
    for (auto e : range) {
        if (trailing) str += ", ";
        trailing = true;
        str += here::to_string_(e);
    }
    return str + "]";
}
template<typename T, typename, typename>
std::string to_string_range(T const & x) {
    return here::to_string_fuseq(x);
}

struct to_string_fuseq_t {
    typedef std::string result_type;
    template<typename T>
    std::string operator()(std::true_type, T const & x) const {
        return "(" + here::to_string_(x);
    }
    template<typename T>
    std::string operator()(std::string const & str, T const & x) const {
        return str + ", " + here::to_string_(x);
    }
};
template<typename Seq, typename>
std::string to_string_fuseq(Seq const & x) {
    return boost::fusion::fold(x, std::true_type{}, to_string_fuseq_t()) + ")";
}
template<typename T, typename, typename>
std::string to_string_fuseq(T const & x) {
    return here::to_string_adl(x);
}

template<typename T, typename>
std::string to_string_adl(T const & x) {
    return to_string(x);
}
template<typename T, typename, typename>
std::string to_string_adl(T const &) {
    return to_string_fallback();
}

template<typename Tuple, typename Index, typename Size, typename>
std::string to_string_tuple(std::string && str, Tuple const & tup, Index, Size size) {
    return here::to_string_tuple(str + here::to_string_(std::get<Index::value>(tup)),
                                 tup,
                                 here::size_t<Index::value+1>{},
                                 size);
}
template<typename Tuple, typename Index, typename Size, typename, typename>
std::string to_string_tuple(std::string && str, Tuple const & tup, Index, Size size) {
    return here::to_string_tuple(str + ", " + here::to_string_(std::get<Index::value>(tup)),
                                 tup,
                                 here::size_t<Index::value+1>{},
                                 size);
}
template<typename Tuple, typename Index, typename Size, typename, typename, typename>
std::string to_string_tuple(std::string && str, Tuple const &, Index, Size) {
    return str + ")";
}

}

template<std::size_t N>
attotest::count<0> attotest_counter(attotest::size_t<N>, attotest::count<0> const &);

#endif
