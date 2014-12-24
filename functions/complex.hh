#ifndef MANIFOLDS_FUNCTIONS_COMPLEX_HH
#define MANIFOLDS_FUNCTIONS_COMPLEX_HH

#include <utility>
#include <complex>
#include "tmp_utils.hh"
#include "function.hh"
#include "full_function_defs.hh"

namespace manifolds {

    struct Real : Function<int_<27>, 1, 1>,
                  FunctionCommon<Real>
    {
        using FunctionCommon<Real>::operator();
        static const bool stateless = true;
        static const ComplexOutputBehaviour complex_output =
            NeverComplex;

        template <class T>
        auto eval(T t) const
        {
            using std::real;
            return real(t);
        }

        template <class T>
        bool operator==(const T &) const
        {
            return std::is_same<Real,T>::value;
        }
    };

    struct Imag : Function<int_<28>, 1, 1>,
                  FunctionCommon<Imag>
    {
        using FunctionCommon<Imag>::operator();
        static const bool stateless = true;
        static const ComplexOutputBehaviour complex_output =
            NeverComplex;

        template <class T>
        auto eval(T t) const
        {
            using std::imag;
            return imag(t);
        }

        template <class T>
        bool operator==(const T &) const
        {
            return std::is_same<Imag,T>::value;
        }
    };

    struct Phase : Function<int_<28>, 1, 1>,
                   FunctionCommon<Phase>
    {
        using FunctionCommon<Phase>::operator();
        static const bool stateless = true;
        static const ComplexOutputBehaviour complex_output =
            NeverComplex;
        template <class T>
        auto eval(T t) const
        {
            using std::arg;
            return arg(t);
        }

        template <class T>
        bool operator==(const T &) const
        {
            return std::is_same<Phase,T>::value;
        }
    };

    using Arg = Phase;

    struct Norm : Function<int_<29>, 1, 1>,
                  FunctionCommon<Norm>
    {
        using FunctionCommon<Norm>::operator();
        static const bool stateless = true;
        static const ComplexOutputBehaviour complex_output =
            NeverComplex;

        template <class T>
        auto eval(T t) const -> decltype(norm(t))
        {
            return norm(t);
        }

        template <class T>
        bool operator==(const T &) const
        {
            return std::is_same<Norm,T>::value;
        }

        template <class T,
                  class = typename std::enable_if<
                      !is_complex<T>::value>::type>
        auto eval(T t) const
        {
            return t * t;
        }
    };

    template <class Complex>
    struct ImagN : Function<int_<30>, 0, 1>,
        FunctionCommon<ImagN<Complex>>
    {
        using FunctionCommon<ImagN>::operator();
        static const bool stateless = true;
        static const ComplexOutputBehaviour complex_output =
            NeverComplex;

        template <class...Args>
        auto eval(Args...) const
        {
            return Complex(0,1);
        }

        template <class T>
        bool operator==(ImagN<T>) const
        {
            return true;
        }

        template <class T>
        bool operator==(const T &) const
        {
            return false;
        }
    };

    struct Sign : Function<int_<31>, 1, 1>,
                  FunctionCommon<Sign>
    {
        using FunctionCommon<Sign>::operator();
        static const bool stateless = true;
        //The sign of a complex number isn't even defined
        static const ComplexOutputBehaviour complex_output = NeverComplex;

        template <class T>
        auto eval(T t) const
        {
            return std::copysign(t, 1);
        }

        template <class T>
        bool operator==(T) const
        {
            return std::is_same<T, Sign>::value;
        }
    };

    struct Conjugate : Function<int_<32>, 1, 1>,
                       FunctionCommon<Conjugate>
    {
        using FunctionCommon<Conjugate>::operator();
        static const bool stateless = true;
        static const ComplexOutputBehaviour complex_output = SameAsInput;

        template <class T>
        auto eval(T t) const
        {
            return T(real(t), -imag(t));
        }

        template <class T>
        bool operator==(T) const
        {
            return std::is_same<Conjugate, T>::value;
        }
    };

    static const ImagN<std::complex<double>> I =
        ImagN<std::complex<double>>();

}

#endif
