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
    Addition<Functions...>, /*var_add*/4>
  {
    typedef decltype(SimplifyV<Addition>
		     (std::declval<tuple<Functions...>>(),
		      std::true_type{})) type;

    static type Combine(Addition<Functions...> a)
    {
      SIMPLIFY_INFO("Simplifying Addition\n");
      return SimplifyV<Addition>(a.GetFunctions(),
				 std::true_type{});
    }
  };

  template <class ... AFuncs, class C>
  struct Simplification<Composition<Addition<AFuncs...>,C>,
			/*com_add*/0>
  {
    typedef Composition<Addition<AFuncs...>,C> in_type;
    static auto Combine(in_type c)
    {
      SIMPLIFY_INFO("Distributing Composition "
		    "into addition\n");
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
    Addition<F, UnaryMinus<F>>, /*add_nadd*/2,
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
    Addition<UnaryMinus<F>, F>, /*nadd_add*/1,
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
