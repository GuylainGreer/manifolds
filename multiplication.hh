#ifndef MANIFOLDS_FUNCTION_MULTIPLICATION_HH
#define MANIFOLDS_FUNCTION_MULTIPLICATION_HH

#include "function.hh"
#include "full_function_defs.hh"
#include "equals.hh"
#include "streaming.hh"
#include "simplify_variadic.hh"

namespace manifolds {

template <class ... Functions>
struct MultiplicationImpl :
    Function<max<Functions::input_dim...>::value,
	     max<Functions::output_dim...>::value>
{
  static const bool stateless =
    and_<is_stateless<Functions>...>::value;

  MultiplicationImpl(){}
  MultiplicationImpl(const Functions & ... functions):
    functions(functions...){}
  MultiplicationImpl(const std::tuple<Functions...> & f):
    functions(f){}

  template <class ... Args>
  auto operator()(Args ... args) const
  {
    return eval(int_<0>(), args...);
  }

  template <class ... Args>
  auto eval(int_<sizeof...(Functions)-1>, Args ... args) const
  {
    return std::get<sizeof...(Functions)-1>
      (functions)(args...);
  }

  template <int N, class ... Args>
  auto eval(int_<N>, Args ... args) const
  {
    typedef decltype(std::get<N>(functions)(args...)) result;
    static const int next_N = N + 1;
    return std::forward<result>(std::get<N>(functions)(args...)) *
      eval(int_<next_N>(), std::forward<Args>(args)...);
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
  std::tuple<Functions...> functions;
};

  DEF_FF_TEMPLATE(Multiplication)

  template <class Func, class ... Functions>
  std::ostream & operator<<(std::ostream & s,
			    Multiplication<Func, Functions...> m)
  {
    StreamVariadic("Multiplication", m, s);
    return s;
  }

  template <class F>
  auto Multiply(F f)
  {
    return f;
  }

  template <class F, class ... Fs>
  auto Multiply(F f, Fs...fs)
  {
    return f * Multiply(fs...);
  }

  template <class ... Functions>
  struct Simplification<
    Multiplication<Functions...>,1>
  {
    typedef bool_<
      has_abelian_arithmetic<
	Multiplication<
	  Functions...>>::value> Abelian;
    typedef decltype(SimplifyV<Multiplication>
		     (std::declval<std::tuple<Functions...>>(),
		      Abelian{})) type;

    static type Combine(Multiplication<Functions...> a)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying multiplication\n";
#endif
      return SimplifyV<Multiplication>(a.GetFunctions(),
				       Abelian{});
    }
  };

  template <class ... MFuncs, class C>
  struct Simplification<Composition<Multiplication<MFuncs...>,C>>
  {
    typedef Composition<Multiplication<MFuncs...>,C> in_type;
    static auto Combine(in_type c)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Distributing Composition "
	"into multiplication\n";
#endif
      return DistributeComposition<Multiplication>(c);
    }

    typedef decltype(Combine(std::declval<in_type>())) type;
  };
}

#endif
