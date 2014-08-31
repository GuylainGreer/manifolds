#ifndef MANIFOLDS_FUNCTION_MULTIPLICATION_HH
#define MANIFOLDS_FUNCTION_MULTIPLICATION_HH

#include "function.hh"
#include "full_function_defs.hh"
#include "equals.hh"
#include "streaming.hh"
#include "simplify_variadic.hh"

namespace manifolds {

template <class ... Functions>
struct MultiplicationImpl :
    Function<
  list<int_<5>, typename Functions::indices...>,
  max<Functions::input_dim...>::value,
  max<Functions::output_dim...>::value>
{
  static const bool stateless =
    and_<is_stateless<Functions>...>::value;

  MultiplicationImpl(){}
  MultiplicationImpl(const Functions & ... functions):
    functions(functions...){}
  MultiplicationImpl(const tuple<Functions...> & f):
    functions(f){}

  template <class ... Args>
  auto operator()(Args ... args) const
  {
    return eval(int_<0>(), args...);
  }

  template <class ... Args>
  auto eval(int_<sizeof...(Functions)-1>, Args ... args) const
  {
    return get<sizeof...(Functions)-1>
      (functions)(args...);
  }

  template <int N, class ... Args>
  auto eval(int_<N>, Args ... args) const
  {
    typedef decltype(get<N>(functions)(args...)) result;
    return std::forward<result>(get<N>(functions)(args...)) *
      eval(int_<N + 1>(), std::forward<Args>(args)...);
  }

  auto GetFunctions() const
  {
    return functions;
  }

  template <class T>
  bool operator==(const T & t) const
  {
    return VariadicEqual(*this, t);
  }

private:
  tuple<Functions...> functions;
};

  DEF_FF_TEMPLATE(Multiplication)

  template <class Func, class ... Functions>
  std::ostream & operator<<(std::ostream & s,
			    Multiplication<Func, Functions...> m)
  {
    StreamVariadic("Multiplication", m, s);
    return s;
  }

  template <class ... Fs>
  Multiplication<Fs...> MultiplyRaw(Fs ... fs)
  {
    return {fs...};
  }

  template <class ... Fs>
  auto Multiply(Fs...fs)
  {
    return Simplify(MultiplyRaw(fs...));
  }

  template <class ... Functions>
  struct Simplification<
    Multiplication<Functions...>,3>
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
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying multiplication\n";
#endif
      return SimplifyV<Multiplication>(a.GetFunctions(),
				       Abelian{});
    }
  };

  template <class ... MFuncs, class C>
  struct Simplification<Composition<Multiplication<MFuncs...>,C>>
  {
    typedef Composition<Multiplication<MFuncs...>,C> in_type;
    static auto Combine(in_type c)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Distributing Composition "
	"into multiplication\n";
#endif
      return DistributeComposition<Multiplication>(c);
    }

    typedef decltype(Combine(std::declval<in_type>())) type;
  };
}

#include "unary_minus.hh"
#include "addition.hh"

namespace manifolds
{
  template <class F1, class F2>
  struct Simplification<
    Multiplication<
    F1, UnaryMinus<F2>>>
  {
    typedef UnaryMinus<Multiplication<F1,F2>> type;
    static type Combine(Multiplication<
			F1, UnaryMinus<F2>> m)
    {
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
      UnaryMinus<F1>, F2>, 1>
  {
    typedef UnaryMinus<Multiplication<F1,F2>> type;
    static type Combine(Multiplication<
			UnaryMinus<F1>, F2> m)
    {
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
            Addition<F1s...>, Addition<F2s...>>, 0>
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
