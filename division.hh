#ifndef MANIFOLDS_FUNCTIONS_RECIRPOCAL_HH
#define MANIFOLDS_FUNCTIONS_RECIRPOCAL_HH

#include "function.hh"
#include "full_function_defs.hh"

namespace manifolds {
template <class Numerator,
	  class Denominator>
struct DivisionImpl : MultiFunction
{
  static const bool stateless =
    and_<is_stateless<Numerator>,
	 is_stateless<Denominator>>::value;

  DivisionImpl(){}
  DivisionImpl(Numerator n, Denominator d):
    n(n),d(d){}

  template <class ... Args>
  auto operator()(Args ... args) const
  {
    return n(args...) / d(args...);
  }

private:
  Numerator n;
  Denominator d;
};

DEF_FF_TEMPLATE(Division)

}

#endif
