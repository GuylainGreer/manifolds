#ifndef MANIFOLDS_FUNCTIONS_ADDITION_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_ADDITION_SIMPLIFICATIONS_HH

#include "simplify_variadic.hh"
#include "addition.hh"
#include "simplify.hh"
#include "unary_minus.hh"
#include "zero.hh"

namespace manifolds {

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

  template <class F>
  struct Simplification<
    Addition<F, UnaryMinus<F>>, /*add_nadd*/2,
    typename std::enable_if<
      is_stateless<F>::value>::type>
  {
    typedef Zero type;
    static type Combine(Addition<F, UnaryMinus<F>>)
    {
      SIMPLIFY_INFO("Simplifying addition and subtraction by self");
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
      SIMPLIFY_INFO("Simplifying subtraction and addition by self");
      return zero;
    }
  };
}

#endif
