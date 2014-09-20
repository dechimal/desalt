#include <vector>
#include <iostream>
#include <cstdlib>
#include <type_traits>

#include "boost/lexical_cast.hpp"
#include "attotest/attotest.hpp"

namespace attotest {

using boost::lexical_cast;

std::vector<std::pair<void(*)(), std::string>> tests;
std::string current_test;

void add_test(void(*f)(), char const * str) {
    tests.push_back(std::make_pair(f, str));
}

void print() {
    std::cout << std::endl;
}
template<typename Arg, typename ...Args>
void print(Arg const & arg, Args const & ...args) {
    std::cout << arg;;
    here::print(args...);
}

void assert_(bool cond, std::string const & file, std::size_t line, std::function<std::string()> msg) {
    if (cond) return;
    here::print("failed at ", file, ":", line, ", in ", current_test, ": ", msg());
    std::exit(1);
}

std::string to_string_fallback() {
    return "{value...}";
}

std::string to_string_(int x) {
    return here::lexical_cast<std::string>(x);
}
std::string to_string_(unsigned int x) {
    return here::lexical_cast<std::string>(x);
}
std::string to_string_(long x) {
    return here::lexical_cast<std::string>(x);
}
std::string to_string_(unsigned long x) {
    return here::lexical_cast<std::string>(x);
}
std::string to_string_(long long x) {
    return here::lexical_cast<std::string>(x);
}
std::string to_string_(unsigned long long x) {
    return here::lexical_cast<std::string>(x);
}
std::string to_string_(double x) {
    return here::lexical_cast<std::string>(x);
}
std::string to_string_(bool x) {
    return x ? "true" : "false";
}
std::string to_string_(std::string const & x) {
    return "\"" + x + "\"";
}
std::string to_string_(std::string && x) {
    return "\"" + x + "\"";
}
std::string to_string_(char const * x) {
    return "\"" + std::string(x) + "\"";
}

}

int main() {
    for (auto const & t : attotest::tests) {
        attotest::current_test = t.second;
        t.first();
    }
}
