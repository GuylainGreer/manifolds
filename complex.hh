#ifndef MANIFOLDS_FUNCTIONS_COMPLEX_HH
#define MANIFOLDS_FUNCTIONS_COMPLEX_HH

#include <utility>
#include <complex>
#include "tmp_utils.hh"
#include "function.hh"

namespace manifolds {

  template <class>
  struct is_complex : std::false_type{};

  template <class Type>
  struct is_complex<std::complex<Type>> : std::true_type{};

  using std::real;
  using std::imag;
  using std::arg;

  struct Real : Function<int_<27>, 1, 1>
  {
    static const bool stateless = true;

    template <class T>
    auto operator()(T t) const
    {
      return real(t);
    }
  };

  struct Imag : Function<int_<28>, 1, 1>
  {
    static const bool stateless = true;

    template <class T>
    auto operator()(T t) const
    {
      return imag(t);
    }
  };

  struct Phase : Function<int_<28>, 1, 1>
  {
    static const bool stateless = true;

    template <class T>
    auto operator()(T t) const
    {
      return arg(t);
    }
  };

  using Arg = Phase;

  struct Norm : Function<int_<29>, 1, 1>
  {
    static const bool stateless = true;

    template <class T>
    auto operator()(T t) const -> decltype(norm(t))
    {
      return norm(t);
    }

    template <class T,
	      class = typename std::enable_if<
		is_complex<T>::value>::type>
    auto operator()(T t) const
    {
      using std::abs;
      auto t2 = abs(t);
      return t2 * t2;
    }
  };
}

#endif
