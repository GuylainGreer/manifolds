#ifndef MANIFOLDS_FUNCTIONS_TUPLE_UTILITIES_HH
#define MANIFOLDS_FUNCTIONS_TUPLE_UTILITIES_HH

#include <utility>
#include "function.hh"
#include "tmp_utils.hh"
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/sort.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/size.hpp>
#include <array>
#include <tuple>

namespace manifolds {
using std::tuple;

template <class... Args> tuple<Args...> make_my_tuple(Args... args) {
  return std::make_tuple(args...);
}

template <class... Ts, class... Args>
auto make_same_as(const tuple<Ts...> &, Args... args) {
  return make_my_tuple(args...);
}

inline auto tuple_cat() { return tuple<>(); }

template <class T> auto tuple_cat(const T &t) { return t; }

template <class T1, class T2> auto tuple_cat(const T1 &t1, const T2 &t2) {
  return tuple_cat(t1, t2,
                   std::make_index_sequence<std::tuple_size<T1>::value>(),
                   std::make_index_sequence<std::tuple_size<T2>::value>());
}

template <class Tuple1, class Tuple2, class... Tuples>
auto tuple_cat(const Tuple1 &t1, const Tuple2 &t2, const Tuples &... tuples) {
  return tuple_cat(tuple_cat(t1, t2), tuples...);
}

template <class Tuple, std::size_t... befores, std::size_t... afters>
auto remove_element_helper(const Tuple &t,
                           std::integer_sequence<std::size_t, befores...>,
                           std::integer_sequence<std::size_t, afters...>) {
  using namespace std;
  static const std::size_t offset = sizeof...(befores) + 1;
  auto r =
      make_same_as(t, std::get<befores>(t)..., std::get<afters + offset>(t)...);
  return r;
}

template <int index, class Tuple> auto remove_element(const Tuple &t) {
  typedef std::make_index_sequence<index> befores;
  typedef std::make_integer_sequence<
      std::size_t, std::tuple_size<Tuple>::value - index - 1> afters;
  return remove_element_helper(t, befores(), afters());
}

template <class Tuple>
auto remove_element(const Tuple &t, boost::mpl::vector<>) {
  return t;
}

template <class A, class... As, class B, class... Bs>
bool operator==(const tuple<A, As...> &a, const tuple<B, Bs...> &b) {
  return std::get<0>(a) == std::get<0>(b) &&
         remove_element<0>(a) == remove_element<0>(b);
}

template <
    class Tuple, class Vector,
    class = typename std::enable_if<boost::mpl::size<Vector>::value == 0>::type>
auto remove_elements(const Tuple &t, Vector, long) {
  return t;
}

template <
    class Tuple, class Vector,
    class = typename std::enable_if<boost::mpl::size<Vector>::value != 0>::type>
auto remove_elements(const Tuple &t, Vector, int) {
  return remove_element<boost::mpl::at_c<Vector, 0>::type::value>(
      remove_elements(t, typename boost::mpl::pop_front<Vector>::type(), 0));
}

template <int... indices, class Tuple> auto remove_elements(const Tuple &t) {
  typedef typename boost::mpl::sort<
      boost::mpl::vector_c<int, indices...> >::type sorted_list;
  return remove_elements(t, sorted_list(), 0);
}

template <int index, class Tuple, class E, std::size_t... befores,
          std::size_t... afters>
auto insert_element_helper(const Tuple &t, const E &e,
                           std::integer_sequence<std::size_t, befores...>,
                           std::integer_sequence<std::size_t, afters...>) {
  using namespace std;
  auto before = make_same_as(t, std::get<befores>(t)...);
  auto middle = make_same_as(t, e);
  auto after = make_same_as(t, std::get<afters + index>(t)...);
  return tuple_cat(before, middle, after);
}

template <int index, class Tuple, class E>
auto insert_element(const Tuple &t, const E &e) {
  return insert_element_helper<index>(
      t, e, std::make_index_sequence<index>(),
      std::make_index_sequence<std::tuple_size<Tuple>::value - index>());
}

template <class Tuple, class E> auto push_back(const Tuple &t, const E &e) {
  return insert_element<std::tuple_size<Tuple>::value>(t, e);
}

template <class T> struct array_size;

template <class T, std::size_t n>
struct array_size<std::array<T, n> > : int_<n> {};

template <class T, std::size_t n>
struct array_size<const std::array<T, n> &> : int_<n> {};

template <class T, std::size_t n>
struct array_size<std::array<T, n> &> : int_<n> {};

template <class T, std::size_t n> struct array_size<T[n]> : int_<n> {};

template <class T, std::size_t n> struct array_size<T (&)[n]> : int_<n> {};

template <class T, std::size_t n>
struct array_size<const T (&)[n]> : int_<n> {};

FUNC_CHECK_DEF(is_gettable, decltype(std::get<0>(std::declval<T>())))

template <int start, int size, class Tuple>
auto subset_helper(const Tuple &t, int_<start>, int_<size>) {
  typedef boost::mpl::range_c<int, 0, start> first_r;
  typedef typename boost::mpl::copy<
      first_r, boost::mpl::back_inserter<boost::mpl::vector<> > >::type first_v;
  typedef typename boost::mpl::if_c<
      (start + size > std::tuple_size<Tuple>::value),
      boost::mpl::range_c<int, 0, 0>,
      boost::mpl::range_c<int, start + size,
                          std::tuple_size<Tuple>::value> >::type second_r;
  typedef typename boost::mpl::copy<
      second_r, boost::mpl::back_inserter<boost::mpl::vector<> > >::type
  second_v;
  return remove_elements(remove_elements(t, second_v(), 0), first_v(), 0);
}

template <int start, class Tuple>
auto subset_helper(const Tuple &, int_<start>, int_<0>) {
  return tuple<>();
}

template <int start, int size, class Tuple> auto subset(const Tuple &t) {
  return subset_helper(t, int_<start>(), int_<size>());
}

template <template <class...> class Variadic, class Tuple,
          std::size_t... indices>
auto MakeVariadic(const Tuple &t,
                  std::integer_sequence<std::size_t, indices...>) {
  return Variadic<typename std::tuple_element<indices, Tuple>::type...>(
      std::get<indices>(t)...);
}

template <template <class...> class Variadic, class Tuple>
auto MakeVariadic(Tuple t) {
  return MakeVariadic<Variadic>(
      t, std::make_index_sequence<std::tuple_size<Tuple>::value>());
}

template <class... Types, std::size_t... is>
auto ToArray(const tuple<Types...> &t,
             std::integer_sequence<std::size_t, is...>) {
  return std::array<typename std::common_type<Types...>::type,
                    sizeof...(Types)>{ { std::get<is>(t)... } };
}

template <class... Types> auto ToArray(const tuple<Types...> &t) {
  return ToArray(t, std::index_sequence_for<Types...>());
}

template <int i, class... Args, class A>
auto replace_element(const tuple<Args...> &t, const A &a) {
  return insert_element<i>(remove_element<i>(t), a);
}
}

#endif
