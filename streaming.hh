#ifndef MANIFOLDS_FUNCTION_STREAM_VARIADIC_HH
#define MANIFOLDS_FUNCTION_STREAM_VARIADIC_HH

#include <ostream>
#include <utility>

template <class T>
void StreamVariadicHelper(std::ostream & s, T t)
{
  s << t;
}

template <class T, class ... Ts>
void StreamVariadicHelper(std::ostream & s, T t, Ts ... ts)
{
  s << t << ", ";
  StreamVariadicHelper(s, ts...);
}

template <class Tuple, std::size_t ... indices>
void StreamVariadicHelper(std::ostream & s, Tuple ts,
			  std::integer_sequence<
			  std::size_t,indices...>)
{
  StreamVariadicHelper(s, std::get<indices>(ts)...);
}


template <template<class...>class Variadic,
	  class ... Functions, class String>
void StreamVariadic(String var_name,
		    Variadic<Functions...> v,
		    std::ostream & s)
{
  s << var_name << "{";
  auto t = v.GetFunctions();
  StreamVariadicHelper(s, t,
		       std::make_index_sequence<
		       std::tuple_size<decltype(t)>::value>());
  s << "}";
}

#endif
