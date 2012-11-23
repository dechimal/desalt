#include <vector>
#include <string>
#include <iostream>
#include "desalt/newtype.hpp"

// Haskellのnewtypeみたいなもの（ある型から，暗黙には元の型から/へは変換できない別の型を作る）

DESALT_NEWTYPE(ivector, (std::vector<int>),
    as_base,
    begin,
    end
);

// 次の型とだいたい同じ
// struct ivector : private std::vector<int> {
//     explicit ivector(std::vector<int> const &);
//     explicit ivector(std::vector<int> &&);
//     std::vector<int> & as_base();
//     std::vector<int> const & as_base() const;
//     using std::vector<int>::begin;
//     using std::vector<int>::end;
// };

DESALT_NEWTYPE(mystring, (std::string),
    std_string,
    this,
    auto append,
    namespace explicit operator(<<)(std::ostream &, mystring const &)
);

// struct mystring : private std::string {
//     explicit mystring(std::string const &);
//     explicit mystring(std::string &&);
//     std::string & std_string();
//     std::string const & std_string() const;
//     using std::string::basic_string;
//     mystring & append(mystring const &);
//     // ... その他のパラメータ中のstd::stringをmystringで置き換えたappendのオーバーロード
// };
// ostream & operator <<(std::ostream &, mystring const &);

template<typename T>
DESALT_NEWTYPE(darray, (std::vector<T>),
    unwrap,
    this,
    begin,
    end,
    typename value_type,
    new (
        void my_push_back(value_type const & x) {
            this->push_back(x);
            this->push_back(x);
        }
    ),
    explicit (push_back)(int);
);

// template<typename T>
// struct darray : std::vector<T> {
//     explicit darray(std::vector<T> const &);
//     explicit darray(std::vector<T> &&);
//     std::vector<T> & unwrap();
//     std::vector<T> const & unwrap() const;
//     using std::vector<T>::vector;
//     using std::vector<T>::begin;
//     using std::vector<T>::end;
//     using typename std::vector<T>::value_type;
//     void my_push_back(value_type const & x) {
//         this->push_back(x);
//         this->push_back(x);
//     }
// };

template<typename T>
void print_line(T const & x) {
    std::cout << x << '\n';
}

int main() {
    {
        ivector iv(std::vector<int>{1,2,3});
        for (auto i : iv) {
            ::print_line(i);
        }
        ::print_line("-----------------");

        std::vector<int> v = iv.as_base();
        for (auto i : v) {
            ::print_line(i);
        }
        ::print_line("-----------------");
    }

    {
        mystring s("hoge");
        s.append(s);
        // s.append(std::string("hoge"));
        ::print_line(s);
        ::print_line("-----------------");
    }

    {
        darray<int> d{1,2,3};
        d.my_push_back(4);
        for (auto i : d) {
            ::print_line(i);
        }
        ::print_line("-----------------");
    }
}
