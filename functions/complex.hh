#ifndef MANIFOLDS_FUNCTIONS_COMPLEX_HH
#define MANIFOLDS_FUNCTIONS_COMPLEX_HH

#include <utility>
#include <complex>
#include "tmp_utils.hh"
#include "function.hh"
#include "full_function_defs.hh"

namespace manifolds {

    using std::real;
    using std::imag;
    using std::arg;

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
            return real(t);
        }

        bool operator==(Real) const
        {
            return true;
        }

        template <class T>
        bool operator==(const T &) const
        {
            return false;
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
            return imag(t);
        }

        bool operator==(Imag) const
        {
            return true;
        }

        template <class T>
        bool operator==(const T &) const
        {
            return false;
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
            return arg(t);
        }

        bool operator==(Phase) const
        {
            return true;
        }

        template <class T>
        bool operator==(const T &) const
        {
            return false;
        }
    };

    using Arg = Phase;

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

        bool operator==(Norm) const
        {
            return true;
        }

        template <class T>
        bool operator==(const T &) const
        {
            return false;
        }

        template <class T,
                  class = typename std::enable_if<
                      !is_complex<T>::value>::type>
        auto eval(T t) const
        {
            auto t2 = abs(t);
            return t2 * t2;
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

    static const ImagN<std::complex<double>> I =
        ImagN<std::complex<double>>();

}

#endif
