#ifndef MANIFOLDS_FUNCTIONS_TUPLE_UTILITIES_HH
#define MANIFOLDS_FUNCTIONS_TUPLE_UTILITIES_HH

#include <tuple>
#include <utility>

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

}

#endif
