#ifndef MANIFOLDS_FUNCTIONS_TUPLE_UTILITIES_HH
#define MANIFOLDS_FUNCTIONS_TUPLE_UTILITIES_HH

#include <tuple>
#include <utility>

#include "function.hh"

#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/sort.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/pop_front.hpp>

namespace manifolds {

  template <class Tuple, std::size_t ... befores,
	    std::size_t ... afters>
  auto remove_element_helper(const Tuple & t,
			     std::integer_sequence<
			     std::size_t, befores...>,
			     std::integer_sequence<
			     std::size_t, afters...>)
  {
    static const std::size_t offset = sizeof...(befores)+1;
    auto r = std::make_tuple(std::get<befores>(t)...,
			     std::get<afters+offset>(t)...);
    return r;
  }

  template <int index, class Tuple>
  auto remove_element(const Tuple & t)
  {
    typedef std::make_index_sequence<index> befores;
    typedef std::make_integer_sequence<
      std::size_t, std::tuple_size<Tuple>::value-index-1>
      afters;
    return remove_element_helper(t, befores(), afters());
  }

  template <class Tuple>
  auto remove_element(const Tuple & t,
		      boost::mpl::vector<>)
  {
    return t;
  }

  template <class Tuple, class Vector>
  auto remove_elements(const Tuple & t, Vector)
  {
    return remove_element<
      boost::mpl::at_c<Vector, 0>::type::value>
      (remove_elements(t, typename boost::mpl::pop_front<
		       Vector>::type()));
  }

  template <int ... indices, class Tuple>
  auto remove_elements(const Tuple & t)
  {
    typedef typename boost::mpl::sort<
      boost::mpl::vector_c<int,indices...>
      >::type sorted_list;
    return remove_elements(t, sorted_list());
  }

  template <int index, class Tuple, class E,
	    std::size_t ... befores,
	    std::size_t ... afters>
  auto insert_element_helper(const Tuple & t,
			     const E & e,
			     std::integer_sequence<
			     std::size_t, befores...>,
			     std::integer_sequence<
			     std::size_t, afters...>)
  {
    auto before = std::make_tuple(std::get<befores>(t)...);
    auto middle = std::make_tuple(e);
    auto after  = std::make_tuple(std::get<afters+index>(t)...);
    return std::tuple_cat(before, middle, after);
  }

  template <int index, class Tuple, class E>
  auto insert_element(const Tuple & t, const E & e)
  {
    return insert_element_helper<index>
      (t, e, std::make_index_sequence<index>(),
       std::make_index_sequence<
       std::tuple_size<Tuple>::value - index>());
  }

  template <class Tuple, class E>
  auto push_back(const Tuple & t, const E & e)
  {
    return insert_element<
      std::tuple_size<Tuple>::value>(t, e);
  }

  template <class T>
  struct array_size;

  template <class T, std::size_t n>
  struct array_size<std::array<T,n>> :
    int_<n>{};

  template <class T, std::size_t n>
  struct array_size<const std::array<T,n> &> :
    int_<n>{};

  template <class T, std::size_t n>
  struct array_size<std::array<T,n> &> :
    int_<n>{};

  template <class T, std::size_t n>
  struct array_size<T[n]> :
    int_<n>{};

  template <class T, std::size_t n>
  struct array_size<T(&)[n]> :
    int_<n>{};

  template <class T, std::size_t n>
  struct array_size<const T(&)[n]> :
    int_<n>{};
}

#endif
