#if !defined DESALT_PARAMETER_PACK_HPP_INCLUDED_
#define      DESALT_PARAMETER_PACK_HPP_INCLUDED_

#include <type_traits>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/quote.hpp>
#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/end.hpp>
#include <boost/fusion/include/fold.hpp>

namespace desalt { namespace parameter_pack {

namespace aux {

namespace fu = boost::fusion::result_of;
namespace mpl = boost::mpl;

template<typename ...Ts>
struct type_seq {
    using type = type_seq;
};

template<typename Seq> struct head;
template<typename Seq> struct tail;
template<typename T, typename Seq> struct cons;
template<typename Seq> struct size;
template<typename Seq> struct reverse;
template<typename Ret, typename Seq> struct reverse_impl;
template<typename Seq1, typename Seq2> struct append;
template<typename Seq> struct from_mpl_seq;
template<typename Seq> struct from_fusion_seq;
template<typename It, typename End, typename Ret> struct from_fusion_seq_impl;
template<typename Seq, typename E> struct push;
template<typename First, typename Last> struct enumerate;
template<typename T, T First, T Last> struct enumerate_c;
template<typename T, T Last, T I, T ...Is> struct enumerate_impl;
template<typename Seq, typename N> struct at;
template<typename Seq, std::size_t N> struct at_c;

template<typename T, typename ...Ts>
struct head<type_seq<T, Ts...>> {
    using type = T;
};
template<typename T, typename ...Ts>
struct tail<type_seq<T, Ts...>>
    : type_seq<Ts...>
{};
template<typename T, typename ...Ts>
struct cons<T, type_seq<Ts...>> : type_seq<T, Ts...> {};
template<typename ...Ts>
struct size<type_seq<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)>
{};
template<typename Seq>
struct reverse
    : reverse_impl<Seq, type_seq<>>
{};
template<typename Ret>
struct reverse_impl<type_seq<>, Ret>
    : Ret
{};
template<typename T, typename ...Ts, typename ...Ret>
struct reverse_impl<type_seq<T, Ts...>, type_seq<Ret...>>
    : reverse_impl<type_seq<Ts...>, type_seq<T, Ret...>>
{};
template<typename ...Ts, typename ...Us>
struct append<type_seq<Ts...>, type_seq<Us...>>
    : type_seq<Ts..., Us...>
{};

template<typename Seq>
struct from_mpl_seq
    : mpl::fold<Seq, type_seq<>, mpl::quote2<push>>
{};

template<typename ...Ts, typename E>
struct push<type_seq<Ts...>, E>
    : type_seq<Ts..., E>
{};

template<typename Seq>
struct from_fusion_seq
    : from_fusion_seq_impl<
        fu::begin<Seq>, typename
        fu::end<Seq>::type,
        type_seq<>>
{};

template<typename It, typename End, typename ...Ts>
struct from_fusion_seq_impl<It, End, type_seq<Ts...>>
    : mpl::eval_if<
          fu::equal_to<typename It::type, End>,
          type_seq<typename Ts::type...>,
          from_fusion_seq_impl<
              fu::next<typename It::type>,
              End,
              type_seq<Ts..., fu::value_of<typename It::type>>>>
{};

template<typename First, typename Last> struct enumerate
    : enumerate_c<typename Last::value_type, First::value, Last::value>
{};
template<typename T, T First, T Last> struct enumerate_c
    : enumerate_impl<T, Last, First>
{};
template<typename T, T Last, T ...Is> struct enumerate_impl<T, Last, Last, Is...>
    : type_seq<std::integral_constant<T, Is>...>
{};
template<typename T, T Last, T I, T ...Is> struct enumerate_impl
    : enumerate_impl<T, Last, I + 1, Is..., I>
{};
template<typename Seq, typename N> struct at
    : at_c<Seq, N::value>
{};
template<typename T, typename ...Ts, std::size_t N> struct at_c<type_seq<T, Ts...>, N>
    : at_c<type_seq<Ts...>, N - 1>
{};
template<typename T, typename ...Ts> struct at_c<type_seq<T, Ts...>, 0> {
    using type = T;
};

}

using aux::type_seq;
using aux::head;
using aux::tail;
using aux::cons;
using aux::size;
using aux::reverse;
using aux::append;
using aux::from_mpl_seq;
using aux::from_fusion_seq;
using aux::enumerate;
using aux::enumerate_c;
using aux::at;
using aux::at_c;

}}

#endif
