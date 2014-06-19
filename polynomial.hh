#ifndef MANIFOLDS_POLYNOMIAL_HH
#define MANIFOLDS_POLYNOMIAL_HH

#include "full_function_defs.hh"
#include "function.hh"
#include <ratio>
#include <array>
#include <type_traits>
#include <ostream>

namespace manifolds
{

  //Somewhat confusingly,
  //order here refers to the number of coefficients
  template <class OutputUnits,
	    class InputUnits,
	    class Order>
  struct PolynomialImpl : Function
  {
    static const bool stateless = false;
    template <class Type, std::size_t N>
    PolynomialImpl(std::array<Type, N> t):
      coeffs(t){}

    template <class T>
    auto operator()(T && t) const
    {
      auto unitless = t/InputUnits(1);
      double result = 0;
      for(auto i = coeffs.rbegin(); i != coeffs.rend(); i++)
	result = result * unitless + *i;
      return OutputUnits(result);
    }

    const auto & GetCoeffs() const
    {
      return coeffs;
    }

  private:

    std::array<double,Order::value> coeffs;
  };

  DEF_FF_TEMPLATE(Polynomial)

  template <class OutputType, class InputType,
	    class ... InputTypes>
  auto GetPolynomial(InputType first, InputTypes ... coeffs)
  {
    typedef typename std::common_type<InputType, double>::type
      real_type;
    std::array<double, sizeof...(InputTypes)+1>
      coeff_arr{{double(first), double(coeffs)...}};
    return Polynomial<
      OutputType,
      real_type,
      std::integral_constant<int,
	     sizeof...(InputTypes)+1>>(coeff_arr);
  }

  template <class A, class B, class N>
  std::ostream & operator<<(std::ostream & s,
			    Polynomial<A,B,N> p)
  {
    s << "Polynomial(";
    for(int i = 0; i < N::value; i++)
      {
	s << p.GetCoeffs()[i];
	if(i != N::value-1)
	  s << ' ';
      }
    return s << ")";
  }
}

#include "simplify.hh"
#include "addition.hh"
#include "multiplication.hh"

namespace manifolds {
  template <class InputType, class OutputType,
	    int order1, int order2>
  struct Simplification<Addition<
    Polynomial<InputType, OutputType,
	       std::integral_constant<int,order1>>,
		    Polynomial<
		      InputType, OutputType,
		      std::integral_constant<int,order2>>>>
  {
    static const int new_order =
      order1 > order2 ? order1 : order2;
    typedef Polynomial<
      InputType, OutputType,
      std::integral_constant<int, new_order>> type;

    template <class T1, class T2>
      static auto Init(const T1 & t1, const T2&,
		std::true_type)
    {
      return t1.GetCoeffs();
    }

    template <class T1, class T2>
      static auto Init(const T1& , const T2& t2,
		std::false_type)
    {
      return t2.GetCoeffs();
    }

    static type Combine(Addition<Polynomial<
			InputType, OutputType,
			std::integral_constant<int,order1>>,
			Polynomial<
			InputType, OutputType,
			std::integral_constant<int,order2>>> p)
    {
      auto p1 = std::get<0>(p.GetFunctions());
      auto p2 = std::get<1>(p.GetFunctions());
      typedef std::integral_constant<
	bool, (order1>order2)> yes_no;
      if(order1 > order2)
	{
	  std::array<double,new_order> r = Init(p1,p2,yes_no());
	  for(unsigned i = 0; i < order2; i++)
	    r[i] += p2.GetCoeffs()[i];
	  return {r};
	}
      else
	{
	  std::array<double,new_order> r = Init(p1,p2,yes_no());
	  for(unsigned i = 0; i < order1; i++)
	    r[i] += p1.GetCoeffs()[i];
	  return {r};
	}
    }
  };

  template <class A>
  struct Simplification<
    Addition<A,A>,
    typename std::enable_if<is_stateless<A>::value>::type>
  {
    typedef Composition<
      Polynomial<double,double,std::integral_constant<int,2>>,
      A> type;

    static type Combine(Addition<A,A> a)
    {
      return {GetPolynomial<double>(0.0,2.0),
	  A()};
    }
  };

  template <class InputType, class OutputType,
	    int order1, int order2>
  struct Simplification<Multiplication<
    Polynomial<InputType, OutputType,
	       std::integral_constant<int,order1>>,
		    Polynomial<
		      InputType, OutputType,
		      std::integral_constant<int,order2>>>>
  {
    static const int new_order = order1 + order2 - 1;
    typedef Polynomial<
      InputType, OutputType,
      std::integral_constant<int, new_order>> type;

    static type Combine(Multiplication<Polynomial<
			InputType, OutputType,
			std::integral_constant<int,order1>>,
			Polynomial<
			InputType, OutputType,
			std::integral_constant<int,order2>>> p)
    {
      auto p1 = std::get<0>(p.GetFunctions());
      auto p2 = std::get<1>(p.GetFunctions());
      std::array<double,new_order> r;
      std::fill(r.begin(), r.end(), 0);
      for(unsigned j = 0; j < order1; j++)
	for(unsigned i = 0; i < order2; i++)
	  r[i+j] += p1.GetCoeffs()[j] * p2.GetCoeffs()[i];
      return {r};
    }
  };

  template <class InputType, class OutputType,
	    int order1, int order2>
  struct Simplification<Composition<
    Polynomial<InputType, OutputType,
	       std::integral_constant<int,order1>>,
		    Polynomial<
		      InputType, OutputType,
		      std::integral_constant<int,order2>>>>
  {
    static const int new_order = (order1-1) * (order2-1) + 1;
    typedef Polynomial<
      InputType, OutputType,
      std::integral_constant<int, new_order>> type;

    template <class T, class U>
      static auto Multiply(const T & t, const U & u)
    {
      return Simplification<Multiplication<T,U>>::
	Combine(Multiplication<T,U>(t,u));
    }

    template <class T, class U>
      static auto Add(const T & t, const U & u)
    {
      return Simplification<Addition<T,U>>::
	Combine(Addition<T,U>(t,u));
    }

    template <class T, class U>
      static auto Accumulate(const T & t, const U & u,
			     std::integral_constant<int,-1>)
    {
      return GetPolynomial<double>(0);
    }

    template <int index, class T, class U>
      static auto Accumulate(const T & t, const U & u,
			     std::integral_constant<int,index>)
    {
      static const int last_coeff =
	std::tuple_size<decltype(t.GetCoeffs())>::value-1;
      auto m1 =
	GetPolynomial<double>
	(std::get<last_coeff-index>(t.GetCoeffs()));
      auto m2 = GetPolynomial<double>(0,1);
      auto m3 = Accumulate(t, u, std::integral_constant<
			   int,index-1>());
      typedef Multiplication<decltype(m2),decltype(m3)> mul_type;
      auto m4 = Simplification<mul_type>::Combine(mul_type(m2,m3));
      typedef Addition<decltype(m1), decltype(m4)> add_type;
      return Simplification
<add_type>::Combine(add_type(m1,m4));
    }

    static type Combine(Multiplication<Polynomial<
			InputType, OutputType,
			std::integral_constant<int,order1>>,
			Polynomial<
			InputType, OutputType,
			std::integral_constant<int,order2>>> p)
    {
      auto p1 = std::get<0>(p.GetFunctions());
      auto p2 = std::get<1>(p.GetFunctions());
      std::array<double,new_order> r;
      std::fill(r.begin(), r.end(), 0);
      return {r};
    }
  };
}

#endif
