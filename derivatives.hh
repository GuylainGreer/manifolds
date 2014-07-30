#ifndef MANIFOLDS_FUNCTIONS_DERIVATIVES_HH
#define MANIFOLDS_FUNCTIONS_DERIVATIVES_HH

#include "addition.hh"
#include "multiplication.hh"
#include "variables.hh"
#include "division.hh"
#include "polynomial.hh"
#include "trig.hh"
#include "operators.hh"

namespace manifolds {
  template <class ... Funcs, int i,
	    std::size_t ... indices>
  auto Derivative(const Addition<Funcs...> & a,
		  Variable<i> v,
		  std::integer_sequence<std::size_t, indices...>)
  {
    return 
      Add(Derivative(
         std::get<indices>(a.GetFunctions())..., v));
  }

  template <class ... Funcs, int i>
  auto Derivative(const Addition<Funcs...> & a,
		  Variable<i> v)
  {
    return Derivative(a, v, std::index_sequence_for<Funcs...>());
  }

  template <class F, int i>
  auto Derivative(UnaryMinus<F> f, Variable<i> v)
  {
    return -Derivative(f.GetFunction(), v);
  }

  template <class Func, int i>
  auto Derivative(const Composition<Func> & c,
		  Variable<i> v)
  {
    return Derivative(std::get<0>(c.GetFunctions()), v);
  }


  template <class Func, class ... Funcs, int i,
	    class = typename std::enable_if<
	      (sizeof...(Funcs)>0)>::type>
  auto Derivative(const Composition<Func,Funcs...> & c,
		  Variable<i> v)
  {
    auto funcs = remove_element<0>(c.GetFunctions());
    auto d = Derivative(std::get<0>(c.GetFunctions()), v);
    auto next =
      std::tuple_cat(std::make_tuple(d), funcs);
    return Composition<decltype(d), Funcs...>(next) *
      Derivative(Composition<Funcs...>(funcs), v);
  }

  template <class F1, class F2, int i>
  auto Derivative(const Division<F1,F2>& d,
		  Variable<i> v)
  {
    auto n = d.GetNumerator();
    auto de = d.GetDenominator();
    return (Derivative(n, v) * de - n * Derivative(de, v)) /
      (de * de);
  }

  template <int i, class ... Funcs, int d_i,
	    std::size_t ... lefts, std::size_t ... rights>
  auto TimesDerivHelper(const Multiplication<Funcs...> & a,
			Variable<d_i> v,
			std::integer_sequence<
			std::size_t, lefts...>,
			std::integer_sequence<
			std::size_t, rights...>)
  {
    auto t = a.GetFunctions();
    return Multiply(std::get<lefts>(t)...,
		    Derivative(std::get<i>(t),v),
		    std::get<rights+i+1>(t)...);
  }

  template <int i, class ... Funcs, int d_i>
  auto TimesDerivHelper(const Multiplication<Funcs...> & a,
			Variable<d_i> v)
  {
    return TimesDerivHelper<i>
      (a, v, std::make_index_sequence<i>(),
       std::make_index_sequence<
       sizeof...(Funcs)-i-1>());
  }

  template <class ... Funcs, int i,
	    std::size_t ... indices>
  auto Derivative(const Multiplication<Funcs...> & a,
		  Variable<i> v,
		  std::integer_sequence<std::size_t, indices...>)
  {
    return 
      Add(TimesDerivHelper<indices>(a, v)...);
  }

  template <class ... Funcs, int i>
  auto Derivative(const Multiplication<Funcs...> & a,
		  Variable<i> v)
  {
    return Derivative(a, v, std::index_sequence_for<Funcs...>());
  }

  template <class CoeffType, class Order, int i>
  auto Derivative(const Polynomial<CoeffType, Order> & p,
		  Variable<i>)
  {
    return Zero();
  }

  template <class CoeffType, int i>
  auto Derivative(Polynomial<CoeffType, int_<1>> p,
		  Variable<i>)
  {
    return zero;
  }

  template <class CoeffType, class Order,
	    class = typename std::enable_if<
	      Order::value != 1
	      >::type>
  auto Derivative(const Polynomial<CoeffType, Order> & p,
		  Variable<0>)
  {
    std::array<CoeffType, Order::value-1> d;
    for(unsigned i = 0; i < Order::value-1; i++)
      d[i] = (i+1)*p.GetCoeffs()[i+1];
    return Polynomial<CoeffType,int_<Order::value-1>>{d};
  }

#define TRIG_DERIV(func, deriv)				\
  auto Derivative(func,Variable<0>){return deriv;}	\
  template <int i>				\
  auto Derivative(func,Variable<i>){return Zero();}

  TRIG_DERIV(Sin, Cos())
  TRIG_DERIV(Cos, -Sin())
  TRIG_DERIV(Tan, 1_c / (Cos()*Cos()))
  TRIG_DERIV(Log, 1_c / x)
  TRIG_DERIV(Sinh, Cosh())
  TRIG_DERIV(Cosh, Sinh())
  TRIG_DERIV(Tanh, 1_c / (Cosh()*Cosh()))
  TRIG_DERIV(ASin, 1_c / Sqrt()(1_c-x*x))
  TRIG_DERIV(ACos, -1_c / Sqrt()(1_c-x*x))
  TRIG_DERIV(ATan, 1_c / (1_c+x*x))
  TRIG_DERIV(ASinh, 1_c / Sqrt()(1_c+x*x))
  TRIG_DERIV(ACosh, 1_c / Sqrt()(-1_c+x*x))
  TRIG_DERIV(ATanh, 1_c / (1_c-x*x))
  TRIG_DERIV(Exp, Exp())
  TRIG_DERIV(Sqrt, 0.5_c / Sqrt())
  TRIG_DERIV(Zero, zero)

  template <int i>
  auto Derivative(Pow, Variable<i>)
  {
    return Zero();
  }

  auto Derivative(Pow, Variable<0>)
  {
    return y * Pow()(x, y-1_c);
  }

  auto Derivative(Pow, Variable<1>)
  {
    return (Log()(x)) * Pow()(x, y);
  }

#undef TRIG_DERIV

  template <int i>
  auto Derivative(Variable<i>, Variable<i>)
  {
    return 1_c;
  }

  template <int i, int j>
  auto Derivative(Variable<i>, Variable<j>)
  {
    return zero;
  }

  template <class T>
  auto Derivative(T t)
  {
    return Derivative(t, x);
  }

  template <int order, int i = 0>
  struct D
  {
    template <class F>
    auto operator()(F f) const
    {
      return D<order-1,i>()(Derivative(f, Variable<i>()));
    }
  };

  template <int i>
  struct D<0,i>
  {
    template <class F>
    auto operator()(F f) const
    {
      return f;
    }
  };
}

#endif
