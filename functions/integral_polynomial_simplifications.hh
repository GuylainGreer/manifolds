#ifndef MANIFOLDS_FUNCTIONS_INTEGRAL_POLYNOMIAL_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_INTEGRAL_POLYNOMIAL_SIMPLIFICATIONS_HH

#include "integral_polynomial.hh"
#include "polynomial.hh"
#include "addition.hh"
#include "simplify.hh"
#include "unary_minus.hh"
#include "zero.hh"

namespace manifolds {
template <IPInt_t... t1s, IPInt_t... t2s>
struct Simplification<
    Addition<IntegralPolynomial<t1s...>, IntegralPolynomial<t2s...> >,
    /*add_ip_ip*/ 0> {
  template <class, class> struct Extended;

  template <IPInt_t... indices, IPInt_t... padding>
  struct Extended<std::integer_sequence<IPInt_t, indices...>,
                  std::integer_sequence<IPInt_t, padding...> > {
    typedef std::integer_sequence<IPInt_t, indices..., (padding - padding)...>
    type;
  };

  static const int padding1 =
      sizeof...(t1s) < sizeof...(t2s) ? sizeof...(t2s) - sizeof...(t1s) : 0;
  typedef typename Extended<
      std::integer_sequence<IPInt_t, t1s...>,
      std::make_integer_sequence<IPInt_t, padding1> >::type padded1;

  static const int padding2 =
      sizeof...(t2s) < sizeof...(t1s) ? sizeof...(t1s) - sizeof...(t2s) : 0;
  typedef typename Extended<
      std::integer_sequence<IPInt_t, t2s...>,
      std::make_integer_sequence<IPInt_t, padding2> >::type padded2;

  template <class, class> struct Add;

  template <IPInt_t... indices1, IPInt_t... indices2>
  struct Add<std::integer_sequence<IPInt_t, indices1...>,
             std::integer_sequence<IPInt_t, indices2...> > {
    typedef IntegralPolynomial<(indices1 + indices2)...> type;
  };

  typedef typename Add<padded1, padded2>::type type;

  static type
  Combine(Addition<IntegralPolynomial<t1s...>, IntegralPolynomial<t2s...> >) {
    SIMPLIFY_INFO("Simplifying addition of "
                  "integral_polynomials\n");
    return type{};
  }
};

template <IPInt_t... t1s, IPInt_t... t2s>
struct Simplification<
    Multiplication<IntegralPolynomial<t1s...>, IntegralPolynomial<t2s...> >,
    /*mult_ip_ip*/ 0> {
  typedef IPInt_t Tr;

  typedef std::integer_sequence<IPInt_t, t1s...> inputs1;
  typedef std::integer_sequence<IPInt_t, t2s...> inputs2;

  template <int, class> struct get_value;

  template <int i, IPInt_t... ts>
  struct get_value<
      i, std::integer_sequence<
             IPInt_t, ts...> > : nth<i, std::integral_constant<IPInt_t,
                                                               ts>...>::type {};

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

  template <IPInt_t... ts>
  struct P1Loop<-1, std::integer_sequence<IPInt_t, ts...> > {
    typedef IntegralPolynomial<ts...> type;
  };

  template <class> struct InitOutput;

  template <IPInt_t... indices> struct InitOutput<iseq<indices...> > {
    typedef std::integer_sequence<Tr, ((void)indices, 0)...> type;
  };

  typedef typename P1Loop<
      sizeof...(t1s) - 1,
      typename InitOutput<
          miseq<sizeof...(t1s) + sizeof...(t2s) - 1> >::type>::type type;

  static type Combine(
      Multiplication<IntegralPolynomial<t1s...>, IntegralPolynomial<t2s...> >) {
    SIMPLIFY_INFO("Simplifying multiplication of "
                  "integral_polynomials\n");
    return type{};
  }
};

template <IPInt_t... ts>
struct Simplification<IntegralPolynomial<ts...>,
                      /*ip*/ 0> {
  template <IPInt_t t> using ic = std::integral_constant<IPInt_t, t>;

  template <IPInt_t... t2s> struct pop {
    template <class> struct apply;

    template <std::size_t... is>
    struct apply<std::integer_sequence<std::size_t, is...> > {
      typedef std::integer_sequence<IPInt_t,
                                    nth<is, ic<t2s>...>::type::value...> type;
    };

    typedef typename apply<std::make_index_sequence<sizeof...(t2s) - 1> >::type
    type;
  };

  template <class, bool> struct pop_if;

  template <class> struct stopping_condition;

  template <IPInt_t... touts>
  struct stopping_condition<std::integer_sequence<IPInt_t, touts...> > {
    static const bool value =
        sizeof...(ts) == 1 || last<ic<touts>...>::type::value != 0;
  };

  template <IPInt_t... touts>
  struct pop_if<std::integer_sequence<IPInt_t, touts...>, true> {
    typedef IntegralPolynomial<touts...> type;
  };

  template <IPInt_t... touts>
  struct pop_if<std::integer_sequence<IPInt_t, touts...>, false> {
    typedef typename pop<touts...>::type next;
    typedef typename pop_if<next, stopping_condition<next>::value>::type type;
  };

  typedef typename pop_if<
      std::integer_sequence<IPInt_t, ts...>,
      stopping_condition<std::integer_sequence<IPInt_t, ts...> >::value>::type
  type;

  static type Combine(IntegralPolynomial<ts...>) {
    SIMPLIFY_INFO("Simplifying of integral_polynomial\n");
    return type{};
  }
};

template <template <class...> class Variadic, class T1, class O1,
          IPInt_t... t2s>
struct Simplification<Variadic<Polynomial<T1, O1>, IntegralPolynomial<t2s...> >,
                      /*var_p_ip*/ 2> {
  typedef Variadic<Polynomial<T1, O1>,
                   Polynomial<IPInt_t, int_<sizeof...(t2s)> > > type;

  static type
  Combine(Variadic<Polynomial<T1, O1>, IntegralPolynomial<t2s...> > v) {
    SIMPLIFY_INFO("Simplifying variadic of "
                  "polynomial and integral_polynomial\n");
    return { std::get<0>(v.GetFunctions()),
             std::get<1>(v.GetFunctions()).ToPoly() };
  }
};

template <template <class...> class Variadic, class T1, class O1,
          IPInt_t... t2s>
struct Simplification<Variadic<IntegralPolynomial<t2s...>, Polynomial<T1, O1> >,
                      /*var_ip_p*/ 0> {
  typedef Variadic<Polynomial<IPInt_t, int_<sizeof...(t2s)> >,
                   Polynomial<T1, O1> > type;

  static type
  Combine(Variadic<IntegralPolynomial<t2s...>, Polynomial<T1, O1> > v) {
    SIMPLIFY_INFO("Simplifying variadic of "
                  "integral_polynomial and polynomial\n");
    return { std::get<0>(v.GetFunctions()).ToPoly(),
             std::get<1>(v.GetFunctions()) };
  }
};

template <IPInt_t... coeffs>
struct Simplification<UnaryMinus<IntegralPolynomial<coeffs...> >, 0> {
  typedef IntegralPolynomial<-coeffs...> type;

