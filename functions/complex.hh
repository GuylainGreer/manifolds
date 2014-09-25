#ifndef MANIFOLDS_FUNCTIONS_COMPLEX_HH
#define MANIFOLDS_FUNCTIONS_COMPLEX_HH

#include <utility>
#include <complex>
#include "tmp_utils.hh"
#include "function.hh"

namespace manifolds {

  using std::real;
  using std::imag;
  using std::arg;

  struct Real : Function<int_<27>, 1, 1>
  {
    static const bool stateless = true;
    static const ComplexOutputBehaviour complex_output =
      NeverComplex;

    template <class T>
    auto operator()(T t) const
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

  struct Imag : Function<int_<28>, 1, 1>
  {
    static const bool stateless = true;
    static const ComplexOutputBehaviour complex_output =
      NeverComplex;

    template <class T>
    auto operator()(T t) const
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

  struct Phase : Function<int_<28>, 1, 1>
  {
    static const bool stateless = true;
    static const ComplexOutputBehaviour complex_output =
      NeverComplex;
    template <class T>
    auto operator()(T t) const
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

  struct Norm : Function<int_<29>, 1, 1>
  {
    static const bool stateless = true;
    static const ComplexOutputBehaviour complex_output =
      NeverComplex;

    template <class T>
    auto operator()(T t) const -> decltype(norm(t))
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
    auto operator()(T t) const
    {
      auto t2 = abs(t);
      return t2 * t2;
    }
  };

  template <class Complex>
  struct ImagN : Function<int_<30>, 0, 1>
  {
    static const bool stateless = true;
    static const ComplexOutputBehaviour complex_output =
      NeverComplex;

    template <class...Args>
    auto operator()(Args...) const
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

  static const ImagN<std::complex<double>> i =
    ImagN<std::complex<double>>();

}

#endif
