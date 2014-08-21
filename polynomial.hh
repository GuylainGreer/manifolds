#ifndef MANIFOLDS_POLYNOMIAL_HH
#define MANIFOLDS_POLYNOMIAL_HH

#include "full_function_defs.hh"
#include "function.hh"
#include <array>
#include <type_traits>
#include <ostream>
#include <iostream>
#include <algorithm>

namespace manifolds
{

  //Somewhat confusingly,
  //order here refers to the number of coefficients
  template <class CoeffType, class Order>
  struct PolynomialImpl :
    Function<int_<6>, 1, 1>
  {
    static const int num_coeffs = Order::value;
    static const bool stateless = false;
    template <class Type, std::size_t N>
    PolynomialImpl(std::array<Type, N> t):
      coeffs(t){}

    template <class Type, class N>
    PolynomialImpl(const PolynomialImpl<Type, N> & t)
      {
	std::copy(t.GetCoeffs().begin(),
		  t.GetCoeffs().end(),
		  coeffs.begin());
      }

    template <class T, class ... Ts>
    auto operator()(T t, Ts ...) const
    {
      static_assert(sizeof...(Ts) == 0 ||
		    Order::value == 1,
		    "Can only call polynomial "
		    "with multiple arguments when "
		    "polynomial is a constant");
      typename std::common_type<
	T,CoeffType>::type result = 0, t2 = t;
      for(auto i = coeffs.rbegin(); i != coeffs.rend(); i++)
	{
	  result = result * t2 + *i;
	}
      return result;
    }

    const auto & GetCoeffs() const
    {
      return coeffs;
    }

    void Print(std::ostream & s) const
    {
      s << "Polynomial(";
      for(int i = 0; i < Order::value; i++)
	{
	  s << coeffs[i];
	  if(i != Order::value-1)
	    s << ' ';
	}
      s << ")";
    }
  private:

    std::array<CoeffType,Order::value> coeffs;
  };

  DEF_FF_TEMPLATE(Polynomial)

  template <class>
  struct is_polynomial : std::false_type{};

  template <class C, class O>
  struct is_polynomial<Polynomial<C, O>> : std::true_type{};

    template <class>
    struct PolynomialOrder : int_<-1>{};

    template <class C, class O>
    struct PolynomialOrder<Polynomial<C, O>> : O {};

  template <class CType, int N>
  std::ostream & operator<<(std::ostream & s,
			    Polynomial<CType,int_<N>> p)
  {
    p.Print(s);
    return s;
  }

  inline Polynomial<long double,int_<1>>
    operator""_c(long double x)
  {
    std::array<long double, 1> a{{x}};
    return {a};
  }

  inline Polynomial<long long unsigned,int_<1>>
    operator""_c(long long unsigned x)
  {
    std::array<long long unsigned, 1> a{{x}};
    return {a};
  }

  template <class ... InputTypes>
  auto GetPolynomial(InputTypes ... coeffs)
  {
    typedef typename std::common_type<
      InputTypes...>::type coeff_type;
    std::array<coeff_type, sizeof...(InputTypes)>
      coeff_arr{{coeff_type(coeffs)...}};
    return Polynomial<
      coeff_type,
      int_<sizeof...(InputTypes)>>(coeff_arr);
  }

}

#include "simplify.hh"
#include "addition.hh"
#include "multiplication.hh"
#include "zero.hh"
#include "unary_minus.hh"

