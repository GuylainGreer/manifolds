#ifndef MANIFOLDS_FUNCTIONS_UNARY_MINUS_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_UNARY_MINUS_SIMPLIFICATIONS_HH

#include "unary_minus.hh"

namespace manifolds
{
  template <class F>
  struct Simplification<
    UnaryMinus<UnaryMinus<F>>,
    /*um_um_f*/0>
  {
    typedef F type;
    static type Combine(UnaryMinus<UnaryMinus<F>> u)
    {
      SIMPLIFY_INFO("Simplifying unary minus of unary "
		    "minus of function\n");
      return u.GetFunction().GetFunction();
    }
  };

  template <class F>
  struct Simplification<
    UnaryMinus<F>, /*um_f*/1>
  {
    static auto Combine(UnaryMinus<F> u)
    {
      SIMPLIFY_INFO("Simplifying unary minus of function\n");
      auto f = Simplify(u.GetFunction());
      return UnaryMinus<decltype(f)>(f);
    }

    typedef decltype(Combine(std::declval<UnaryMinus<F>>())) type;
  };
}

#endif

