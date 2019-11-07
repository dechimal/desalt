#include <desalt/record.hpp>

#include <cassert>
#include <vector>

template<typename F, typename ...Args>
constexpr std::true_type sfinae_impl(decltype((void)std::declval<F>()(std::declval<Args>()...)) *) { return {}; }
template<typename ...>
constexpr std::false_type sfinae_impl(...) { return {}; }
template<typename ...Args, typename F>
constexpr decltype(sfinae_impl<F &&, Args...>(nullptr)) sfinae(F &&) { return {}; }

int main() {
    using desalt::record::record;
    using desalt::record::combine;

    {
        // construction with arbitrary member
        auto r = record {
            DESALT_MEMBER(n) = 42,
            DESALT_MEMBER(vec) = std::vector{1,2,3},
            DESALT_MEMBER(s) = record {
                DESALT_MEMBER(x) = 1,
                DESALT_MEMBER(y) = 2,
                DESALT_MEMBER(z) = 3,
            },
        };
        assert(r.n == 42);
        assert(r.vec == (std::vector{1,2,3}));
        assert(r.s.x == 1);
        assert(r.s.y == 2);
        assert(r.s.z == 3);

        // subscription by name
        assert(r[DESALT_MEMBER(n)] == 42);
        assert(r[DESALT_MEMBER(vec)] == (std::vector{1,2,3}));

        // assignment
        r = record {
            DESALT_MEMBER(s) = record {
                DESALT_MEMBER(x) = 10,
                DESALT_MEMBER(z) = 30
            },
            DESALT_MEMBER(vec) = std::vector{4,5,6},
        };

        assert(r.n == 42);
        assert(r.vec == (std::vector{4,5,6}));
        assert(r.s.x == 10);
        assert(r.s.y == 2);
        assert(r.s.z == 30);

        // combine two records
        auto r2 = combine(r, record {
            DESALT_MEMBER(a) = 5,
            DESALT_MEMBER(b) = 6,
            DESALT_MEMBER(s) = 7,
        });

        assert(r2.n == 42);
        assert(r2.vec == (std::vector{4,5,6}));
        assert(r2.a == 5);
        assert(r2.b == 6);
        assert(r2.s == 7);
        assert(&r2.a < &r2.s);
    }

    // multiple combine
    {
        constexpr auto r = combine(
            record { DESALT_MEMBER(x) = 1, DESALT_MEMBER(y) = 2 },
            record { DESALT_MEMBER(y) = 3, DESALT_MEMBER(z) = 4 },
            record { DESALT_MEMBER(z) = 5, DESALT_MEMBER(x) = 6 }
        );
        static_assert(r.y == 3);
        static_assert(r.z == 5);
        static_assert(r.x == 6);
        assert(&r.y < &r.z);
        assert(&r.z < &r.x);
    }

    // compare
    {
        auto a = record { DESALT_MEMBER(x) = 0 };
        auto b = record { DESALT_MEMBER(y) = 0 };
        static_assert(!sfinae<decltype(a), decltype(b)>([] (auto a, auto b, decltype((void)(a == b)) * = {}) {}));
        assert(a == a);

        assert((record { DESALT_MEMBER(x) = 12, DESALT_MEMBER(y) = 34 } == record { DESALT_MEMBER(x) = 12, DESALT_MEMBER(y) = 34 }));
        assert((record { DESALT_MEMBER(x) = 12, DESALT_MEMBER(y) = 34 } == record { DESALT_MEMBER(y) = 34, DESALT_MEMBER(x) = 12 }));
        assert((record { DESALT_MEMBER(x) = 12, DESALT_MEMBER(y) = 34 } != record { DESALT_MEMBER(x) = 34, DESALT_MEMBER(y) = 12 }));
        assert((record { DESALT_MEMBER(x) = 12, DESALT_MEMBER(y) = 34 } != record { DESALT_MEMBER(y) = 12, DESALT_MEMBER(x) = 34 }));
    }

    // swap
    {
        auto a = record { DESALT_MEMBER(x) = 12, DESALT_MEMBER(y) = 34 };
        auto b = record { DESALT_MEMBER(y) = 12, DESALT_MEMBER(x) = 34 };
        swap(a, b);
        assert(a.x == 34);
        assert(a.y == 12);
        assert(b.x == 12);
        assert(b.y == 34);
    }

    // member function and constexpr-ness with member function
    {
        constexpr auto r = record {
            DESALT_MEMBER(f).fun = [&] (auto && self, int x) {
                return self.x + x;
            },
            DESALT_MEMBER(x) = 42,
        };

        assert(r.f(3) == 45);          // any call of member function is not constexpr
        assert(r[DESALT_MEMBER(f)](3) == 45); // also access by symbol
        static_assert(r.x == 42);      // but value access is constexpr
        static_assert(combine(r, record { DESALT_MEMBER(y) = 5 }).x == 42);
    }
}
