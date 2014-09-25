#ifndef MANIFOLDS_FUNCTIONS_MULTIPLICATION_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_MULTIPLICATION_SIMPLIFICATIONS_HH

#include "simplify_variadic.hh"
#include "unary_minus.hh"
#include "addition.hh"

namespace manifolds
{
  template <class ... Fs>
  auto Multiply(Fs...fs)
  {
    return Simplify(MultiplyRaw(fs...));
  }

  template <class ... Functions>
  struct Simplification<
    Multiplication<Functions...>, /*var_mult*/4>
  {
    typedef bool_<
      has_abelian_arithmetic<
	Multiplication<
	  Functions...>>::value> Abelian;
    typedef decltype(SimplifyV<Multiplication>
		     (std::declval<tuple<Functions...>>(),
		      Abelian{})) type;

    static type Combine(Multiplication<Functions...> a)
    {
      SIMPLIFY_INFO("Simplifying multiplication\n");
      return SimplifyV<Multiplication>(a.GetFunctions(),
				       Abelian{});
    }
  };

  template <class ... MFuncs, class C>
  struct Simplification<
    Composition<Multiplication<MFuncs...>,C>,
    /*com_mult_f*/2>
  {
    typedef Composition<Multiplication<MFuncs...>,C> in_type;
    static auto Combine(in_type c)
    {
      SIMPLIFY_INFO("Distributing Composition "
		    "into multiplication\n");
      return DistributeComposition<Multiplication>(c);
    }

    typedef decltype(Combine(std::declval<in_type>())) type;
  };

  template <class F1, class F2>
  struct Simplification<
    Multiplication<
    F1, UnaryMinus<F2>>,
    /*mult_f1_um_f2*/2>
  {
    typedef UnaryMinus<Multiplication<F1,F2>> type;
    static type Combine(Multiplication<
			F1, UnaryMinus<F2>> m)
    {
      SIMPLIFY_INFO("Simplifying multiplication of function "
		    "by negative of another function");
      auto t = m.GetFunctions();
      Multiplication<F1,F2> r =
	Multiplication<F1,F2>
	(get<0>(t), get<1>(t).GetFunction());
      return UnaryMinus<decltype(r)>(r);
    }
  };

  template <class F1, class F2>
  struct Simplification<
    Multiplication<
      UnaryMinus<F1>, F2>, /*mult_um_f1_f2*/1>
  {
    typedef UnaryMinus<Multiplication<F1,F2>> type;
    static type Combine(Multiplication<
			UnaryMinus<F1>, F2> m)
    {
      SIMPLIFY_INFO("Simplifying multiplication of negative "
		    "of function by another function");
      auto l = get<0>(m.GetFunctions()).GetFunction();
      auto r = get<1>(m.GetFunctions());
      auto mr = Multiplication<
	decltype(l), decltype(r)>(l,r);
      return UnaryMinus<decltype(mr)>(mr);
    }
  };

    template <class ... F1s, class ... F2s>
    struct Simplification<
        Multiplication<
            Addition<F1s...>, Addition<F2s...>>,
      /*mult_add_add*/0>
    {
        template <class T1, class T2, std::size_t ... indices>
        static auto Second(T1 t1, T2 t2, std::integer_sequence<
                               std::size_t, indices...>)
        {
            return AddRaw(Multiply(t1, get<indices>(t2))...);
        }

        template <class T1, class T2, std::size_t ... indices>
        static auto First(T1 t1, T2 t2,
			  std::integer_sequence<
			  std::size_t, indices...>)
        {
            return AddRaw(Second(get<indices>(t1), t2,
				 std::make_index_sequence<
				 tuple_size<T2>::value>())...);
        }

        typedef Multiplication<
            Addition<F1s...>, Addition<F2s...> > in_type;

        static auto Combine(in_type m)
        {
	  SIMPLIFY_INFO("Simplifying multiplication of "
			"addition of functions");
            auto t1 =
	      get<0>(m.GetFunctions()).GetFunctions();
            return First(t1, get<1>(m.GetFunctions()).
			 GetFunctions(),
                         std::make_index_sequence<
			 tuple_size<decltype(t1)>::value>());
        }

        typedef decltype(Combine(std::declval<in_type>())) type;
    };
}

#endif
