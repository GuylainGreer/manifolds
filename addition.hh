#ifndef MANIFOLDS_FUNCTION_ADDITION_HH
#define MANIFOLDS_FUNCTION_ADDITION_HH

#include "function.hh"
#include "full_function_defs.hh"
#include "simplify_variadic.hh"
#include <utility>

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

private:
  tuple<Functions...> functions;
};

  DEF_FF_TEMPLATE(Addition)

  template <class Func, class ... Functions>
  std::ostream & operator<<(std::ostream & s,
			    Addition<Func, Functions...> m)
  {
    StreamVariadic("Addition", m, s);
    return s;
  }

  template <class ... Fs>
  auto AddRaw(Fs ... fs)
  {
    return Addition<Fs...>(fs...);
  }

  template <class ... Funcs>
  auto Add(Funcs ... fs)
  {
    return Simplify(AddRaw(fs...));
  }

  template <class ... Functions>
  struct Simplification<
    Addition<Functions...>, 2>
  {
    typedef decltype(SimplifyV<Addition>
		     (std::declval<tuple<Functions...>>(),
		      std::true_type{})) type;

    static type Combine(Addition<Functions...> a)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying Addition\n";
#endif
      return SimplifyV<Addition>(a.GetFunctions(),
				 std::true_type{});
    }
  };

  template <class ... AFuncs, class C>
  struct Simplification<Composition<Addition<AFuncs...>,C>>
  {
    typedef Composition<Addition<AFuncs...>,C> in_type;
    static auto Combine(in_type c)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Distributing Composition "
	"into addition\n";
#endif
      return DistributeComposition<Addition>(c);
    }

    typedef decltype(Combine(std::declval<in_type>())) type;
  };
}

#include "unary_minus.hh"
#include "zero.hh"

namespace manifolds {

  template <class F>
  struct Simplification<
    Addition<F, UnaryMinus<F>>,0,
    typename std::enable_if<
      is_stateless<F>::value>::type>
  {
    typedef Zero type;
    static type Combine(Addition<F, UnaryMinus<F>>)
    {
      return zero;
    }
  };

  template <class F>
  struct Simplification<
    Addition<UnaryMinus<F>, F>,0,
    typename std::enable_if<is_stateless<F>::value>::type>
  {
    typedef Zero type;
    static type Combine(Addition<UnaryMinus<F>,F>)
    {
      return zero;
    }
  };
}

#endif
