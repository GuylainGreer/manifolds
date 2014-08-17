#ifndef MANIFOLDS_FUNCTIONS_STREAMING_HH
#define MANIFOLDS_FUNCTIONS_STREAMING_HH

#include <ostream>
#include "composition.hh"
#include "addition.hh"
#include "multiplication.hh"

namespace manifolds
{
  template <int iter>
  struct Stream2Wrapper
  {
    template <class Func>
    static void Stream2(std::ostream & s, Composition<Func> c)
    {
      Stream2Wrapper<iter+1>::
	Stream2(s, std::get<0>(c.GetFunctions()));
    }

    template <class F, class ... Funcs,
	      class = typename std::enable_if<
		(sizeof...(Funcs)>0)>::type>
    static void Stream2(std::ostream & s,
			Composition<F, Funcs...> c)
    {
      Stream2Wrapper<iter+1>::Stream2
	(s, std::get<0>(c.GetFunctions()));
      s << '(';
      Stream2Wrapper<iter+1>::Stream2
	(s, Composition<Funcs...>
	 (remove_element<0>(c.GetFunctions())));
      s << ')';
    }

    template <class F>
    static void Stream2(std::ostream & s, F f)
    {
      s << f;
    }

    template <class Func>
    static void Stream2(std::ostream & s,
			Addition<Func> a)
    {
      Stream2Wrapper<iter+1>::Stream2
	(s, std::get<0>(a.GetFunctions()));
    }

    template <class F, class ... Funcs,
	      class = typename std::enable_if<
		(sizeof...(Funcs)>0)>::type>
    static void Stream2(std::ostream & s,
			Addition<F,Funcs...> a)
    {
      Stream2Wrapper<iter+1>::Stream2
	(s, std::get<0>(a.GetFunctions()));
      s << " + ";
      Stream2Wrapper<iter+1>::Stream2
	(s, Addition<Funcs...>
	 (remove_element<0>(a.GetFunctions())));
    }

    template <class T>
    static void PrintMultElem(std::ostream & s, T t)
    {
      static const bool b =
	IsVariadic<
	  Addition, T
	>::value;
      if(b) s << '(';
      Stream2Wrapper<iter+1>::Stream2(s, t);
      if(b) s << ')';
    }

    template <class F>
    static void Stream2(std::ostream & s,
			Multiplication<F> m)
    {
      PrintMultElem(s, std::get<0>(m.GetFunctions()));
    }

    template <class F, class ... Funcs>
    static void Stream2(std::ostream & s,
			Multiplication<F,Funcs...> m)
    {
      PrintMultElem(s, std::get<0>(m.GetFunctions()));
      s << " * ";
      Stream2Wrapper<iter+1>::Stream2
	(s, Multiplication<Funcs...>
	 (remove_element<0>(m.GetFunctions())));
    }
  };

  template <class T>
  std::ostream & Stream2(std::ostream & s, T t)
  {
    Stream2Wrapper<0>::Stream2(s,t);
    return s;
  }
}

#endif
