
#include <desalt/struct.hpp>
#include <vector>
#include <cassert>

int main() {

int a = 42;
    auto v = DESALT_STRUCT(x, &y, z)(1, a, std::vector<int>{1, 2, 3});
    a = 50;

    assert(v.x == 1);
    assert(v.y == 50);
    assert(v.z == (std::vector<int>{1,2,3}));
}
