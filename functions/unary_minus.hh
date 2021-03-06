
#ifndef MANIFOLDS_FUNCTIONS_UNARY_MINUS_HH
#define MANIFOLDS_FUNCTIONS_UNARY_MINUS_HH

#include "full_function_defs.hh"
#include "function.hh"

namespace manifolds {

template <class Func>
struct UnaryMinus : Function<list<int_<24>, typename Func::indices>,
                             Func::input_dim, Func::output_dim>,
                    FunctionCommon<UnaryMinus<Func> > {
  static const bool stateless = is_stateless<Func>::value;
  static const ComplexOutputBehaviour complex_output = SameAsInput;
  UnaryMinus() {}
  UnaryMinus(const Func &f) : f(f) {}

  template <class... Args> auto eval(Args... args) const { return -f(args...); }

  auto GetFunction() const { return f; }

  auto GetFunctions() const { return make_my_tuple(f); }

private:
  Func f;
};

template <class T, class U> bool operator==(UnaryMinus<T>, U) { return false; }

template <class T> bool operator==(UnaryMinus<T> u1, UnaryMinus<T> u2) {
  return u1.GetFunction() == u2.GetFunction();
}

template <class F> UnaryMinus<F> Negative(F f) { return UnaryMinus<F>(f); }
}
#endif
