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
}

#endif
