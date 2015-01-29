#ifndef MANIFOLDS_FUNCTIONS_COMPLEX_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_COMPLEX_SIMPLIFICATIONS_HH

#include "simplify.hh"
#include "complex.hh"
#include "variables.hh"
#include "zero.hh"
#include "multiplication.hh"
#include "integral_polynomial.hh"

namespace manifolds {
template <class T>
struct Simplification<Composition<Real, ImagN<T> >, /*com_re_i*/ 0> {
  static auto Combine(Composition<Real, ImagN<T> >) {
    SIMPLIFY_INFO("Simplifying Real part of "
                  "non-complex function");
    return zero;
  }
};

template <class T>
struct Simplification<Composition<Imag, ImagN<T> >, /*com_im_i*/ 0> {
  static auto Combine(Composition<Imag, ImagN<T> >) {
    return GetIPolynomial<1>();
  }
};

template <class T>
struct Simplification<Composition<Real, T>, /*com_re_f*/ 1,
                      typename std::enable_if<ComplexOutputType<T>::value ==
                                              NeverComplex>::type> {
  static auto Combine(Composition<Real, T> t) {
    return get<1>(t.GetFunctions());
  }
};

template <class T>
struct Simplification<Composition<Imag, T>, /*com_im_f*/ 1,
                      typename std::enable_if<ComplexOutputType<T>::value ==
                                              NeverComplex>::type> {
  static auto Combine(Composition<Imag, T> t) { return zero; }
};

template <class T>
struct Simplification<Composition<Phase, T>, /*com_ph_f*/ 1,
                      typename std::enable_if<ComplexOutputType<T>::value ==
                                              NeverComplex>::type> {
  static auto Combine(Composition<Phase, T> t) {
    auto t2 = get<1>(t.GetFunctions());
    return GetPolynomial(-M_PI/2, M_PI/2)(Sign()(t2));
  }
};

template <class T>
struct Simplification<Composition<Phase, ImagN<T> >, /*com_ph_i*/ 0> {
  static auto Combine(Composition<Phase, ImagN<T> >) {
    return GetPolynomial(M_PI);
  }
};

template <class T>
struct Simplification<Composition<Norm, ImagN<T> >, /*com_nm_i*/ 0> {
  static auto Combine(Composition<Norm, ImagN<T> >) {
    return GetIPolynomial<1>();
  }
};

template <class T>
struct Simplification<Composition<Norm, T>, /*com_nm_f*/ 1,
                      typename std::enable_if<ComplexOutputType<T>::value ==
                                              NeverComplex>::type> {
  static auto Combine(Composition<Norm, T> t) {
    return GetIPolynomial<0, 0, 1>()(get<1>(t.GetFunctions()));
  }
};

template <class T>
struct Simplification<Composition<Conjugate, ImagN<T> >, /*com_con_i*/ 0> {
  static auto Combine(Composition<Conjugate, ImagN<T> >) {
    return UnaryMinus<ImagN<T> >();
  }
};

template <class T>
struct Simplification<Composition<Conjugate, T>, /*com_con_f*/ 1,
                      typename std::enable_if<ComplexOutputType<T>::value ==
                                              NeverComplex>::type> {
  static auto Combine(Composition<Conjugate, T> t) {
    return get<1>(t.GetFunctions());
  }
};

template <class T, class T2, T2... cs>
struct Simplification<
    Composition<IntegralPolynomial<std::integer_sequence<T2, cs...> >,
                ImagN<T> >,
    /**/ 0, typename std::enable_if<(sizeof...(cs) > 2)>::type> {

  typedef typename std::conditional<std::is_unsigned<T2>::value,
                                    typename std::make_signed<T2>::type,
                                    T2>::type Tout;

  template <Tout c0, Tout c1, Tout... c_outs>
  static auto Add(
      int_<1>,
      IntegralPolynomial<std::integer_sequence<Tout, c0, c1, c_outs...> > p) {
    return AddRaw(
        IntegralPolynomial<std::integer_sequence<Tout, c0> >(),
        MultiplyRaw(IntegralPolynomial<std::integer_sequence<Tout, c1> >(), I));
  }

  template <Tout c0, Tout c1, Tout... c_outs, int N>
  static auto Add(
      int_<N>,
      IntegralPolynomial<std::integer_sequence<Tout, c0, c1, c_outs...> >) {
    static const Tout c0_2 =
        c0 +
        ((N % 2 == 0)
             ? ((N % 4 == 0)
                    ? nth<N - 2,
                          std::integral_constant<Tout, c_outs>...>::type::value
                    : -nth<N - 2, std::integral_constant<
                                      Tout, c_outs>...>::type::value)
             : 0);
    static const Tout c1_2 =
        c1 +
        ((N % 2 == 1)
             ? ((N % 4 == 1)
                    ? nth<N - 2,
                          std::integral_constant<Tout, c_outs>...>::type::value
                    : -nth<N - 2, std::integral_constant<
                                      Tout, c_outs>...>::type::value)
             : 0);
    return Add(int_<N - 1>(),
               IntegralPolynomial<
                   std::integer_sequence<Tout, c0_2, c1_2, c_outs...> >());
  }

  static auto Combine(Composition<
      IntegralPolynomial<std::integer_sequence<T2, cs...> >, ImagN<T> >) {
    return Add(int_<sizeof...(cs) - 1>(),
               IntegralPolynomial<std::integer_sequence<Tout, (Tout)cs...> >());
  }
};

template <class... C, class Complex>
struct Simplification<Composition<ImagN<Complex>, C...>, /*com_i_cs*/ 0> {
  static auto Combine(Composition<ImagN<Complex>, C...>) { return I; }
};

    template <class F, class ... Fs>
    struct Simplification<
        Composition<
            F, Addition<Fs...> >, /*re_add_fs*/0,
        typename std::enable_if<
            std::is_same<F, Real>::value ||
            std::is_same<F, Imag>::value
            >::type>
    {
        template <std::size_t ... is>
        static auto Combine(Composition<
                                F, Addition<Fs...> > c,
                            std::integer_sequence<
                                std::size_t, is...>)
        {
            auto t = get<1>(c.GetFunctions()).GetFunctions();
            return AddRaw((F()(get<is>(t)))...);
        }

        static auto Combine(Composition<
                                F, Addition<Fs...> > c)
        {
            return Combine(c, std::index_sequence_for<Fs...>());
        }
    };

    template <class F1, class F2>
    struct Simplification<
        Composition<
            Real, Multiplication<F1, F2> >, /*re_add_fs*/0>
    {
        static auto Combine(Composition<
                                Real, Multiplication<F1,F2> > c)
        {
            auto t = get<1>(c.GetFunctions()).GetFunctions();
            auto left = get<0>(t);
            auto right = get<1>(t);
            return SubRaw(MultiplyRaw(Real()(left), Real()(right)),
                          MultiplyRaw(Imag()(left), Imag()(right)));
        }
    };

    template <class F1, class F2>
    struct Simplification<
        Composition<
            Imag, Multiplication<F1, F2> >, /*im_add_fs*/0>
    {
        static auto Combine(Composition<
                                Imag, Multiplication<F1, F2> > c)
        {
            auto t = get<1>(c.GetFunctions()).GetFunctions();
            auto left = get<0>(t);
            auto right = get<1>(t);
            return AddRaw(MultiplyRaw(Real()(left), Imag()(right)),
                          MultiplyRaw(Imag()(left), Real()(right)));
        }
    };
}

#endif
