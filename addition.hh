#ifndef MANIFOLDS_FUNCTION_ADDITION_HH
#define MANIFOLDS_FUNCTION_ADDITION_HH

#include "function.hh"
#include "full_function_defs.hh"
#include "simplify_variadic.hh"
#include <utility>
#include <tuple>

namespace manifolds {

template <class ... Functions>
struct AdditionImpl : MultiFunction
{
  AdditionImpl(){}

  static const bool stateless =
    and_<is_stateless<Functions>...>::value;

  AdditionImpl(Functions ... fs):
    functions(fs...){}

  AdditionImpl(std::tuple<Functions...> f):
    functions(f){}

  template <class ... Args>
  auto operator()(Args && ... args) const
  {
    return eval<0>(std::forward<Args>(args)...);
  }

  template <int N, class ... Args>
  auto eval(Args && ... args) const
  {
    typedef decltype(std::get<N>(functions)(args...)) result;
    static const int next_N =
      N == sizeof...(Functions)-1?
      sizeof...(Functions)-1 : N+1;
    if(N == sizeof...(Functions) - 1)
      return std::forward<result>(std::get<N>(functions)(args...));
    else
      return std::forward<result>(std::get<N>(functions)(args...)) +
	eval<next_N>(std::forward<Args>(args)...);
  }

  auto GetFunctions() const
  {
    return functions;
  }

private:
  std::tuple<Functions...> functions;
};

  DEF_FF_TEMPLATE(Addition)

  template <class F>
  auto Add(F f)
  {
    return f;
  }

  template <class F, class ... Funcs>
  auto Add(F f, Funcs ... fs)
  {
    return f + Add(fs...);
  }

  template <class ... Functions>
  struct Simplification<
    Addition<Functions...>,
    typename std::enable_if<(sizeof...(Functions)>2)>::type>
  {
    typedef decltype(SimplifyV<Addition>
		     (std::declval<std::tuple<Functions...>>(),
		      std::true_type{})) type;

    static type Combine(Addition<Functions...> a)
    {
      return SimplifyV<Addition>(a.GetFunctions(),
				 std::true_type{});
    }
  };
}

#endif
