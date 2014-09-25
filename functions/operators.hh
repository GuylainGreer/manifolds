#ifndef MANIFOLDS_FUNCTION_OPERATORS_HH
#define MANIFOLDS_FUNCTION_OPERATORS_HH

#include "full_function_defs.hh"
#include "addition.hh"
#include "multiplication.hh"
#include "division.hh"
#include "unary_minus.hh"
#include "simplify.hh"
#include "all_simplifications.hh"

namespace manifolds
{
  template <class ... Functions>
  using e_if_funcs = typename std::enable_if<
    and_<is_function<Functions>...>::value>::type;

  template <class A, class B,
	    class = e_if_funcs<A,B>>
  auto operator+(A a, B b)
  {
    return Simplify(Addition<A,B>(a,b));
  }

  template <class A,
	    class = e_if_funcs<A>>
  auto operator-(A a)
  {
    return Simplify(UnaryMinus<A>(a));
  }

  template <class A, class B,
	    class = e_if_funcs<A,B>>
  auto operator-(A a, B b)
  {
    return a + -b;
  }

  template <class A, class B,
	    class = e_if_funcs<A,B>>
  auto operator*(A a, B b)
  {
    return Simplify(Multiplication<A,B>(a,b));
  }

  template <class A, class B,
	    class = e_if_funcs<A,B>>
  auto operator/(A a, B b)
  {
    return Simplify(Division<A,B>(a,b));
  }
}

#endif
