#ifndef MANIFOLDS_FUNCTION_OPERATORS_HH
#define MANIFOLDS_FUNCTION_OPERATORS_HH

#include "full_function_defs.hh"
#include "addition.hh"
#include "multiplication.hh"
#include "division.hh"
#include "unary_minus.hh"
#include "simplify.hh"

namespace manifolds
{
  template <class A, class B>
  auto operator+(A a, B b)
  {
    return Simplify(Addition<A,B>(a,b));
  }

  template <class A>
  auto operator-(A a)
  {
    return UnaryMinus<A>(a);
  }

  template <class A, class B>
  auto operator-(A a, B b)
  {
    return a + -b;
  }

  template <class A, class B>
  auto operator*(A a, B b)
  {
    return Simplify(Multiplication<A,B>(a,b));
  }

  template <class A, class B>
  auto operator/(A a, B b)
  {
    return Division<A,B>(a,b);
  }
}

#endif
