#ifndef MANIFOLDS_POLYNOMIAL_HH
#define MANIFOLDS_POLYNOMIAL_HH

#include "full_function_defs.hh"
#include "function.hh"
#include <ratio>
#include <array>
#include <type_traits>

namespace manifolds
{

  //Somewhat confusingly,
  //order here refers to the number of coefficients
  template <class OutputUnits,
	    class InputUnits,
	    class Order>
  struct PolynomialImpl : Function
  {
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

}

#include "simplify.hh"
#include "addition.hh"

namespace manifolds {
  template <class InputType, class OutputType,
	    int order1, int order2>
  struct Simplify<Addition<
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

    static type Combine(Polynomial<
			InputType, OutputType,
			std::integral_constant<int,order1>> p1,
			Polynomial<
			InputType, OutputType,
			std::integral_constant<int,order2>> p2)
    {
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

  /*  template <class A>
  struct Simplify<
    Addition<A,A>,
    typename std::enable_if<
      std::is_same<typename Simplify<Addition<A,A>>::type,
		   Addition<A,A>>::value
		   >::type>
  {
    typedef Composition<
      Polynomial<double,double,std::integral_constant<int,2>>,
      A> type;

    static type Combine(Addition<A,A> a)
    {
      return {GetPolynomial<double>(0.0,1.0),
	  A()};
    }

    };*/
}

#endif
