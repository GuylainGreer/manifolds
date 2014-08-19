#ifndef MANIFOLDS_FUNCTIONS_STREAMING_HH
#define MANIFOLDS_FUNCTIONS_STREAMING_HH

#include <ostream>
#include "composition.hh"
#include "addition.hh"
#include "multiplication.hh"
#include "polynomial.hh"

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

    template <class CType, class Order,
	      class ... Funcs,
	      class = typename std::enable_if<
		(sizeof...(Funcs)>0)>::type>
    static void Stream2(std::ostream & s,
			Composition<
			Polynomial<CType, Order>,
			Funcs...> c)
    {
      std::stringstream ss;
      Stream2Wrapper<iter+1>::Stream2
	(ss, Composition<Funcs...>(remove_element<0>
				   (c.GetFunctions())));
      std::string var = ss.str();
      for(auto c : var)
	{
	  if(!std::isalnum(c))
	    {
	      var = "(" + var + ")";
	      break;
	    }
	}
      Stream2Wrapper<iter+1>::Stream2
	(s, std::get<0>(c.GetFunctions()), var);
    }

    template <class F, class ... Funcs,
	      class = typename std::enable_if<
		(sizeof...(Funcs)>0) &&
      !is_polynomial<F>::value>::type>
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

    template <class CoeffType, class Order>
    static void Stream2(std::ostream & s,
			Polynomial<CoeffType, Order> p,
			std::string var = "")
    {
      if(var == "")
	var = "x";
      auto coeffs = p.GetCoeffs();
      bool is_first = coeffs[0] == CoeffType(0);
      typedef typename std::common_type<
	CoeffType, long double>::type abs_type;
      if(!is_first)
	s << coeffs[0];
      for(unsigned i = 1; i < coeffs.size(); i++)
	{
	  if(coeffs[i] == CoeffType(0))
	    continue;
	  if(is_first)
	    {
	      if(coeffs[i] == CoeffType(1))
		;
	      else if(coeffs[i] == CoeffType(-1))
		s << '-';
	      else
		s << coeffs[i] << " * ";
	      is_first = false;
	    }
	  else if(coeffs[i] > 0)
	    {
	      s << " + ";
	      if(coeffs[i] != 1)
		s << coeffs[i] << " * ";
	    }
	  else
	    {
	      s << " - ";
	      if(coeffs[i] != -1)
		s << -coeffs[i] << " * ";
	    }
	  s << var;
	  if(i != 1)
	    s << "**" << i;
	}
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
