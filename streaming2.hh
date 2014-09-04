#ifndef MANIFOLDS_FUNCTIONS_STREAMING_HH
#define MANIFOLDS_FUNCTIONS_STREAMING_HH

#include <ostream>
#include <map>
#include <algorithm>
#include <iterator>
#include "composition.hh"
#include "addition.hh"
#include "multiplication.hh"
#include "polynomial.hh"
#include "variables.hh"
#include "function_matrix.hh"
#include "division.hh"

namespace manifolds
{
  template <int iter>
  struct Stream2Wrapper
  {
    template <class N, class D, class VNamer>
    static void Stream2(std::ostream & s, Division<N,D> d,
			VNamer v)
    {
      s << '(';
      Stream2Wrapper<iter+1>::Stream2(s, d.GetNumerator(), v);
      s << " / ";
      Stream2Wrapper<iter+1>::Stream2(s, d.GetDenominator(), v);
      s << ')';
    }

    template <class VNamer>
    static void Stream2(std::ostream & s, Zero, VNamer)
    {
      s << "0";
    }

    template <class VNamer>
    static void Stream2Tuple(std::ostream & s,
			     tuple<>, VNamer)
    {
    }

    template <class Tuple, class VNamer,
	      class = typename std::enable_if<
		tuple_size<Tuple>::value != 0>::type>
    static void Stream2Tuple(std::ostream & s,
			     const Tuple & t,
			     VNamer vn)
    {
      Stream2Wrapper<iter+1>::Stream2(s, get<0>(t), vn);
      if(tuple_size<Tuple>::value > 1)
	s << ", ";
      Stream2Tuple(s, remove_element<0>(t), vn);
    }

    template <class c, class ... Functions,
	      class VNamer>
    static void Stream2FMatrix(std::ostream & s,
			       FunctionMatrix<
			       int_<0>,c,Functions...> fm,
			       VNamer vn)
    {
    }

    template <class r, class c, class ... Functions,
	      class VNamer,
	      class = typename std::enable_if<
		r::value != 0>::type>
    static void Stream2FMatrix(std::ostream & s,
			       FunctionMatrix<
			       r,c,Functions...> fm,
			       VNamer vn)
    {
      s << "{";
      Stream2Tuple(s, subset<0, c::value>
		   (fm.GetFunctions()), vn);
      s << "}";
      if(r::value > 1)
	s << ", ";
      auto t = GetFunctionMatrix<
	r::value-1, c::value>
	(subset<r::value, c::value*(r::value-1)>
	 (fm.GetFunctions()));
      Stream2FMatrix(s, t, vn);
    }

    template <class ... Functions, class VNamer>
    static void Stream2(std::ostream & s,
			FunctionMatrix<Functions...> fm,
			VNamer vn)
    {
      s << "{";
      Stream2FMatrix(s, fm, vn);
      s << "}";
    }

    template <int N, bool abelian, class VNamer>
    static void Stream2(std::ostream & s,
			Variable<N,abelian> v,
			VNamer vn)
    {
      s << vn(v);
    }

    template <class Func, class VNamer>
    static void Stream2(std::ostream & s,
			Composition<Func> c,
			VNamer vn)
    {
      Stream2Wrapper<iter+1>::
	Stream2(s, get<0>(c.GetFunctions()), vn);
    }

    template <class CType, class Order,
	      class ... Funcs, class VNamer,
	      class = typename std::enable_if<
		(sizeof...(Funcs)>0)>::type>
    static void Stream2(std::ostream & s,
			Composition<
			Polynomial<CType, Order>,
			Funcs...> c,
			VNamer vn)
    {
      std::stringstream ss;
      Stream2Wrapper<iter+1>::Stream2
	(ss, Composition<Funcs...>(remove_element<0>
				   (c.GetFunctions())), vn);
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
	(s, get<0>(c.GetFunctions()), vn, var);
    }

    template <class F, class ... Funcs,
	      class VNamer,
	      class = typename std::enable_if<
		(sizeof...(Funcs)>0) &&
      !is_polynomial<F>::value>::type>
      static void Stream2(std::ostream & s,
			  Composition<F, Funcs...> c,
			  VNamer vn)
    {
      Stream2Wrapper<iter+1>::Stream2
	(s, get<0>(c.GetFunctions()), vn);
      s << '(';
      Stream2Wrapper<iter+1>::Stream2
	(s, Composition<Funcs...>
	 (remove_element<0>(c.GetFunctions())), vn);
      s << ')';
    }

