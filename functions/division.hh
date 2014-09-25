#ifndef MANIFOLDS_FUNCTIONS_RECIRPOCAL_HH
#define MANIFOLDS_FUNCTIONS_RECIRPOCAL_HH

#include "function.hh"
#include "full_function_defs.hh"

namespace manifolds {
template <class Numerator,
	  class Denominator>
struct DivisionImpl :
    Function<
      list<int_<2>,
	   typename Numerator::indices,
	   typename Denominator::indices>,
      max<Numerator::input_dim,
	  Denominator::input_dim>::value,
      max<Numerator::output_dim,
	  Denominator::output_dim>::value>
{
  static const bool stateless =
    and_<is_stateless<Numerator>,
	 is_stateless<Denominator>>::value;
  static const ComplexOutputBehaviour complex_output =
    VariadicComplexOutput<Numerator, Denominator>::value;

  DivisionImpl(){}
  DivisionImpl(Numerator n, Denominator d):
    n(n),d(d){}

  template <class ... Args>
  auto operator()(Args ... args) const
  {
    return n(args...) / d(args...);
  }

  template <class ... Rights>
  bool operator==(const DivisionImpl<Rights...> & d) const
  {
    return VariadicEqual(*this, d);
  }

  template <class T>
  bool operator==(const T &) const
  {
    return false;
  }

  auto GetNumerator() const
  {
    return n;
  }

  auto GetDenominator() const
  {
    return d;
  }

  auto GetFunctions() const
  {
    return make_my_tuple(n,d);
  }

private:
  Numerator n;
  Denominator d;
};

  DEF_FF_TEMPLATE(Division)

}

#endif
