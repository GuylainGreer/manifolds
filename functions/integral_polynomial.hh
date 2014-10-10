#ifndef MANIFOLDS_FUNCTIONS_INTEGRAL_POLYNOMIAL_HH
#define MANIFOLDS_FUNCTIONS_INTEGRAL_POLYNOMIAL_HH

#include <cstdint>
#include "polynomial.hh"

namespace manifolds {

  template <int ... coeffs>
  using iseq = std::integer_sequence<int, coeffs...>;

template <class C>
struct IntegralPolynomial
{
  static_assert(!std::is_same<C,C>::value,
		"Integral polynomial is only instantiable "
		"from a std::integer_sequence<...>");
};

template <class Type, Type ... coeffs>
struct IntegralPolynomial<
  std::integer_sequence<Type,coeffs...>>:
    Function<int_<31>, 1, 1>,
    FunctionCommon<IntegralPolynomial<std::integer_sequence<Type, coeffs...>>>
{
    using FunctionCommon<IntegralPolynomial>::operator();
  static const int num_coeffs = sizeof...(coeffs);
  static const bool stateless = true;
  static const ComplexOutputBehaviour complex_output =
    SameAsInput;
  typedef std::integer_sequence<
    Type, coeffs...> coeff_sequence;

  std::array<Type,sizeof...(coeffs)> GetCoeffs() const
  {
    return {{coeffs...}};
  }

  template <class T, class ... Ts>
    auto eval(T t, Ts ...) const
  {
    static_assert(sizeof...(Ts) == 0 ||
		  sizeof...(coeffs) == 1,
		  "Can only call polynomial "
		  "with multiple arguments when "
		  "polynomial is a constant");
    typename std::common_type<
      Type, T>::type result = 0, t2 = t;
    Type a_coeffs[] = {coeffs...};
    std::reverse(std::begin(a_coeffs), std::end(a_coeffs));
    for(auto i = std::begin(a_coeffs);
	i != std::end(a_coeffs); i++)
      {
	result = result * t2 + *i;
      }
    return result;
  }

  Polynomial<Type, int_<sizeof...(coeffs)>> ToPoly() const
  {
    std::array<Type, sizeof...(coeffs)> c =
      {{coeffs...}};
    return {c};
  }
};

  template <class C>
  struct is_polynomial<IntegralPolynomial<C>> :
    std::true_type{};

  template <int ... is>
  auto GetIPolynomial()
  {
    return IntegralPolynomial<iseq<is...>>();
  }

template <class T, T ... ts, class U>
bool operator==(IntegralPolynomial<
                std::integer_sequence<T,ts...>>,
                IntegralPolynomial<
                std::integer_sequence<U,(U)ts...>>)
{
    return true;
}


template <class T, T ... ts, class U>
bool operator==(IntegralPolynomial<
                std::integer_sequence<T,ts...>>,
                U)
{
    return false;
}

}

#endif
