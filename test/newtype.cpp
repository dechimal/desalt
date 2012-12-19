#include <vector>
#include <string>
#include <iostream>
#include "desalt/newtype.hpp"

// Haskellのnewtypeみたいなもの（ある型から，暗黙には元の型から/へは変換できない別の型を作る）

DESALT_NEWTYPE(ivector, std::vector<int>,
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

DESALT_NEWTYPE(mystring, std::string,
    std_string,
    this,
    auto append,
    namespace explicit (operator<<)(std::ostream &, mystring const &)
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
DESALT_NEWTYPE(darray, std::vector<T>,
    unwrap,
    this,
    typename value_type,
    new (
        void my_push_back(value_type const & x) {
            this->push_back(x);
            this->push_back(x);
        }
    ),
    new (
        DESALT_NEWTYPE(iterator, typename std::vector<T>::iterator,
            base,
            friend class darray,
            this,
            operator*,
            operator++,
            operator--,
            operator->,
            namespace operator!=,
            operator+=,
            operator-=,
            namespace operator+,
            namespace operator-,
            operator[],
            namespace operator<,
            namespace operator>,
            namespace operator<=,
            namespace operator>=,
            value_type,
            difference_type,
            pointer,
            reference,
            iterator_category
        );
    ),
    new (
        using const_iterator = iterator const;
        iterator begin() { return iterator(std::vector<T>::begin()); }
        iterator end() { return iterator(std::vector<T>::end()); }
        const_iterator begin() const { return iterator(std::vector<T>::begin()); }
        const_iterator end() const { return iterator(std::vector<T>::end()); }
    ),
    explicit (insert)(iterator, T const &)
);

// template<typename T>
// struct darray : std::vector<T> {
//     explicit darray(std::vector<T> const &);
//     explicit darray(std::vector<T> &&);
//     std::vector<T> & unwrap();
//     std::vector<T> const & unwrap() const;
//     using std::vector<T>::vector;
//     using typename std::vector<T>::value_type;
//     void my_push_back(value_type const & x) {
//         this->push_back(x);
//         this->push_back(x);
//     }
//     struct iterator : private std::vector<T>::iterator {
//         // (snip)
//     };
//     using std::vector<T>::begin;
//     using std::vector<T>::end;
//     using const_iterator = iterator const;
//     iterator begin() { return iterator(std::vector<T>::begin()); }
//     iterator end() { return iterator(std::vector<T>::end()); }
//     const_iterator begin() const { return iterator(std::vector<T>::begin()); }
//     const_iterator end() const { return iterator(std::vector<T>::end()); }
// };

// Syntax and feature list (*-ed features now supported)
// DESALT_NEWTYPE(fuga, (hoge<int, int>),         // *new type and underlying type
//     to_base,                                   // *function getting own reference as underlying type
//     f,                                         // *using
//     this,                                      // *inheriting ctor
//     operator+,                                 // *operator using
//     (operator,),                               // operator using (for operator,)
//     explicit (this)(int, int),                 // ctor forwarding with specified signature
//     explicit (f)(int),                         // *forwarding with specified signature
//     explicit (operator+)(),                    // unary operator forwarding with specified signature
//     explicit (operator++)(int),                // unary postfix operator forwarding with specified signature
//     explicit (operator+)(int),                 // binary operator forwarding with specified signature
//     auto f,                                    // *auto wrapped forwarding
//     auto operator+,                            // *auto wrapped operator forwarding
//     auto (operator,),                          // auto wrapped operator forwarding (for operator,)
//     namespace explicit (f)(int),               // *non-member function forwarding with specified signature
//     namespace explicit (operator+)(int),       // *non-member unary operator forwarding with specified signature
//     namespace explicit (operator++)(int, int), // non-member unary postfix operator forwarding with specified signature
//     namespace explicit (operator+)(int, int),  // *non-member binary operator forwarding with specified signature
//     namespace f,                               // auto wrapped non-member function forwarding
//     namespace operator+,                       // **auto wrapped non-member operator forwarding (** only binary op)
//     namespace (operator,),                     // auto wrapped non-member operator forwarding (for operator,)
//     friend f,                                  // *friend function declaration
//     friend class piyo,                         // *friend class declaration
//     typename type,                             // *using as typename
//     new ( void f() {} ),                       // *manual definition
//                                                // *no operation
// );

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
        d.insert(d.begin(), 5);
        auto i = d.begin();
        static_assert(!std::is_same<decltype(i), std::vector<int>::iterator>::value, "");
        std::cout << *i << '\n';
        std::cout << *i.base() << '\n';
        ++i;
        std::cout << *i << '\n';
        i += 3;
        std::cout << *i << '\n';
        std::cout << *(i - 1) << '\n';
        auto d2 = d;
        // d.insert(d2.begin(), d2.end());
        for (auto i : d) {
            ::print_line(i);
        }
        ::print_line("-----------------");
    }
}
