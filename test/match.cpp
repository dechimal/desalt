#include <cassert>
#include <desalt/match.hpp>
#include <vector>
#include <map>
#include <string>
#include <string_view>
#include <iostream>
#include <type_traits>
#include <desalt/match/macros.hpp>

using namespace desalt::match::literals;
using desalt::match::match;
using desalt::match::tag;
using desalt::match::at;

template<typename> void f[[deprecated]]();

// tiny json type
struct json;
using json_array = std::vector<json>;
using json_object = std::map<std::string, json>;
using json_base = std::variant<std::nullptr_t, bool, double, std::string, json_array, json_object>;
struct json : json_base {
    using json_base::json_base;
    json(char const * s) : json_base(std::string(s)) {}
    template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    json(T i) : json_base(double(i)) {}
    json(std::initializer_list<json> && l) : json_base(json_array(std::move(l))) {}
    json(std::initializer_list<std::pair<std::string const, json>> && l) : json_base(json_object(std::move(l))) {}
};

// traits for json type
namespace desalt::match::traits {

template<> struct pattern<json> : pattern<json_base> {};

}

int main() {
    // patterns
    1_;
    -1_;
    1_[1];
    1_[1](1_, 2_);
    +1_[1](1_, 2_);

    // template<typename T, typename ...Patterns>
    // std::optional<std::tuple<MatchResults...>> match(T && value, Patterns && ...patterns)
    //
    // valueをpatternsでパターンマッチする．
    // patternsの各パターンp_iは，valueに対する述語(test_iとする)と，
    // test_i(value)が真である場合にvalueから値を得る関数(get_iとする)を規定する．
    // test_i(value)が全て真であるとき，マッチは成功する．
    // result_iをget_i(value)の戻り値，nをsizeof...(patterns)として，
    // 成功した場合の結果はstd::tuple_cat(result_0, result_1..., result_n-1)である．
    //
    // 引数:
    //   value: パターンマッチングの対象となる値
    //   patterns: パターンの列
    // 戻り値:
    //   valueがpatternsにマッチすると，戻り値は各要素がpatternsによって規定されるtupleを持つ．
    //   マッチに失敗すると，戻り値は空である．

    // for tuple
    auto v = std::tuple(1, '2', 3ul);
    // index pattern
    if (auto t = ::match(v, 1_)) {
        auto && [x] = *t;
        static_assert(std::is_same_v<decltype(x), char&>);
        assert(x == '2');
    } else {
        assert(false);
    }
    // tag pattern
    if (auto t = ::match(v, ::tag<int>)) {
        auto && [x] = *t;
        static_assert(std::is_same_v<decltype(x), int&>);
        assert(x == 1);
    } else {
        assert(false);
    }
    // multiple patterns
    if (auto t = ::match(std::tuple(1, '2', 3ul), 2_, 0_)) {
        auto && [x, y] = *t;
        static_assert(std::is_same_v<decltype(x), unsigned long&&>);
        static_assert(std::is_same_v<decltype(y), int&&>);
        assert(x == 3);
        assert(y == 1);
    } else {
        assert(false);
    }
    // value pattern
    if (auto t = ::match(std::tuple(1, '2', 3ul), 2_[3u], 0_)) {
        auto && [y] = *t;
        static_assert(std::is_same_v<decltype(y), int&&>);
        assert(y == 1);
    } else {
        assert(false);
    }
    // value pattern, failed to match
    if (auto t = ::match(std::tuple(1, '2', 3ul), 2_[42u], 0_)) {
        auto && [y] = *t;
        static_assert(std::is_same_v<decltype(y), int&&>);
        assert(false);
    }
    // value pattern, with binding
    if (auto t = ::match(std::tuple(1, '2', 3ul), +2_[3u], 0_)) {
        auto && [x, y] = *t;
        static_assert(std::is_same_v<decltype(x), unsigned long&&>);
        static_assert(std::is_same_v<decltype(y), int&&>);
        assert(x == 3);
        assert(y == 1);
    } else {
        assert(false);
    }
    // structured pattern
    if (auto t = ::match(std::tuple(1, '2', std::tuple(3ul, 4.0f, 5.0)), 2_(1_, 2_), 1_)) {
        auto && [x, y, z] = *t;
        static_assert(std::is_same_v<decltype(x), float&&>);
        static_assert(std::is_same_v<decltype(y), double&&>);
        static_assert(std::is_same_v<decltype(z), char&&>);
        assert(x == 4);
        assert(y == 5);
        assert(z == '2');
    } else {
        assert(false);
    }

    // for variant
    if (auto t = ::match(std::variant<int, char, unsigned>('2'), 1_)) {
        auto && [x] = *t;
        static_assert(std::is_same_v<decltype(x), char&&>);
        assert(x == '2');
    } else {
        assert(false);
    }

    // more complex type
    auto j = json{
        "hoge", 1, nullptr, {std::pair{"a", 2}, {"b", 3}}
    };
    if (auto t = ::match(
        j,
        tag<json_array>(
            ::at(1)(tag<double>),
            ::at(0)(
                +tag<std::string>["hoge"]
            ),
            ::at(3)(
                tag<json_object>(
                    ::at("b"),
                    ::at("a")[json(2)]
                )
            )
        ))
    ) {
        auto && [a, b, c] = *t;
        static_assert(std::is_same_v<decltype(a), double&>);
        static_assert(std::is_same_v<decltype(b), std::string&>);
        static_assert(std::is_same_v<decltype(c), json&>);
        assert(a == 1);
        assert(b == "hoge");
        assert(c == json(3));
    } else {
        assert(false);
    }

    // macro syntax
    // match-statement:
    //     DESALT_MATCH ( expression ) { match-case-statement... }
    // match-case-statement:
    //     CASE ( pattern... ) ( identifier... ) statement

    DESALT_MATCH (j) {
        DESALT_CASE (
            tag<json_array>(
                ::at(1)(tag<double>),
                ::at(0)(
                    +tag<std::string>["hoge"]
                ),
                ::at(3)(
                    tag<json_object>(
                        ::at("b"),
                        ::at("a")[json(2)]
                    )
                )
            )
        ) (a, b, c) {
            static_assert(std::is_same_v<decltype(a), double&>);
            static_assert(std::is_same_v<decltype(b), std::string&>);
            static_assert(std::is_same_v<decltype(c), json&>);
            assert(a == 1);
            assert(b == "hoge");
            assert(c == json(3));
        }
        DESALT_CASE ()() {
            assert(false);
        }
    }

    return 0;
}
