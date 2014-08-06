#ifndef MANIFOLDS_FUNCTIONS_SIMPLIFY_HH
#define MANIFOLDS_FUNCTIONS_SIMPLIFY_HH

#include "tuple_util.hh"
#include <boost/type_traits/is_same.hpp>
#include <iostream>

namespace manifolds {
  //Default, don't do anything
  template <class A, class ForEnableIf = void>
  struct Simplification
  {
    typedef A type;

    static type Combine(A a)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "No simplification found\n";
#endif
      return a;
    }
  };

  template <template<class...>class Variadic, class...>
  struct IsVariadic : std::false_type{};

  template <template<class...>class V, class ... Fs,
	    class ... Fs2>
  struct IsVariadic<V, V<Fs...>,Fs2...> : std::true_type{};

  template <template <class...> class Variadic,
	    class ... Args1, class ... Args2>
  struct Simplification<
    Variadic<Variadic<Args1...>, Args2...>,
    typename std::enable_if<
      !IsVariadic<Variadic,Args2...>::value>::type>
  {
    typedef Variadic<Args1..., Args2...> type;

    static type Combine(Variadic<Variadic<Args1...>, Args2...> v)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Flattening variadics\n";
#endif
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
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying more variadics\n";
#endif
      auto t1 =
	std::get<0>(input.GetFunctions()).GetFunctions();
      auto t2 =
	std::get<1>(input.GetFunctions()).GetFunctions();
      return type(std::tuple_cat(t1, t2));
    }
  };

  template <class F>
  auto Simplify(F f, F)
  {
#ifdef PRINT_SIMPLIFIES
    std::cout << "Done: " << f << std::endl;
#endif
    return f;
  }

  template <class F, class G>
  auto Simplify(F f, G)
  {
#ifdef PRINT_SIMPLIFIES
    std::cout << "Simplifying: " << f << std::endl;
#endif
    return Simplify(Simplification<F>::Combine(f), f);
  }

  template <class F>
  auto Simplify(F f)
  {
#ifdef PRINT_SIMPLIFIES
    std::cout << "Simplifying: " << f << std::endl;
#endif
    return Simplify(Simplification<F>::Combine(f), f);
  }
}

#endif
