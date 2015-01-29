#ifndef MANIFOLDS_FUNCTIONS_INTEGRAL_POLYNOMIAL_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_INTEGRAL_POLYNOMIAL_SIMPLIFICATIONS_HH

#include "integral_polynomial.hh"
#include "polynomial.hh"
#include "addition.hh"
#include "simplify.hh"
#include "unary_minus.hh"
#include "zero.hh"

namespace manifolds {
template <class T1, class T2, T1... t1s, T2... t2s>
struct Simplification<
    Addition<IntegralPolynomial<std::integer_sequence<T1, t1s...> >,
             IntegralPolynomial<std::integer_sequence<T2, t2s...> > >,
    /*add_ip_ip*/ 0> {
  typedef std::int64_t Tr;

  template <class, class> struct Extended;

  template <class T, T... indices, Tr... padding>
  struct Extended<std::integer_sequence<T, indices...>,
                  std::integer_sequence<Tr, padding...> > {
    typedef std::integer_sequence<Tr, indices..., (padding - padding)...> type;
  };

  static const int padding1 =
      sizeof...(t1s) < sizeof...(t2s) ? sizeof...(t2s) - sizeof...(t1s) : 0;
  typedef typename Extended<std::integer_sequence<T1, t1s...>,
                            std::make_integer_sequence<Tr, padding1> >::type
  padded1;

  static const int padding2 =
      sizeof...(t2s) < sizeof...(t1s) ? sizeof...(t1s) - sizeof...(t2s) : 0;
  typedef typename Extended<std::integer_sequence<T2, t2s...>,
                            std::make_integer_sequence<Tr, padding2> >::type
  padded2;

  template <class, class> struct Add;

  template <Tr... indices1, Tr... indices2>
  struct Add<std::integer_sequence<Tr, indices1...>,
             std::integer_sequence<Tr, indices2...> > {
    typedef std::integer_sequence<Tr, (indices1 + indices2)...> type;
  };

  typedef IntegralPolynomial<typename Add<padded1, padded2>::type> type;

  static type
  Combine(Addition<IntegralPolynomial<std::integer_sequence<T1, t1s...> >,
                   IntegralPolynomial<std::integer_sequence<T2, t2s...> > >) {
    SIMPLIFY_INFO("Simplifying addition of "
                  "integral_polynomials\n");
    return type{};
  }
};

template <class T1, class T2, T1... t1s, T2... t2s>
struct Simplification<
    Multiplication<IntegralPolynomial<std::integer_sequence<T1, t1s...> >,
                   IntegralPolynomial<std::integer_sequence<T2, t2s...> > >,
    /*mult_ip_ip*/ 0> {
  typedef long Tr;

  typedef std::integer_sequence<T1, t1s...> inputs1;
  typedef std::integer_sequence<T2, t2s...> inputs2;

  template <int, class> struct get_value;

  template <int i, class T, T... ts>
  struct get_value<
      i, std::integer_sequence<
             T, ts...> > : nth<i, std::integral_constant<T, ts>...>::type {};

  template <int, Tr, class> struct set_value;

  template <int i, Tr t, class T, T... ts>
  struct set_value<i, t, std::integer_sequence<T, ts...> > {
    typedef std::integer_sequence<T, ts...> values;
    template <class, class> struct apply;

    template <int... lefts, int... rights>
    struct apply<std::integer_sequence<int, lefts...>,
                 std::integer_sequence<int, rights...> > {
      typedef std::integer_sequence<Tr, get_value<lefts, values>::value..., t,
                                    get_value<rights + i + 1, values>::value...>
      type;
    };

    typedef typename apply<
        std::make_integer_sequence<int, i>,
        std::make_integer_sequence<int, sizeof...(ts) - i - 1> >::type type;
  };

  template <int i, int j, class OutSeq,
            bool = (i + j < seq_size<OutSeq>::value)>
  struct Add {
    typedef typename set_value<i + j, get_value<i + j, OutSeq>::value +
                                          (Tr)get_value<i, inputs1>::value *(
                                              Tr)get_value<j, inputs2>::value,
                               OutSeq>::type type;
  };

  template <int i, int j, class T, T... ts>
  struct Add<i, j, std::integer_sequence<T, ts...>, false> {
    static_assert(i + j == sizeof...(ts), "Internal error");
    typedef std::integer_sequence<T, ts..., (Tr)get_value<i, inputs1>::value *(
                                                Tr)get_value<j, inputs2>::value>
    type;
  };

  template <int i, int j, class OutSeq> struct P2Loop {
    typedef typename P2Loop<i, j - 1, typename Add<i, j, OutSeq>::type>::type
    type;
  };

  template <int i, class OutSeq> struct P2Loop<i, -1, OutSeq> {
    typedef OutSeq type;
  };

  template <int i, class OutSeq> struct P1Loop {
    typedef typename P1Loop<
        i - 1, typename P2Loop<i, sizeof...(t2s) - 1, OutSeq>::type>::type type;
  };

  template <class OutSeq> struct P1Loop<-1, OutSeq> {
    typedef OutSeq type;
  };

  template <class> struct InitOutput;

  template <int... indices> struct InitOutput<iseq<indices...> > {
    typedef std::integer_sequence<Tr, ((void)indices, 0)...> type;
  };

  typedef IntegralPolynomial<typename P1Loop<
      sizeof...(t1s) - 1,
      typename InitOutput<
          miseq<sizeof...(t1s) + sizeof...(t2s) - 1> >::type>::type> type;

  static type Combine(
      Multiplication<IntegralPolynomial<std::integer_sequence<T1, t1s...> >,
                     IntegralPolynomial<std::integer_sequence<T2, t2s...> > >) {
    SIMPLIFY_INFO("Simplifying multiplication of "
                  "integral_polynomials\n");
    return type{};
  }
};

template <class T, T... ts>
struct Simplification<IntegralPolynomial<std::integer_sequence<T, ts...> >,
                      /*ip*/ 0> {
  template <T t> using ic = std::integral_constant<T, t>;

  template <T... t2s> struct pop {
    template <class> struct apply;

    template <std::size_t... is>
    struct apply<std::integer_sequence<std::size_t, is...> > {
      typedef std::integer_sequence<T, nth<is, ic<t2s>...>::type::value...>
      type;
    };

    typedef typename apply<std::make_index_sequence<sizeof...(t2s) - 1> >::type
    type;
  };

  template <class, bool> struct pop_if;

  template <class> struct stopping_condition;

  template <T... touts>
  struct stopping_condition<std::integer_sequence<T, touts...> > {
    static const bool value =
        sizeof...(ts) == 1 || last<ic<touts>...>::type::value != 0;
  };

  template <T... touts>
  struct pop_if<std::integer_sequence<T, touts...>, true> {
    typedef std::integer_sequence<T, touts...> type;
  };

  template <T... touts>
  struct pop_if<std::integer_sequence<T, touts...>, false> {
    typedef typename pop<touts...>::type next;
    typedef typename pop_if<next, stopping_condition<next>::value>::type type;
  };

  typedef IntegralPolynomial<typename pop_if<
      std::integer_sequence<T, ts...>,
      stopping_condition<std::integer_sequence<T, ts...> >::value>::type> type;

  static type Combine(IntegralPolynomial<std::integer_sequence<T, ts...> >) {
    SIMPLIFY_INFO("Simplifying of "
                  "integral_polynomial\n");
    return type{};
  }
};

template <template <class...> class Variadic, class T1, class O1, class T2,
          T2... t2s>
struct Simplification<
    Variadic<Polynomial<T1, O1>,
             IntegralPolynomial<std::integer_sequence<T2, t2s...> > >,
    /*var_p_ip*/2> {
  typedef Variadic<Polynomial<T1, O1>, Polynomial<T2, int_<sizeof...(t2s)> > >
  type;

  static type
  Combine(Variadic<Polynomial<T1, O1>,
                   IntegralPolynomial<std::integer_sequence<T2, t2s...> > > v) {
    SIMPLIFY_INFO("Simplifying variadic of "
                  "polynomial and integral_polynomial\n");
    return { get<0>(v.GetFunctions()), get<1>(v.GetFunctions()).ToPoly() };
  }
};

template <template <class...> class Variadic, class T1, class O1, class T2,
          T2... t2s>
struct Simplification<
    Variadic<IntegralPolynomial<std::integer_sequence<T2, t2s...> >,
             Polynomial<T1, O1> >,
    /*var_ip_p*/ 0> {
  typedef Variadic<Polynomial<T2, int_<sizeof...(t2s)> >, Polynomial<T1, O1> >
  type;

  static type
  Combine(Variadic<IntegralPolynomial<std::integer_sequence<T2, t2s...> >,
                   Polynomial<T1, O1> > v) {
    SIMPLIFY_INFO("Simplifying variadic of "
                  "integral_polynomial and polynomial\n");
    return { get<0>(v.GetFunctions()).ToPoly(), get<1>(v.GetFunctions()) };
  }
};

template <class T, T... coeffs>
struct Simplification<
    UnaryMinus<IntegralPolynomial<std::integer_sequence<T, coeffs...> > >, 0> {
  typedef typename std::conditional<std::is_unsigned<T>::value,
                                    std::make_signed<T>,
                                    Identity<T> >::type::type CType;

  typedef IntegralPolynomial<std::integer_sequence<CType, (-(CType)coeffs)...> >
  type;

  static type Combine(
      UnaryMinus<IntegralPolynomial<std::integer_sequence<T, coeffs...> > >) {
    SIMPLIFY_INFO("Simplifying negative of integral polynomial");
    return type{};
  }
};

template <class A>
struct Simplification<Addition<A, A>, /*add_f_f*/ 3,
                      typename std::enable_if<is_stateless<A>::value>::type> {
  typedef Composition<IntegralPolynomial<iseq<0, 2> >, A> type;
  static type Combine(Addition<A, A> a) {
    SIMPLIFY_INFO("Simplifying addition of function by itself\n");
    type b = { IntegralPolynomial<iseq<0, 2> >(), A() };
    return b;
  }
};

template <class A>
struct Simplification<Multiplication<A, A>, /*mult_f_f*/ 3,
                      typename std::enable_if<is_stateless<A>::value>::type> {
  typedef Composition<IntegralPolynomial<iseq<0, 0, 1> >, A> type;

  static type Combine(Multiplication<A, A>) {
    SIMPLIFY_INFO("Simplifying multiplication of function "
                  "by itself\n");
    return { IntegralPolynomial<iseq<0, 0, 1> >(), A() };
  }
};

template <class T, class U, U us>
struct Simplification<
    Addition<T, IntegralPolynomial<std::integer_sequence<U, us> > >,
    /*add_f_ip_1*/3> {
  typedef Composition<IntegralPolynomial<std::integer_sequence<U, us, (U)1> >,
                      T> type;

  static type
  Combine(Addition<T, IntegralPolynomial<std::integer_sequence<U, us> > > a) {
    SIMPLIFY_INFO("Simplifying addition of constant "
                  "and another function");
    auto t = a.GetFunctions();
    return { IntegralPolynomial<std::integer_sequence<U, us, (U)1> >(),
             get<0>(t) };
  }
};

template <class T, class U>
struct Simplification<
    Multiplication<T, IntegralPolynomial<std::integer_sequence<U, (U)1> > >,
    /*mult_f_ip_1*/1> {
  static T Combine(Multiplication<
      T, IntegralPolynomial<std::integer_sequence<U, (U)1> > > m) {
    SIMPLIFY_INFO("Simplifying multiplication of function by 1");
    return get<0>(m.GetFunctions());
  }
};

template <class T>
struct Simplification<IntegralPolynomial<std::integer_sequence<T, (T)0> >,
                      /*ip_0*/ 0> {
  static auto Combine(IntegralPolynomial<std::integer_sequence<T, (T)0> >) {
    return zero;
  }
};
}

#endif