namespace manifolds {
  template <int order1, int order2,
	    class CoeffType1, class CoeffType2>
  struct Simplification<Addition<
    Polynomial<CoeffType1, int_<order1>>,
    Polynomial<CoeffType2, int_<order2>>>,0>
  {
    static const int new_order =
      order1 > order2 ? order1 : order2;
    typedef typename std::common_type<
      CoeffType1, CoeffType2>::type coeff_type;
    typedef Polynomial<
      coeff_type,
      int_< new_order>> type;

    template <class T1, class T2, bool b>
      static auto Init(const T1 & t1, const T2 & t2,
		       std::true_type)
    {
      typedef typename std::common_type<
	decltype(t1[0]), decltype(t2[0])>::type type;
      std::array<type, array_size<
	decltype(t1.GetCoeffs())>::value> result;
      std::copy(t1.GetCoeffs().begin(),
		t1.GetCoeffs().end(), result.begin());
      return result;
    }

    template <class T1, class T2>
      static auto Init(const T1 & t1, const T2& t2,
		       std::false_type)
    {
      typedef typename std::common_type<
	decltype(t1.GetCoeffs()[0]),
	decltype(t2.GetCoeffs()[0])>::type type;
      std::array<type, array_size<
	decltype(t2.GetCoeffs())>::value> result;
      std::copy(t2.GetCoeffs().begin(),
		t2.GetCoeffs().end(), result.begin());
      return result;
    }

    static type Combine(Addition<Polynomial<
			CoeffType1,
			int_<order1>>,
			Polynomial<
			CoeffType2,
			int_<order2>>> p)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying addition of two polynomials\n";
#endif
      auto p1 = get<0>(p.GetFunctions());
      auto p2 = get<1>(p.GetFunctions());
      typedef std::integral_constant<
	bool, (order1>order2)> yes_no;
      if(order1 > order2)
	{
	  std::array<coeff_type,new_order> r =
	    Init(p1,p2,yes_no());
	  for(unsigned i = 0; i < order2; i++)
	    r[i] += p2.GetCoeffs()[i];
	  return {r};
	}
      else
	{
	  std::array<coeff_type,new_order> r =
	    Init(p1,p2,yes_no());
	  for(unsigned i = 0; i < order1; i++)
	    r[i] += p1.GetCoeffs()[i];
	  return {r};
	}
    }
  };

  template <int order1, int order2,
	    class CoeffType1, class CoeffType2>
  struct Simplification<Multiplication<
    Polynomial<CoeffType1,
	       int_<order1>>,
    Polynomial<CoeffType2,
	       int_<order2>>>,0>
  {
    static const int new_order = order1 + order2 - 1;
    typedef typename std::common_type<
      CoeffType1, CoeffType2>::type coeff_type;
    typedef Polynomial<
      coeff_type,
      int_< new_order>> type;

    static type Combine(Multiplication<Polynomial<
			CoeffType1,
			int_<order1>>,
			Polynomial<
			CoeffType2,
			int_<order2>>> p)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying multiplication "
	"of two polynomials\n";
#endif
      auto p1 = get<0>(p.GetFunctions());
      auto p2 = get<1>(p.GetFunctions());
      std::array<coeff_type,new_order> r;
      std::fill(r.begin(), r.end(), 0);
      for(unsigned j = 0; j < order1; j++)
	for(unsigned i = 0; i < order2; i++)
	  r[i+j] += p1.GetCoeffs()[j] * p2.GetCoeffs()[i];
      return {r};
    }
  };

  template <int order1, int order2,
	    class CoeffType1, class CoeffType2>
  struct Simplification<
    Composition<
    Polynomial<CoeffType1,int_<order1>>,
      Polynomial<CoeffType2,int_<order2>>>,0>
  {
    static const int new_order = (order1-1) * (order2-1) + 1;
    typedef typename std::common_type<
      CoeffType1, CoeffType2>::type coeff_type;
    typedef Polynomial<
      coeff_type,
      int_< new_order>> type;

    template <class T, class U>
      static auto Multiply(const T & t, const U & u)
    {
      return Simplify(Multiplication<T,U>(t,u));
    }

    template <class T, class U>
      static auto Add(const T & t, const U & u)
    {
      return Simplify(Addition<T,U>(t,u));
    }

    template <class T, class U>
      static auto Accumulate(const T & t, const U & u,
			     int_<-1>)
    {
      return zero;
    }

    template <int index, class T, class U>
      static auto Accumulate(const T & t, const U & u,
			     int_<index> i)
    {
      auto t_coeffs = t.GetCoeffs();
      static const int last_coeff =
	std::tuple_size<decltype(t_coeffs)>::value-1;
      auto m1 =
	GetPolynomial(get<last_coeff-index>(t_coeffs));
      auto m2 = u;
      auto m3 = Accumulate(t, u, int_<index-1>());
      auto m4 = Multiply(m2,m3);
      return Add(m1,m4);
    }

    static auto Combine(Composition<Polynomial<
			CoeffType1,
			int_<order1>>,
			Polynomial<
			CoeffType2,
			int_<order2>>> p)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying composition of two polynomials\n";
#endif
      auto p1 = get<0>(p.GetFunctions());
      auto p2 = get<1>(p.GetFunctions());
      return
	Accumulate(p1, p2, int_<order1-1>());
    }
  };

  template <class Order, class CoeffType>
  struct Simplification<
    UnaryMinus<Polynomial<CoeffType, Order>>,0>
  {
    typedef typename std::conditional<
      std::is_signed<CoeffType>::value,
      CoeffType,
      typename std::make_signed<
	CoeffType>::type
      >::type CoeffType2;
    typedef Polynomial<CoeffType2, Order> type;

    static type Combine(UnaryMinus<Polynomial<
			CoeffType, Order>> u)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying negative of polynomial\n";
#endif
      std::array<CoeffType2, Order::value> a;
      for(int i = 0; i < Order::value; i++)
	a[i] = -CoeffType2(u.GetFunction().GetCoeffs()[i]);
      return {a};
    }
  };

  template <class Order, class CoeffType1,
	    class CoeffType2, class ... Funcs>
  struct Simplification<
    Multiplication<Composition<Polynomial<CoeffType1, Order>,
			       Funcs...>,
		   Polynomial<CoeffType2, int_<1>>>,0>
  {
    typedef typename std::common_type<
      CoeffType1, CoeffType2>::type CoeffType;
    typedef Composition<Polynomial<CoeffType, Order>,
			Funcs...> type;

    static type Combine(Multiplication<
			Composition<Polynomial<CoeffType1, Order>,
			Funcs...>,
			Polynomial<CoeffType2, int_<1>>> m)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying multiplication of composed "
	"polynomial by a constant\n";
#endif
      std::array<CoeffType, Order::value> cs;
      auto p =
	get<0>(get<0>(m.GetFunctions()).
		    GetFunctions()).GetCoeffs();
      CoeffType2 c =
	get<1>(m.GetFunctions()).GetCoeffs()[0];
      std::transform(p.begin(), p.end(), cs.begin(),
		     [&c](auto x){return x * c;});
      return 
	{insert_element<0>
	(remove_element<0>
	 (get<0>(m.GetFunctions()).GetFunctions()),
	 Polynomial<CoeffType, Order>(cs))};
    }
  };
}

#include "polynomial_composition_simplifications.hh"

#endif
