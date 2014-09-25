#ifndef MANIFOLDS_ZERO_FUNCTION_HH
#define MANIFOLDS_ZERO_FUNCTION_HH

#include "full_function_defs.hh"
#include <ostream>

namespace manifolds {
  struct ZeroImpl : Function<int_<25>, 1,1>
{
  static const bool stateless = true;
  static const ComplexOutputBehaviour complex_output =
    NeverComplex;

  template <class ... Args>
  auto operator()(Args...) const
  {
    return 0;
  }
};

  DEF_FULL_FUNCTION(Zero)

  static const Zero zero = Zero();

}

#endif
