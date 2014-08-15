#ifndef MANIFOLDS_FUNCTIONS_RECIRPOCAL_HH
#define MANIFOLDS_FUNCTIONS_RECIRPOCAL_HH

#include "function.hh"
#include "full_function_defs.hh"
#include "streaming.hh"

namespace manifolds {
template <class Numerator,
	  class Denominator>
struct DivisionImpl :
    Function<max<Numerator::input_dim,
		 Denominator::input_dim>::value,
	     max<Numerator::output_dim,
		 Denominator::output_dim>::value>
{
  static const bool stateless =
    and_<is_stateless<Numerator>,
	 is_stateless<Denominator>>::value;

  DivisionImpl(){}
  DivisionImpl(Numerator n, Denominator d):
    n(n),d(d){}

  template <class ... Args>
  auto operator()(Args ... args) const
  {
    return n(args...) / d(args...);
  }

  auto GetNumerator() const
  {
    return n;
  }

  auto GetDenominator() const
  {
    return d;
  }

  auto GetFunctions() const
  {
    return std::make_tuple(n,d);
  }

private:
  Numerator n;
  Denominator d;
};

DEF_FF_TEMPLATE(Division)

template <class A, class B>
std::ostream & operator<<(std::ostream & s, Division<A,B> d)
{
  StreamVariadic("Division", d, s);
  return s;
}

}

#include "multiplication.hh"

namespace manifolds {

  template <class N1, class N2, class D1, class D2>
  struct Simplification<
    Multiplication<Division<N1,D1>,
		   Division<N2,D2>>, 0>
  {
    typedef Division<
      Multiplication<N1,N2>,
      Multiplication<D1,D2>> type;

    static type Combine(Multiplication<Division<N1,D1>,
			Division<N2,D2>> a)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying multiplication of division "
	"by division\n";
#endif
      Multiplication<N1,N2>
	mn(std::get<0>(a.GetFunctions()).GetNumerator(),
	   std::get<1>(a.GetFunctions()).GetNumerator());

      Multiplication<D1,D2>
	md(std::get<0>(a.GetFunctions()).GetDenominator(),
	   std::get<1>(a.GetFunctions()).GetDenominator());

      return {mn, md};
    }
  };

  template <class N, class D, class F>
  struct Simplification<
    Multiplication<Division<N,D>,F>, 0>
  {
    typedef Division<
      Multiplication<N,F>, D> type;

    static type Combine(Multiplication<Division<N,D>,F> a)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying multiplication of division "
	"by function\n";
#endif
      Multiplication<N,F>
	m(std::get<0>(a.GetFunctions()).GetNumerator(),
	  std::get<1>(a.GetFunctions()));
      return {m, std::get<0>(a.GetFunctions()).GetDenominator()};
    }
  };

  template <class N, class D, class F>
  struct Simplification<Division<Division<N,D>,F>, 0>
  {
    typedef Division<N, Multiplication<D,F>> type;

    static type Combine(Division<Division<N,D>,F> a)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying division of division by function\n";
#endif
      return {a.GetNumerator().GetNumerator(),
	  Multiplication<D,F>(a.GetNumerator().GetDenominator(),
			      a.GetDenominator())};
    }
  };

  template <class N, class D, class F>
  struct Simplification<Division<F, Division<N,D>>, 0>
  {
    typedef Division<Multiplication<D,F>, N> type;

    static type Combine(Division<Division<N,D>,F> a)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying division of division by function\n";
#endif
      return {Multiplication<D,F>(a.GetNumerator().
				  GetDenominator(),
				  a.GetDenominator()),
	  a.GetNumerator().GetNumerator()};
    }
  };
}
#endif
