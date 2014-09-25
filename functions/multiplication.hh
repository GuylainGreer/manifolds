#ifndef MANIFOLDS_FUNCTION_MULTIPLICATION_HH
#define MANIFOLDS_FUNCTION_MULTIPLICATION_HH

#include "function.hh"
#include "full_function_defs.hh"
#include "equals.hh"

namespace manifolds {

template <class ... Functions>
struct MultiplicationImpl :
    Function<
  list<int_<5>, typename Functions::indices...>,
  max<Functions::input_dim...>::value,
  max<Functions::output_dim...>::value>
{
  static const bool stateless =
    and_<is_stateless<Functions>...>::value;
  static const ComplexOutputBehaviour complex_output =
    VariadicComplexOutput<Functions...>::value;

  MultiplicationImpl(){}
  MultiplicationImpl(const Functions & ... functions):
    functions(functions...){}
  MultiplicationImpl(const tuple<Functions...> & f):
    functions(f){}

  template <class ... Args>
  auto operator()(Args ... args) const
  {
    return eval(int_<0>(), args...);
  }

  template <class ... Args>
  auto eval(int_<sizeof...(Functions)-1>, Args ... args) const
  {
    return get<sizeof...(Functions)-1>
      (functions)(args...);
  }

  template <int N, class ... Args>
  auto eval(int_<N>, Args ... args) const
  {
    typedef decltype(get<N>(functions)(args...)) result;
    return std::forward<result>(get<N>(functions)(args...)) *
      eval(int_<N + 1>(), std::forward<Args>(args)...);
  }

  auto GetFunctions() const
  {
    return functions;
  }

  template <class T>
  bool operator==(const T & t) const
  {
    return VariadicEqual(*this, t);
  }

private:
  tuple<Functions...> functions;
};

  DEF_FF_TEMPLATE(Multiplication)

  template <class ... Fs>
  Multiplication<Fs...> MultiplyRaw(Fs ... fs)
  {
    return {fs...};
  }

}

#endif