  static type Combine(UnaryMinus<IntegralPolynomial<coeffs...> >) {
    SIMPLIFY_INFO("Simplifying negative of integral polynomial");
    return type{};
  }
};

template <class A>
struct Simplification<Addition<A, A>, /*add_f_f*/ 3,
                      typename std::enable_if<is_stateless<A>::value>::type> {
  typedef Composition<IntegralPolynomial<0, 2>, A> type;
  static type Combine(Addition<A, A> a) {
    SIMPLIFY_INFO("Simplifying addition of function by itself\n");
    type b = { IntegralPolynomial<0, 2>(), A() };
    return b;
  }
};

template <class A>
struct Simplification<Multiplication<A, A>, /*mult_f_f*/ 3,
                      typename std::enable_if<is_stateless<A>::value>::type> {
  typedef Composition<IntegralPolynomial<0, 0, 1>, A> type;

  static type Combine(Multiplication<A, A>) {
    SIMPLIFY_INFO("Simplifying multiplication of function "
                  "by itself\n");
    return { IntegralPolynomial<0, 0, 1>(), A() };
  }
};

template <class T, IPInt_t us>
struct Simplification<Addition<T, IntegralPolynomial<us> >,
                      /*add_f_ip_1*/ 3> {
  typedef Composition<IntegralPolynomial<us, 1>, T> type;

  static type Combine(Addition<T, IntegralPolynomial<us> > a) {
    SIMPLIFY_INFO("Simplifying addition of constant "
                  "and another function");
    auto t = a.GetFunctions();
    return { IntegralPolynomial<us, 1>(), std::get<0>(t) };
  }
};

template <class T>
struct Simplification<Multiplication<T, IntegralPolynomial<1> >,
                      /*mult_f_ip_1*/ 1> {
  static T Combine(Multiplication<T, IntegralPolynomial<1> > m) {
    SIMPLIFY_INFO("Simplifying multiplication of function by 1");
    return std::get<0>(m.GetFunctions());
  }
};

template <>
struct Simplification<IntegralPolynomial<0>,
                      /*ip_0*/ 0> {
  static auto Combine(IntegralPolynomial<0>) { return zero; }
};

template <class C>
struct Simplification<Composition<IntegralPolynomial<0, 1>, C>,
                      /*com_ip01_f*/ 0> {
  static auto Combine(Composition<IntegralPolynomial<0, 1>, C> a) {
    return std::get<1>(a.GetFunctions());
  }
};

template <class C, IPInt_t i>
struct Simplification<Composition<C, IntegralPolynomial<i> >, 0> {
  static auto Combine(Composition<C, IntegralPolynomial<i> > c) {
    return GetPolynomial((std::get<0>(c.GetFunctions()))(i));
  }
};

template <class C, IPInt_t... coeffs>
struct Simplification<
    Multiplication<C, Composition<IntegralPolynomial<coeffs...>, C> >, 0,
    typename std::enable_if<C::stateless>::type> {
  static auto Combine(
      Multiplication<C, Composition<IntegralPolynomial<coeffs...>, C> >) {
    return GetIPolynomial<0, coeffs...>()(C{});
  }
};

template <class... Fs, IPInt_t... coeffs>
struct Simplification<
    Multiplication<Composition<Fs...>,
                   Composition<IntegralPolynomial<coeffs...>, Fs...> >,
    0, typename std::enable_if<Composition<Fs...>::stateless>::type> {
  static auto Combine(Multiplication<
      Composition<Fs...>, Composition<IntegralPolynomial<coeffs...>, Fs...> >) {
    return GetIPolynomial<0, coeffs...>()(Composition<Fs...>());
  }
};
}

#endif
