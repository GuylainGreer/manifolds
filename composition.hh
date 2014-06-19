#ifndef MANIFOLDS_FUNCTION_COMPOSITION_HH
#define MANIFOLDS_FUNCTION_COMPOSITION_HH

#include "is_function.hh"
#include "function.hh"

namespace manifolds {

template <class ... Functions>
struct Composition : MultiFunction
{
  static const bool stateless =
    and_<is_stateless<Functions>...>::value;

  Composition(){}
  Composition(const Functions & ... functions):
    functions(functions...){}

  template <class Arg, class ... Args,
	    class = typename std::enable_if<
	      !is_function<Arg>::value
	      >::type>
  auto operator()(Arg arg, Args && ... args) const
  {
    static const int last = sizeof...(Functions)-1;
    return eval<last-1>(std::get<last>(functions)(arg, args...));
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

  template <class InnerFunc>
  using prepare = typename std::remove_reference<
    typename std::remove_cv<
      InnerFunc>::type
    >::type;

  //Return a composite function
  template <class InnerFunc, class = typename std::enable_if<
			       is_function<InnerFunc>::value
			       >::type>
  auto operator()(InnerFunc && f) const
  {
    return Composition<
      Composition,
      prepare<InnerFunc>>(*this, f);
  }

private:
  std::tuple<Functions...> functions;
};

}

#endif
