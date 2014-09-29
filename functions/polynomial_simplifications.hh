#ifndef MANIFOLDS_FUNCTIONS_POLYNOMIAL_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_POLYNOMIAL_SIMPLIFICATIONS_HH

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
    Polynomial<CoeffType2, int_<order2>>>,
			/*add_p1_p2*/0>
  {
    static const int new_order =
      order1 > order2 ? order1 : order2;
    typedef typename std::common_type<
      CoeffType1, CoeffType2>::type coeff_type;
    typedef Polynomial<
      coeff_type,
      int_< new_order>> type;

    template <class T1, class T2>
      static auto Init(const T1 & t1, const T2 & t2,
		       std::true_type)
    {
      typedef typename std::common_type<
	decltype(t1.GetCoeffs()[0]),
	decltype(t2.GetCoeffs()[0])>::type type;
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
      SIMPLIFY_INFO("Simplifying addition of two polynomials\n");
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
  struct Simplification<
    Multiplication<
    Polynomial<CoeffType1,
	       int_<order1>>,
    Polynomial<CoeffType2,
	       int_<order2>>>,
    /*mult_p1_p2*/0>
  {
    static const int new_order = order1 + order2 - 1;
    typedef typename std::common_type<
      CoeffType1, CoeffType2>::type coeff_type;
    typedef Polynomial<
      coeff_type,
      int_<new_order>> type;

    static type Combine(Multiplication<Polynomial<
			CoeffType1,
			int_<order1>>,
			Polynomial<
			CoeffType2,
			int_<order2>>> p)
    {
      SIMPLIFY_INFO("Simplifying multiplication "
		    "of two polynomials\n");
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
      Polynomial<CoeffType2,int_<order2>>>,
    /*com_p1_p2*/3>
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
      SIMPLIFY_INFO("Simplifying composition of two polynomials\n");
      auto p1 = get<0>(p.GetFunctions());
      auto p2 = get<1>(p.GetFunctions());
      return
	Accumulate(p1, p2, int_<order1-1>());
    }
  };

  template <class Order, class CoeffType>
  struct Simplification<
    UnaryMinus<Polynomial<CoeffType, Order>>,
    /*um_p*/0>
  {
    typedef typename std::conditional<
      std::is_unsigned<CoeffType>::value,
      std::make_signed<CoeffType>,
      Identity<CoeffType>
      >::type::type CoeffType2;
    typedef Polynomial<CoeffType2, Order> type;

    static type Combine(UnaryMinus<Polynomial<
			CoeffType, Order>> u)
    {
      SIMPLIFY_INFO("Simplifying negative of polynomial\n");
      std::array<CoeffType2, Order::value> a;
      for(int i = 0; i < Order::value; i++)
	{
	  a[i] = -CoeffType2(u.GetFunction().GetCoeffs()[i]);
	}
      return {a};
    }
  };

  template <class Order, class CoeffType1,
	    class CoeffType2, class ... Funcs>
  struct Simplification<
    Multiplication<Composition<Polynomial<CoeffType1, Order>,
			       Funcs...>,
		   Polynomial<CoeffType2, int_<1>>>,
    /*mult_com_p1_fs_p2_1*/0>
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
      SIMPLIFY_INFO("Simplifying multiplication of composed "
		    "polynomial by a constant\n");
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

  template <template <class...>class Variadic,
	    class Arg1, class Arg2, class ... Args>
  struct Simplification<
    Variadic<Composition<Arg1, Args...>,
	     Composition<Arg2, Args...>>,
    /*mult_f_fvar_com_f1_fs_com_f2_fs*/1,
    typename std::enable_if<
      and_<is_stateless<Args>...>::value &&
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
      SIMPLIFY_INFO("Simplifying composition of "
		    "function with 0");
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
      SIMPLIFY_INFO("Simplifying composition of "
		    "function by constant polynomial");
      auto t = c.GetFunctions();
      return GetPolynomial(get<0>(t)(get<1>(t).GetCoeffs()[0]));
    }
  };
}

#endif
