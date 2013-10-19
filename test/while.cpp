
#include <iostream>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/arithmetic/inc.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <desalt/preprocessor/while.hpp>

#define P0(z, st) BOOST_PP_EQUAL(st, 0)
#define P1(z, st) BOOST_PP_EQUAL(st, 0)
#define P2(z, st) BOOST_PP_EQUAL(st, 0)
#define P3(z, st) BOOST_PP_EQUAL(st, 0)
#define P4(z, st) BOOST_PP_EQUAL(st, 0)
#define P5(z, st) BOOST_PP_EQUAL(st, 0)

#define F0(z, st) BOOST_PP_INC(DESALT_PP_WHILE(P1, F1, st))
#define F1(z, st) BOOST_PP_INC(DESALT_PP_WHILE(P2, F2, st))
#define F2(z, st) BOOST_PP_INC(DESALT_PP_WHILE(P3, F3, st))
#define F3(z, st) BOOST_PP_INC(DESALT_PP_WHILE(P4, F4, st))
#define F4(z, st) BOOST_PP_INC(DESALT_PP_WHILE(P5, F5, st))
#define F5(z, st) BOOST_PP_INC(st)

#define VAL DESALT_PP_WHILE(P0, F0, 0)

int main() {
    static_assert(VAL == 6, "faild: " BOOST_PP_STRINGIZE(VAL));
}
