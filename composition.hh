#ifndef MANIFOLDS_FUNCTION_COMPOSITION_HH
#define MANIFOLDS_FUNCTION_COMPOSITION_HH

#include "function.hh"

namespace manifolds {

template <class ... Functions>
struct Composition : Function, MultiFunction
{
  Composition(){}
  Composition(const Functions & ... functions):
    functions(functions...){}

  template <class ... Args>
  auto operator()(Args && ... args) const
  {
    static const int last = sizeof...(Functions)-1;
    return eval<last-1>(std::get<last>(functions)(args...));
  }

  template <int N, class Arg>
  auto eval(Arg && arg) const
  {
    typedef decltype(std::get<N>(functions)(arg)) result;
    static const int next_N = N == 0? 0 : N-1;
    if(N == 0)
      return std::forward<result>(std::get<N>(functions)(arg));
    else
      return eval<next_N>(std::forward<result>
			  (std::get<N>(functions)(arg)));
  }

private:
  std::tuple<Functions...> functions;
};

}

#endif
