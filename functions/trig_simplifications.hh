#ifndef MANIFOLDS_FUNCTIONS_TRIG_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_TRIG_SIMPLIFICATIONS_HH

#include "multiplication.hh"
#include "polynomial.hh"

namespace manifolds {

  template <>
  struct Simplification<
    Multiplication<Cos, Cos>,
    /*mult_cos_cos*/0>
  {
    typedef Multiplication<Cos, Cos> in_type;
    static auto Combine(in_type)
    {
      SIMPLIFY_INFO("Simplifying cos**2\n");
      return ComposeRaw(GetIPolynomial<1,0,-1>(), Sin());
    }
  };
}

#endif
