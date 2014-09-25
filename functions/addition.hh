#ifndef MANIFOLDS_FUNCTION_ADDITION_HH
#define MANIFOLDS_FUNCTION_ADDITION_HH

#include "function.hh"
#include "full_function_defs.hh"
#include <utility>
#include "equals.hh"

namespace manifolds {

template <class ... Functions>
struct AdditionImpl : Function<
  list<int_<0>, typename Functions::indices...>,
  max<Functions::input_dim...>::value,
  max<Functions::output_dim...>::value>
{
  AdditionImpl(){}

  static const bool stateless =
    and_<is_stateless<Functions>...>::value;

  static const ComplexOutputBehaviour complex_output =
    VariadicComplexOutput<Functions...>::value;

  AdditionImpl(Functions ... fs):
    functions(fs...){}

  AdditionImpl(tuple<Functions...> f):
    functions(f){}

  template <class ... Args>
  auto operator()(Args ... args) const
  {
    return eval(int_<0>(), args...);
  }

  template <class ... Args>
  auto eval(int_<sizeof...(Functions)-1>,
	    Args ... args) const
  {
    return get<sizeof...(Functions)-1>
      (functions)(args...);
  }

  template <int N, class ... Args>
  auto eval(int_<N>, Args ... args) const
  {
      return get<N>(functions)(args...) +
	eval(int_<N+1>(), args...);
  }

  auto GetFunctions() const
  {
    return functions;
  }

  template <class Other>
  bool operator==(const Other &) const
  {
    return false;
  }

  template <class ... Rights>
  bool operator==(const AdditionImpl<Rights...> & r) const
  {
    return VariadicEqual(*this, r);
  }

private:
  tuple<Functions...> functions;
};

  DEF_FF_TEMPLATE(Addition)

  template <class ... Fs>
  auto AddRaw(Fs ... fs)
  {
    return Addition<Fs...>(fs...);
  }
}

#endif
