#ifndef MANIFOLDS_FUNCTIONS_POLY_COMPOS_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_POLY_COMPOS_SIMPLIFICATIONS_HH

namespace manifolds {

  template <class A>
  struct Simplification<
    Addition<A,A>, /*add_f_f*/3,
    typename std::enable_if<
      is_stateless<A>::value>::type>
  {
    typedef Composition<
      Polynomial<double,int_<2>>,
      A> type;
    static type Combine(Addition<A,A> a)
    {
      SIMPLIFY_INFO("Simplifying addition of function by itself\n");
      type b = {GetPolynomial(0.0,2.0), A()};
      return b;
    }
  };

  template <class A>
  struct Simplification<
    Multiplication<A,A>, /*mult_f_f*/3,
    typename std::enable_if<is_stateless<A>::value>::type>
  {
    typedef Composition<
      Polynomial<double,int_<3>>,A> type;

    static type Combine(Multiplication<A,A>)
    {
      SIMPLIFY_INFO("Simplifying multiplication of function "
		    "by itself\n");
      return {GetPolynomial(0,0,1), A()};
    }
  };

  template <template <class...>class Variadic,
	    class Arg1, class Arg2, class ... Args>
  struct Simplification<
    Variadic<Composition<Arg1, Args...>,
	     Composition<Arg2, Args...>>,
    /*mult_f_fvar_com_f1_fs_com_f2_fs*/1,
    typename std::enable_if<
      and_<is_stateless<Args>...>::value &&
      (!std::is_same<Arg1,Arg2>::value ||
       !is_stateless<Arg1>::value ||
       !is_stateless<Arg2>::value) &&
    Simplifies<Variadic<Arg1,Arg2>
	       >::value>::type>
  {
    typedef Composition<
      SimplifiedType<
	Variadic<Arg1, Arg2>>,
      Args...> type;

    static type Combine(Variadic<Composition<Arg1, Args...>,
			Composition<Arg2, Args...>> a)
    {
      SIMPLIFY_INFO("Simplifying variadic operation of "
		    "composition of "
		    "simplifiable functions\n");
      auto first = get<0>(get<0>(a.GetFunctions()).
			       GetFunctions());
      auto second = get<0>(get<1>(a.GetFunctions()).
				GetFunctions());
      Variadic<decltype(first),decltype(second)>
	m(first,second);
      auto s = Simplify(m);
      return {s, Args()...};
    }
  };

