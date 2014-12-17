#ifndef MANIFOLDS_FUNCTIONS_COMPLEX_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_COMPLEX_SIMPLIFICATIONS_HH

#include "simplify.hh"
#include "complex.hh"
#include "variables.hh"
#include "zero.hh"
#include "multiplication.hh"

namespace manifolds
{
  template <class T>
  struct Simplification<
    Composition<Real, ImagN<T>>, 0>
  {
    static auto Combine(Composition<Real, ImagN<T>>)
    {
      SIMPLIFY_INFO("Simplifying Real part of "
		    "non-complex function");
      return zero;
    }
  };

    template <class T>
    struct Simplification<
        Composition<Imag, ImagN<T>>, 0>
    {
        static auto Combine(Composition<Imag, ImagN<T>>)
        {
            return GetIPolynomial<1>();
        }
    };

    template <class T>
    struct Simplification<
        Composition<Real, T>, 1,
        typename std::enable_if<
            ComplexOutputType<T>::value == NeverComplex
            >::type>
    {
        static auto Combine(Composition<Real, T> t)
        {
            return get<1>(t.GetFunctions());
        }
    };

    template <class T>
    struct Simplification<
        Composition<Imag, T>, 1,
        typename std::enable_if<
            ComplexOutputType<T>::value == NeverComplex
            >::type>
    {
        static auto Combine(Composition<Imag, T> t)
        {
            return zero;
        }
    };

    template <class T>
    struct Simplification<
        Composition<Phase, T>, 1,
        typename std::enable_if<
            ComplexOutputType<T>::value == NeverComplex
            >::type>
    {
        static auto Combine(Composition<Phase, T> t)
        {
            auto t2 = get<1>(t.GetFunctions());
            return MultiplyRaw(Sign()(t2), GetPolynomial(M_PI));
        }
    };

    template <class T>
    struct Simplification<
        Composition<Phase, ImagN<T>>, 0>
    {
        static auto Combine(Composition<Phase, ImagN<T>>)
        {
            return GetPolynomial(M_PI);
        }
    };
}

#endif
