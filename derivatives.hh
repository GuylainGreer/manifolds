#ifndef MANIFOLDS_FUNCTIONS_DERIVATIVES_HH
#define MANIFOLDS_FUNCTIONS_DERIVATIVES_HH

#include "addition.hh"
#include "multiplication.hh"
#include "variables.hh"
#include "division.hh"
#include "polynomial.hh"
#include "trig.hh"
#include "operators.hh"
#include "matrix.hh"

namespace manifolds {

  template <int iter>
  struct DerivativeWrapper
  {
    template <class ... Funcs, int i,
	      std::size_t ... indices>
    static auto Derivative(const Addition<Funcs...> & a,
		    Variable<i> v,
		    std::integer_sequence<std::size_t, indices...>)
    {
      return 
	Add(DerivativeWrapper<iter+1>::
	    Derivative(std::get<indices>(a.GetFunctions())..., v));
    }

    template <class ... Funcs, int i>
    static auto Derivative(const Addition<Funcs...> & a,
		    Variable<i> v)
    {
      return DerivativeWrapper<iter+1>::
	Derivative(a, v, std::index_sequence_for<Funcs...>());
    }

    template <class F, int i>
    static auto Derivative(UnaryMinus<F> f, Variable<i> v)
    {
      return -DerivativeWrapper<iter+1>::
	Derivative(f.GetFunction(), v);
    }

    template <class Func, int i>
    static auto Derivative(const Composition<Func> & c,
		    Variable<i> v)
    {
      return DerivativeWrapper<iter+1>::
	Derivative(std::get<0>(c.GetFunctions()), v);
    }


    template <class Func, class ... Funcs, int i,
	      class = typename std::enable_if<
		(sizeof...(Funcs)>0)>::type>
    static auto Derivative(const Composition<Func,Funcs...> & c,
		    Variable<i> v)
    {
      auto funcs = remove_element<0>(c.GetFunctions());
      auto d = Derivative(std::get<0>(c.GetFunctions()),
			  Variable<0>());
      auto next =
	std::tuple_cat(std::make_tuple(d), funcs);
      return Composition<decltype(d), Funcs...>(next) *
	DerivativeWrapper<iter+1>::
	Derivative(Composition<Funcs...>(funcs), v);
    }

    template <class F1, class F2, int i>
    static auto Derivative(const Division<F1,F2>& d,
		    Variable<i> v)
    {
      auto n = d.GetNumerator();
      auto de = d.GetDenominator();
      return (DerivativeWrapper<iter+1>::Derivative(n, v) * de -
	      n * DerivativeWrapper<iter+1>::Derivative(de, v)) /
	(de * de);
    }

    template <int i, class ... Funcs, int d_i,
	      std::size_t ... lefts, std::size_t ... rights>
    static auto TimesDerivHelper(const Multiplication<Funcs...> & a,
			  Variable<d_i> v,
			  std::integer_sequence<
			  std::size_t, lefts...>,
			  std::integer_sequence<
			  std::size_t, rights...>)
    {
      auto t = a.GetFunctions();
      return Multiply(std::get<lefts>(t)...,
		      DerivativeWrapper<iter+1>::
		      Derivative(std::get<i>(t),v),
		      std::get<rights+i+1>(t)...);
    }

    template <int i, class ... Funcs, int d_i>
    static auto TimesDerivHelper(const Multiplication<Funcs...> & a,
			  Variable<d_i> v)
    {
      return TimesDerivHelper<i>
	(a, v, std::make_index_sequence<i>(),
	 std::make_index_sequence<
	 sizeof...(Funcs)-i-1>());
    }

    template <class ... Funcs, int i,
	      std::size_t ... indices>
    static auto Derivative(const Multiplication<Funcs...> & a,
		    Variable<i> v,
		    std::integer_sequence<std::size_t, indices...>)
    {
      return 
	Add(TimesDerivHelper<indices>(a, v)...);
    }

