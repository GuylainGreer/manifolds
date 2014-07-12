#ifndef MANIFOLDS_FUNCTIONS_SIMPLIFY_HH
#define MANIFOLDS_FUNCTIONS_SIMPLIFY_HH

#include "tuple_util.hh"
#include <boost/type_traits/is_same.hpp>

namespace manifolds {
  //Default, don't do anything
  template <class A, class ForEnableIf = void>
  struct Simplification
  {
    typedef A type;

    static type Combine(A a)
    {
      return a;
    }
  };

  template <template <class...> class Variadic,
	    class ... Args1, class ... Args2>
  struct Simplification<Variadic<Variadic<Args1...>, Args2...> >
  {
    typedef Variadic<Args1..., Args2...> type;

    static type Combine(Variadic<Variadic<Args1...>, Args2...> v)
    {
      auto t =
	std::tuple_cat(std::get<0>(v.GetFunctions()).GetFunctions(),
		       remove_element<0>(v.GetFunctions()));
      return {t};
    }
  };

  template <template <class...> class Variadic,
	    class ... Args1, class ... Args2>
  struct Simplification<Variadic<Variadic<Args1...>,
			   Variadic<Args2...>>>
  {
    typedef Variadic<Args1..., Args2...> type;

    static type Combine(Variadic<Variadic<Args1...>,
			Variadic<Args2...>> input)
    {
      return type(std::tuple_cat(std::get<0>(input),
				 std::get<1>(input)));
    }
  };

  template <class F>
  auto Simplify(F f, F)
  {
    return f;
  }

  template <class F, class G>
  auto Simplify(F f, G)
  {
    return Simplify(Simplification<F>::Combine(f), f);
  }

  template <class F>
  auto Simplify(F f)
  {
    return Simplify(Simplification<F>::Combine(f), f);
  }
}

#endif