    template <class T, class VNamer>
    static void Stream2(std::ostream & s,
			UnaryMinus<T> u,
			VNamer vn)
    {
      s << "-(";
      Stream2Wrapper<iter+1>::Stream2
	(s, u.GetFunction(), vn);
      s << ")";
    }

    template <class CoeffType, class Order, class VNamer>
    static void Stream2(std::ostream & s,
			Polynomial<CoeffType, Order> p,
			VNamer vn,
			std::string var = "")
    {
      if(var == "")
	var = "x";
      auto coeffs = p.GetCoeffs();
      bool is_first = coeffs[0] == CoeffType(0);
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
	      if((double)coeffs[i] != -1)
		s << -coeffs[i] << " * ";
	    }
	  s << var;
	  if(i != 1)
	    s << "**" << i;
	}
    }

    template <class F, class VNamer>
    static void Stream2(std::ostream & s, F f, VNamer vn)
    {
      s << f;
    }

    template <class Func, class VNamer>
    static void Stream2(std::ostream & s,
			Addition<Func> a,
			VNamer vn)
    {
      Stream2Wrapper<iter+1>::Stream2
	(s, get<0>(a.GetFunctions()), vn);
    }

    template <class F, class ... Funcs,
	      class VNamer,
	      class = typename std::enable_if<
		(sizeof...(Funcs)>0)>::type>
    static void Stream2(std::ostream & s,
			Addition<F,Funcs...> a,
			VNamer vn)
    {
      Stream2Wrapper<iter+1>::Stream2
	(s, get<0>(a.GetFunctions()), vn);
      s << " + ";
      Stream2Wrapper<iter+1>::Stream2
	(s, Addition<Funcs...>
	 (remove_element<0>(a.GetFunctions())), vn);
    }

    template <class T, class VNamer>
    static void PrintMultElem(std::ostream & s, T t,
			      VNamer vn)
    {
      static const bool b =
	IsVariadic<
        Addition, T
	>::value ||
        (is_polynomial<T>::value &&
         PolynomialOrder<T>::value != 1);
      if(b) s << '(';
      Stream2Wrapper<iter+1>::Stream2(s, t, vn);
      if(b) s << ')';
    }

    template <class F, class VNamer>
    static void Stream2(std::ostream & s,
			Multiplication<F> m,
			VNamer vn)
    {
      PrintMultElem(s, get<0>(m.GetFunctions()), vn);
    }

    template <class F, class ... Funcs,
	      class VNamer,
	      class = typename std::enable_if<
		(sizeof...(Funcs)>0)>::type>
    static void Stream2(std::ostream & s,
			Multiplication<F,Funcs...> m,
			VNamer vn)
    {
      PrintMultElem(s, get<0>(m.GetFunctions()), vn);
      s << " * ";
      Stream2Wrapper<iter+1>::Stream2
	(s, Multiplication<Funcs...>
	 (remove_element<0>(m.GetFunctions())), vn);
    }
  };

  template <class T, class VariableNamer>
  std::ostream & Stream2(std::ostream & s, T t,
			 VariableNamer vn)
  {
    Stream2Wrapper<0>::Stream2(s, t, std::ref(vn));
    return s;
  }

  template <class T>
  std::ostream & Stream2(std::ostream & s, T t)
  {
    Stream2Wrapper<0>::Stream2(s, t, DefaultVariableNamer());
    return s;
  }

  struct CustomVariableNamer
  {
    std::map<std::pair<int,bool>,std::string> names;
    CustomVariableNamer(std::initializer_list<
			tuple<int,bool,std::string>> l)
    {
      std::transform(std::begin(l), std::end(l),
		     std::inserter(names, names.begin()),
		     [](auto x){
		       return std::make_pair
			 (std::make_pair(get<0>(x), get<1>(x)),
			  get<2>(x));
		     });
    }

    CustomVariableNamer(std::initializer_list<
			std::pair<int,std::string>> l)
    {
      for(auto i : l)
	{
	  names[std::make_pair(i.first, false)] =
	    names[std::make_pair(i.first, true)] = i.second;
	}
    }

    template <int N, bool a>
    std::string operator()(Variable<N,a> v) const
    {
      auto i = names.find(std::make_pair(N,a));
      if(i == names.end())
	return DefaultVariableNamer()(v);
      return i->second;
    }
  };
}

#endif