    template <class ... Funcs, int i>
    static auto Derivative(const Multiplication<Funcs...> & a,
		    Variable<i> v)
    {
      return DerivativeWrapper<iter+1>::
	Derivative(a, v, std::index_sequence_for<Funcs...>());
    }

    template <class CoeffType, class Order, int i>
    static auto Derivative(const Polynomial<CoeffType, Order> & p,
		    Variable<i>)
    {
      return zero;
    }

    template <class CoeffType, int i>
    static auto Derivative(Polynomial<CoeffType, int_<1>> p,
		    Variable<i>)
    {
      return zero;
    }

    template <class CoeffType, class Order,
	      class = typename std::enable_if<
		Order::value != 1
		>::type>
    static auto Derivative(const Polynomial<CoeffType, Order> & p,
		    Variable<0>)
    {
      std::array<CoeffType, Order::value-1> d;
      for(unsigned i = 0; i < Order::value-1; i++)
	d[i] = (i+1)*p.GetCoeffs()[i+1];
      return Polynomial<CoeffType,int_<Order::value-1>>{d};
    }

#define TRIG_DERIV(func, deriv)					\
    static auto Derivative(func,Variable<0>){return deriv;}	\
    template <int i>						\
    static auto Derivative(func,Variable<i>){return zero;}

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

#undef TRIG_DERIV

    template <int i>
    static auto Derivative(Pow, Variable<i>)
    {
      return zero;
    }

    static auto Derivative(Pow, Variable<0>)
    {
      return y * Pow()(x, y-1_c);
    }

    static auto Derivative(Pow, Variable<1>)
    {
      return (Log()(x)) * Pow()(x, y);
    }

    template <int i>
    static auto Derivative(Variable<i>, Variable<i>)
    {
      return 1_c;
    }

    template <int i, int j>
    static auto Derivative(Variable<i>, Variable<j>)
    {
      return zero;
    }

    template <class T, std::size_t ... ins>
    static auto FullDeriv(T t, int_<T::output_dim>,
		   std::integer_sequence<std::size_t, ins...>)
    {
      return std::tuple<>();
    }

    template <class T>
    static auto GetOutputs(T t, int_<1>)
    {
      return std::make_tuple(t);
    }

    template <class T, int i>
    static auto GetOutputs(T t, int_<i>)
    {
      return t.GetOutputs();
    }

    template <class T>
    static auto GetOutputs(T t)
    {
      return GetOutputs(t, int_<T::output_dim>());
    }

    template <int col, class T, std::size_t ... ins,
	      class = typename std::enable_if<
		col != T::output_dim>::type>
    static auto FullDeriv(T t, int_<col>,
		   std::integer_sequence<std::size_t, ins...> b)
    {
      auto r =
	std::make_tuple(Derivative(std::get<col>(GetOutputs(t)),
				   Variable<ins>())...);
      return tuple_cat(r, DerivativeWrapper<iter+1>::
		       FullDeriv(t, int_<col+1>(), b));
    }

    template <class T>
    static auto FullDerivOutput(std::tuple<T> t,
				int_<1>, int_<1>)
    {
      return std::get<0>(t);
    }

    template <class ... Ts, int rows, int cols>
    static auto FullDerivOutput(std::tuple<Ts...> t,
				int_<rows>, int_<cols>)
    {
      return GetFunctionMatrix<rows,cols>(t);
    }

    template <class T>
    static auto FullDeriv(T t)
    {
      return DerivativeWrapper<iter+1>::
	FullDeriv(t, int_<0>(), std::make_index_sequence<
		  T::input_dim>());
    }
  };

  template <class T, int i>
  auto Derivative(T t, Variable<i> v)
  {
    return DerivativeWrapper<0>::Derivative(t, v);
  }

  template <class T>
  auto Derivative(T t)
  {
    return DerivativeWrapper<0>::Derivative(t, x);
  }

  template <class T>
  auto FullDerivative(T t)
  {
    return DerivativeWrapper<0>::
      FullDerivOutput(DerivativeWrapper<0>::FullDeriv(t),
		      int_<T::output_dim>(),
		      int_<T::input_dim>());
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
