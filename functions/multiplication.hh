#ifndef MANIFOLDS_FUNCTION_MULTIPLICATION_HH
#define MANIFOLDS_FUNCTION_MULTIPLICATION_HH

#include "function.hh"
#include "full_function_defs.hh"
#include "equals.hh"

namespace manifolds {

template <class... Functions>
struct Multiplication : Function<list<int_<5>, typename Functions::indices...>,
                                 max<Functions::input_dim...>::value,
                                 max<Functions::output_dim...>::value>,
                        FunctionCommon<Multiplication<Functions...> > {
  using FunctionCommon<Multiplication>::operator();
  static const bool stateless = and_<is_stateless<Functions>...>::value;
  static const ComplexOutputBehaviour complex_output =
      VariadicComplexOutput<Functions...>::value;

  Multiplication() {}
  Multiplication(const Functions &... functions) : functions(functions...) {}
  Multiplication(const tuple<Functions...> &f) : functions(f) {}

  template <class... Args> auto eval(Args... args) const {
    return eval(int_<0>(), args...);
  }

  template <class... Args>
  auto eval(int_<sizeof...(Functions) - 1>, Args... args) const {
    return std::get<sizeof...(Functions) - 1>(functions)(args...);
  }

  template <int N, class... Args> auto eval(int_<N>, Args... args) const {
    typedef decltype(std::get<N>(functions)(args...)) result;
    return std::forward<result>(std::get<N>(functions)(args...)) *
           eval(int_<N + 1>(), std::forward<Args>(args)...);
  }

  auto GetFunctions() const { return functions; }

private:
  tuple<Functions...> functions;
};

template <class... Fs> Multiplication<Fs...> MultiplyRaw(Fs... fs) {
  return { fs... };
}

template <class... Args, class T>
bool operator==(Multiplication<Args...> m, T t) {
  return VariadicEqual(m, t);
}
}

#endif
