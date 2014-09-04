#ifndef MANIFOLDS_FUNCTIONS_SIMPLIFY_HH
#define MANIFOLDS_FUNCTIONS_SIMPLIFY_HH

#include "tuple_util.hh"
#include <boost/type_traits/is_same.hpp>
#include <iostream>

namespace manifolds {

  static const int num_simplification_levels = 5;

#ifdef PRINT_SIMPLIFIES
#  define SIMPLIFY_INFO(msg) \
  std::cout << __FILE__ << ":" << __LINE__ << " " << msg
#else
#  define SIMPLIFY_INFO(msg)
#endif

  template <class A,
	    int level,
	    class ForEnableIf = void>
  struct Simplification
  {
    typedef A type;
    static type Combine(A a)
    {
      SIMPLIFY_INFO("No simplification found\n");
      return a;
    }
  };

  template <class>
  struct SimplificationLevel;

  template <class A, int level, class B>
  struct SimplificationLevel<Simplification<A,level,B>>:
    int_<level>{};

  template <template<class...>class Variadic, class...>
  struct IsVariadic : std::false_type{};

  template <template<class...>class V, class ... Fs,
	    class ... Fs2>
  struct IsVariadic<V, V<Fs...>,Fs2...> : std::true_type{};

  template <int iter>
  struct SimplificationWrapper
  {
    template <class F, int level>
    static auto Simplify(F f, F, int_<level>)
    {
      SIMPLIFY_INFO("Done level " << level << ", "
		    << "moving to level " << level+1 << '\n');
      return SimplificationWrapper<iter+1>::
	Simplify(f, int_<level+1>());
    }

    template <class F, class G, int level>
    static auto Simplify(F f, G, int_<level>)
    {
      return SimplificationWrapper<iter+1>::
	Simplify(f, int_<0>());
    }

    template <class F, int level>
    static auto Simplify(F f, int_<level>)
    {
      SIMPLIFY_INFO("Simplifying: " << f << '\n');
      return Simplify(Simplification<F,level>::Combine(f), f,
		      int_<level>());
    }

    template <class F>
    static auto Simplify(F f, int_<num_simplification_levels>)
    {
      SIMPLIFY_INFO("Done: " << f << "\n\n");
      return f;
    }
  };

  template <class F>
  auto Simplify(F f)
  {
    SIMPLIFY_INFO("Simplifying: " << f << "\n");
    return SimplificationWrapper<0>::
      Simplify(f, int_<0>());
  }

  template <class T>
  using SimplifiedType =
    decltype(Simplify(std::declval<T>()));

  template <class T>
  using Simplifies = bool_<
    !std::is_same<SimplifiedType<T>, T>::value>;
}

#endif
