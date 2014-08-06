#ifndef MANIFOLDS_FUNCTIONS_POLY_COMPOS_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_POLY_COMPOS_SIMPLIFICATIONS_HH

namespace manifolds {

  template <class A>
  struct Simplification<
    Addition<A,A>,
    typename std::enable_if<
      is_stateless<A>::value &&
      !std::is_same<A,Zero>::value>::type>
  {
    typedef Composition<
      Polynomial<double,int_<2>>,
      A> type;

    static type Combine(Addition<A,A> a)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying addition of function by itself\n";
#endif
      type b = {GetPolynomial(0.0,2.0), A()};
      return b;
    }
  };

  template <class A>
  struct Simplification<
    Multiplication<A,A>,
    typename std::enable_if<is_stateless<A>::value>::type>
  {
    typedef Composition<
      Polynomial<double,int_<3>>,A> type;

    static type Combine(Multiplication<A,A>)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying multiplication of function "
	"by itself\n";
#endif
      return {GetPolynomial(0,0,1), A()};
    }
  };

  template <class Arg1, class Arg2, class ... Args>
  struct Simplification<
    Multiplication<Composition<Arg1, Args...>,
		   Composition<Arg2, Args...>>,
    typename std::enable_if<
      and_<is_stateless<Args>...>::value &&
      (!std::is_same<Arg1,Arg2>::value ||
       !is_stateless<Arg1>::value ||
       !is_stateless<Arg2>::value) &&
      !std::is_same<
	typename Simplification<Multiplication<Arg1,Arg2>>::type,
	Multiplication<Arg1,Arg2>
	>::value>::type>
  {
    typedef Composition<
      typename Simplification<
	Multiplication<Arg1, Arg2>>::type,
      Args...> type;

    static type Combine(Multiplication<Composition<Arg1, Args...>,
			Composition<Arg2, Args...>> a)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying multipication of composition of "
	"simplifiable functions\n";
#endif
      auto first = std::get<0>(std::get<0>(a.GetFunctions()).
			       GetFunctions());
      auto second = std::get<0>(std::get<1>(a.GetFunctions()).
				GetFunctions());
      Multiplication<decltype(first),decltype(second)>
	m(first,second);
      auto s = Simplify(m);
      return {s, Args()...};
    }
  };

  template <class CType, int order, class T>
  struct Simplification<
    Addition<Composition<
	       Polynomial<CType, int_<order>>, T>, T>,
    typename std::enable_if<is_stateless<T>::value>::type>
  {
    static const int new_order = 2 > order ? 2 : order;
    typedef Composition<Polynomial<CType, int_<new_order>>, T> type;
    template <class A, class B>
      static auto Add(A a, B b)
    {
      return Addition<A,B>(a,b);
    }
    static type Combine(Addition<Composition<
			Polynomial<CType, int_<order>>, T>, T> a)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying addition of composed polynomial "
	"by inner function\n";
#endif
      auto p = std::get<0>(std::get<0>(a.GetFunctions()).
			   GetFunctions());
      return Simplify(Add(GetPolynomial(0,1), p))(T());
    }
  };

  template <class CType, int order, class T>
  struct Simplification<
    Multiplication<Composition<
		     Polynomial<CType, int_<order>>, T>, T>,
    typename std::enable_if<is_stateless<T>::value>::type>
  {
    static const int new_order = 1 + order;
    typedef Composition<Polynomial<CType, int_<new_order>>, T> type;

    template <class A, class B>
      static auto Mult(A a, B b)
    {
      return Simplify(Multiplication<A,B>(a,b));
    }

    static type Combine(Multiplication<Composition<
			Polynomial<CType, int_<order>>, T>, T> a)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying multiplication of composed "
	"polynomial by inner function\n";
#endif
      auto m =
	Mult(GetPolynomial(0,1),
	     std::get<0>(std::get<0>
			 (a.GetFunctions()).
			 GetFunctions()));
      T t;
      return m(t);
    }
  };

  template <template <class...>class Variadic,
	    class CType, int order, class T>
  struct Simplification<
    Variadic<T, Composition<
	       Polynomial<CType, int_<order>>, T>>,
    typename std::enable_if<is_stateless<T>::value>::type>
  {
    typedef Simplification<
      Variadic<Composition<
		 Polynomial<CType, int_<order>>,
		 T>, T>> ref_type;
    typedef typename ref_type::type type;

    static type Combine(Variadic<T, Composition<
			Polynomial<CType, int_<order>>, T>> a)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying inverted composed "
	"polynomial operation\n";
#endif
      auto t = a.GetFunctions();
      typedef typename std::remove_reference<
	decltype(std::get<1>(t))>::type T1;
      typedef typename std::remove_reference<
	decltype(std::get<0>(t))>::type T2;
      Variadic<T1, T2> v(std::get<1>(t), std::get<0>(t));
      return ref_type::Combine(v);
    }
  };

  template <class CoeffType,
	    class ... Functions>
  struct Simplification<
    Composition<Polynomial<CoeffType,int_<1>>,Functions...>>
  {
    typedef Polynomial<CoeffType,int_<1>> type;

    static type Combine(Composition<Polynomial<
			CoeffType,int_<1>>,Functions...> p)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying composition of "
	"constant polynomial\n";
#endif
      return std::get<0>(p.GetFunctions());
    }
  };
}

#endif
