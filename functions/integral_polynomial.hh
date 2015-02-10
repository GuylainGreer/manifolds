#ifndef MANIFOLDS_FUNCTIONS_INTEGRAL_POLYNOMIAL_HH
#define MANIFOLDS_FUNCTIONS_INTEGRAL_POLYNOMIAL_HH

#include <cstdint>
#include "polynomial.hh"

namespace manifolds {

typedef std::intmax_t IPInt_t;
template <IPInt_t... coeffs>
using iseq = std::integer_sequence<IPInt_t, coeffs...>;
template <std::size_t size>
using miseq = std::make_integer_sequence<IPInt_t, size>;

template <IPInt_t... coeffs>
struct IntegralPolynomial : Function<int_<31>, 1, 1>,
                            FunctionCommon<IntegralPolynomial<coeffs...> > {
  using FunctionCommon<IntegralPolynomial>::operator();
  static const int num_coeffs = sizeof...(coeffs);
  static const bool stateless = true;
  static const ComplexOutputBehaviour complex_output = SameAsInput;
  typedef std::integer_sequence<IPInt_t, coeffs...> coeff_sequence;

  std::array<IPInt_t, sizeof...(coeffs)> GetCoeffs() const {
    return { { coeffs... } };
  }

  template <class T, class... Ts> auto eval(T t, Ts...) const {
    static_assert(sizeof...(Ts) == 0 || sizeof...(coeffs) == 1,
                  "Can only call polynomial "
                  "with multiple arguments when "
                  "polynomial is a constant");
    typename std::common_type<IPInt_t, T>::type result = 0, t2 = t;
    IPInt_t a_coeffs[] = { coeffs... };
    std::reverse(std::begin(a_coeffs), std::end(a_coeffs));
    for (auto i = std::begin(a_coeffs); i != std::end(a_coeffs); i++) {
      result = result * t2 + *i;
    }
    return result;
  }

  Polynomial<IPInt_t, int_<sizeof...(coeffs)> > ToPoly() const {
    std::array<IPInt_t, sizeof...(coeffs)> c = { { coeffs... } };
    return { c };
  }
};

template <IPInt_t... coeffs>
struct is_polynomial<IntegralPolynomial<coeffs...> > : std::true_type {};

template <IPInt_t... coeffs> using IP = IntegralPolynomial<coeffs...>;

template <IPInt_t... ts>
bool operator==(IntegralPolynomial<ts...>, IntegralPolynomial<ts...>) {
  return true;
}

template <IPInt_t... ts, class U>
bool operator==(IntegralPolynomial<ts...>, U) {
  return false;
}
}

#endif
