#ifndef MANIFOLDS_VARIABLE_INTERFACE_HH
#define MANIFOLDS_VARIABLE_INTERFACE_HH

#include "function.hh"
#include <tuple>

namespace manifolds {

  template <int N>
  struct Variable : Function, MultiFunction
  {
    template <class ... Args>
    auto&& operator()(Args && ... args) const
    {
      static_assert(N < sizeof...(Args),
		    "Not enough arguments to extract Nth");
      typedef decltype(std::tie(args...)) tuple_type;
      typedef typename std::tuple_element<N, tuple_type>::type tuple_elem;
      return std::forward<tuple_elem>(std::get<N>(tuple_type(args...)));
    }
  };

  static const Variable<0> t;
  static const Variable<1> x;
  static const Variable<2> y;
  static const Variable<3> z;
}

#endif
