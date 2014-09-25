#ifndef MANIFOLDS_FUNCTIONS_COMPLEX_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_COMPLEX_SIMPLIFICATIONS_HH

#include "simplify.hh"
#include "complex.hh"
#include "variables.hh"
#include "zero.hh"

namespace manifolds
{
  template <class T>
  struct Simplification<
    Composition<Real, ImagN<T>>, 0>
  {
    typedef Zero type;
    static type Combine(Composition<Real, ImagN<T>>)
    {
      SIMPLIFY_INFO("Simplifying Real part of "
		    "non-complex function");
      return zero;
    }
  };
}

#endif
