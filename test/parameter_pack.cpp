
#include <type_traits>
#include <boost/preprocessor/stringize.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/print.hpp>
#include <boost/fusion/include/vector.hpp>
#include <desalt/parameter_pack.hpp>
#include <iostream>
#include <typeinfo>
#include <cxxabi.h>

namespace ppack = desalt::parameter_pack;
namespace mpl = boost::mpl;
namespace fu = boost::fusion;
using ppack::type_seq;

using a = type_seq<int, char, double>;
using m = mpl::vector<int, char, double>;
using v = fu::vector<int, char, double>;

#define STATIC_TEST(...) static_assert((__VA_ARGS__), __FILE__ "(" BOOST_PP_STRINGIZE(__LINE__) ")")

STATIC_TEST(std::is_same<ppack::head<a>::type, int>::value);
STATIC_TEST(std::is_same<ppack::tail<a>::type, type_seq<char, double>>::value);
STATIC_TEST(std::is_same<ppack::cons<float, a>::type, type_seq<float, int, char, double>>::value);
STATIC_TEST(ppack::size<a>::value == 3);
STATIC_TEST(std::is_same<ppack::reverse<a>::type, type_seq<double, char, int>>::value);
STATIC_TEST(std::is_same<ppack::append<a, type_seq<float, short>>::type, type_seq<int, char, double, float, short>>::value);
STATIC_TEST(std::is_same<ppack::from_mpl_seq<m>::type, a>::value);
STATIC_TEST(std::is_same<ppack::from_fusion_seq<v>::type, a>::value);
STATIC_TEST(std::is_same<ppack::enumerate_c<int, 0, 5>::type, type_seq<std::integral_constant<int, 0>,
                                                                       std::integral_constant<int, 1>,
                                                                       std::integral_constant<int, 2>,
                                                                       std::integral_constant<int, 3>,
                                                                       std::integral_constant<int, 4>>>::value);
STATIC_TEST(std::is_same<ppack::at_c<a, 0>::type, int>::value);
STATIC_TEST(std::is_same<ppack::at_c<a, 1>::type, char>::value);
STATIC_TEST(std::is_same<ppack::at_c<a, 2>::type, double>::value);

int main() {}