  template <class CType, int order, class T>
  struct Simplification<
    Addition<Composition<
	       Polynomial<CType, int_<order>>, T>, T>,
    /*add_com_p_f_f*/2,
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
      SIMPLIFY_INFO("Simplifying addition of composed polynomial "
		    "by inner function\n");
      auto p = get<0>(get<0>(a.GetFunctions()).
			   GetFunctions());
      return Simplify(Add(GetPolynomial(0,1), p))(T());
    }
  };

  template <class CType, int order, class T>
  struct Simplification<
    Multiplication<Composition<
		     Polynomial<CType, int_<order>>, T>, T>,
    /*mult_com_p_f_f*/2,
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
      SIMPLIFY_INFO("Simplifying multiplication of composed "
		    "polynomial by inner function\n");
      auto m =
	Mult(GetPolynomial(0,1),
	     get<0>(get<0>
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
    /*var_f_com_p_f*/1,
    typename std::enable_if<is_stateless<T>::value>::type>
  {
    typedef SimplifiedType<
      Variadic<Composition<
		 Polynomial<CType, int_<order>>,
		 T>, T>> type;

    static type Combine(Variadic<T, Composition<
			Polynomial<CType, int_<order>>, T>> a)
    {
      SIMPLIFY_INFO("Simplifying inverted composed "
		    "polynomial operation\n");
      auto t = a.GetFunctions();
      typedef typename std::remove_reference<
	decltype(get<1>(t))>::type T1;
      typedef typename std::remove_reference<
	decltype(get<0>(t))>::type T2;
      Variadic<T1, T2> v(get<1>(t), get<0>(t));
      return Simplify(v);
    }
  };

  template <class CoeffType,
	    class ... Functions>
  struct Simplification<
    Composition<Polynomial<CoeffType,int_<1>>,Functions...>,
    /*com_p_1_fs*/2>
  {
    typedef Polynomial<CoeffType,int_<1>> type;

    static type Combine(Composition<Polynomial<
			CoeffType,int_<1>>,Functions...> p)
    {
      SIMPLIFY_INFO("Simplifying composition of "
		    "constant polynomial\n");
      return get<0>(p.GetFunctions());
    }
  };

  template <class T, class C>
  struct Simplification<Addition<T, Polynomial<C, int_<1>>>,
			/*add_f_p_1*/2>
  {
    typedef Addition<T, Polynomial<C, int_<1>>> in_type;
    static auto Combine(in_type a)
    {
      SIMPLIFY_INFO("Simplifying adddition of constant "
		    "and another function\n");
      auto t = a.GetFunctions();
      return GetPolynomial(get<1>(t).GetCoeffs()[0], (C)1)
	(get<0>(t));
    }
    typedef decltype(Combine(std::declval<in_type>())) type;
  };

  /*template <template<class...>class Variadic,
	    class C1, class O1,
	    class C2, class O2,
	    class ... F1s>
  struct Simplification<
    Variadic<Composition<Polynomial<C1,O1>,F1s...>,
	     Composition<Polynomial<C2,O2>,F1s...>>, 1,
    typename std::enable_if<
      is_stateless<Composition<F1s...>>::value>::type>
  {
    typedef Variadic<Polynomial<C1,O1>,
		     Polynomial<C2,O2>> inter_type;
    typedef Composition<
      inter_type,
      F1s...> type;
    static type Combine(Variadic<Composition<
			Polynomial<C1,O1>,F1s...>,
			Composition<
			Polynomial<C2,O2>,F1s...>> v)
    {
    SIMPLIFY_INFO("Simplifying operation on "
	"polynomials composed by same functions\n");
      auto t = v.GetFunctions();
      inter_type i(get<0>(get<0>(t).GetFunctions()),
		   get<0>(get<1>(t).GetFunctions()));
      return make_my_tuple(i, F1s()...);
    }
    };*/

  template <class C, class O, class ... Fs>
  struct Simplification<
    Composition<
      Polynomial<C,O>, Addition<Fs...>>,
    /*com_p_add_fs*/3>
  {
    typedef Composition<
      Polynomial<C,O>, Addition<Fs...> > in_type;

    static auto P(C c){return GetPolynomial(c);}

    template <class T, std::size_t>
    struct Dummy
    {
      typedef T type;
    };

    template <std::size_t ... count>
    static auto Mult(C c, Addition<Fs...> a,
		     std::integer_sequence<std::size_t, count...>)
    {
      auto p = P(c);
      Multiplication<
	decltype(p),
	typename Dummy<Addition<Fs...>,
		       count>::type...>
	r(P(c), ((void)count, a)...);
      return r;
    }

    template <std::size_t ... orders>
    static auto Addem(in_type c, std::integer_sequence<
		      std::size_t, orders...>)
    {
      auto a = get<1>(c.GetFunctions());
      auto coeffs = get<0>(c.GetFunctions()).GetCoeffs();
      return Add(Mult(coeffs[orders], a,
		      std::make_index_sequence<orders>())...);
    }

    static auto Combine(in_type c)
    {
      SIMPLIFY_INFO("Simplifying polynomial of addition of "
		    "functions\n");
      return Addem(c, std::make_index_sequence<O::value>());
    }
  };

  template <class T, class ... Fs>
  struct Simplification<
    UnaryMinus<Composition<T,Fs...>>,
    /*um_com_f_fs*/0,
    typename std::enable_if<
      Simplifies<UnaryMinus<T>>::value>::type>
  {
    typedef Composition<
      SimplifiedType<UnaryMinus<T>>, Fs...> type;

    static type Combine(UnaryMinus<Composition<T,Fs...>> f)
    {
      SIMPLIFY_INFO("Simplifying unary minus of composition\n");
      auto t = f.GetFunction().GetFunctions();
      auto ut =
	make_my_tuple(Simplify(UnaryMinus<T>(get<0>(t))));
      return tuple_cat(ut, remove_element<0>(t));
    }
  };

  template <class F>
  struct Simplification<Composition<F,Zero>, /*com_f_z*/1>
  {
    typedef Polynomial<
      decltype(std::declval<F>()(0)), int_<1>> type;

    static type Combine(Composition<F,Zero> c)
    {
      return GetPolynomial(get<0>(c.GetFunctions())(0));
    }
  };

  template <class C, class F>
  struct Simplification<
    Composition<F, Polynomial<C,int_<1>>>,
    /*com_f_p_1*/1>
  {
    typedef Polynomial<
      decltype(std::declval<F>()(std::declval<C>())),
      int_<1>>type;

    static type Combine(Composition<F, Polynomial<C,int_<1>>> c)
    {
      auto t = c.GetFunctions();
      return GetPolynomial(get<0>(t)(get<1>(t).GetCoeffs()[0]));
    }
  };

}

#endif
