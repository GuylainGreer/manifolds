#ifndef MANIFOLDS_FUNCTIONS_RECIRPOCAL_HH
#define MANIFOLDS_FUNCTIONS_RECIRPOCAL_HH

#include "function.hh"
#include "full_function_defs.hh"

namespace manifolds {
template <class Numerator, class Denominator>
struct Division
    : Function<list<int_<2>, typename Numerator::indices,
                    typename Denominator::indices>,
               max<Numerator::input_dim, Denominator::input_dim>::value,
               max<Numerator::output_dim, Denominator::output_dim>::value>,
      FunctionCommon<Division<Numerator, Denominator> > {
  using FunctionCommon<Division>::operator();
  static const bool stateless =
      and_<is_stateless<Numerator>, is_stateless<Denominator> >::value;
  static const ComplexOutputBehaviour complex_output =
      VariadicComplexOutput<Numerator, Denominator>::value;

  Division() {}
  Division(Numerator n, Denominator d) : n(n), d(d) {}

  template <class... Args> auto eval(Args... args) const {
    return n(args...) / d(args...);
  }

  auto GetNumerator() const { return n; }

  auto GetDenominator() const { return d; }

  auto GetFunctions() const { return make_my_tuple(n, d); }

private:
  Numerator n;
  Denominator d;
};

template <class D, class N, class U> bool operator==(Division<N, D>, U) {
  return false;
}

template <class N, class D>
bool operator==(Division<N, D> d1, Division<N, D> d2) {
  return VariadicEqual(d1, d2);
}

template <class N, class D> Division<N, D> DivideRaw(N n, D d) {
  return { n, d };
}
}

#endif
