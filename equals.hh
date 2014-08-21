#ifndef MANIFOLDS_FUNCTIONS_VARIADIC_EQUALS_HH
#define MANIFOLDS_FUNCTIONS_VARIADIC_EQUALS_HH

namespace manifolds {

template <class Func1, class Func2>
bool VariadicEqual(Func1, Func2)
{
  return false;
}

inline bool and_helper(bool b)
{
  return b;
}

template <class...Bools>
bool and_helper(bool b, Bools...bs)
{
  return b && and_helper(bs...);
}

template <template<class...>class Variadic,
	  class ... Functions,
	  std::size_t ... indices>
bool VariadicEqualHelper(Variadic<Functions...> f1,
			 Variadic<Functions...> f2,
			 std::integer_sequence<
			 std::size_t,indices...>)
{
  return and_helper((get<indices>(f1.GetFunctions()) ==
		     get<indices>(f2.GetFunctions()))...);
}

template <template<class...>class Variadic,
	  class ... Functions>
bool VariadicEqual(Variadic<Functions...> f1,\
		   Variadic<Functions...> f2)
{
  return VariadicEqualHelper
    (f1, f2, std::index_sequence_for<Functions...>());
}

}
#endif
