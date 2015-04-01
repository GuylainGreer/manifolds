#ifndef MANIFOLDS_POLYNOMIAL_HH
#define MANIFOLDS_POLYNOMIAL_HH

#include "full_function_defs.hh"
#include "function.hh"
#include <array>
#include <type_traits>
#include <ostream>
#include <iostream>
#include <algorithm>

namespace manifolds {
// Somewhat confusingly,
// order here refers to the number of coefficients
template <class CoeffType, class Order>
struct Polynomial : Function<int_<6>, 1, 1>,
                    FunctionCommon<Polynomial<CoeffType, Order> > {
  using FunctionCommon<Polynomial>::operator();
  static const int num_coeffs = Order::value;
  static const bool stateless = false;
  static const ComplexOutputBehaviour complex_output =
      is_complex<CoeffType>::value ? AlwaysComplex : SameAsInput;

  template <class Type, std::size_t N>
  Polynomial(std::array<Type, N> t)
      : coeffs(t) {}

  template <class Type, class N> Polynomial(const Polynomial<Type, N> &t) {
    std::copy(t.GetCoeffs().begin(), t.GetCoeffs().end(), coeffs.begin());
  }

    template <class T> auto eval2impl(T t, int_<1>) const {
        return coeffs[Order::value-1];
    }

    template <class T, int i> auto eval2impl(T t, int_<i>) const {
        static const int index = Order::value - i;
        return eval2impl(t, int_<i-1>{}) * t + coeffs[index];
    }

    template <class T, class... Ts> auto eval(T t, Ts...) const {
        static_assert(sizeof...(Ts) == 0 || Order::value == 1,
                      "Can only call polynomial "
                      "with multiple arguments when "
                      "polynomial is a constant");
        return eval2impl(t, int_<Order::value>{});
    }

  const auto &GetCoeffs() const { return coeffs; }

  void Print(std::ostream &s) const {
    s << "Polynomial" << Order::value << "(";
    for (int i = 0; i < Order::value; i++) {
      s << coeffs[i];
      if (i != Order::value - 1)
        s << ' ';
    }
    s << ")";
  }

  const auto &operator[](std::size_t i) const { return coeffs[i]; }

private:
  std::array<CoeffType, Order::value> coeffs;
};

template <class> struct is_polynomial : std::false_type {};

template <class C, class O>
struct is_polynomial<Polynomial<C, O> > : std::true_type {};

template <class> struct PolynomialOrder : int_<-1> {};

template <class C, class O> struct PolynomialOrder<Polynomial<C, O> > : O {};

inline Polynomial<long double, int_<1> > operator"" _c(long double x) {
  std::array<long double, 1> a{ { x } };
  return { a };
}

inline Polynomial<long long, int_<1> > operator"" _c(long long unsigned x) {
  std::array<long long, 1> a{ {(long long)x } };
  return { a };
}

template <class... InputTypes> auto GetPolynomial(InputTypes... coeffs) {
  typedef typename std::common_type<InputTypes...>::type coeff_type;
  std::array<coeff_type, sizeof...(InputTypes)> coeff_arr{ { coeff_type(
      coeffs)... } };
  return Polynomial<coeff_type, int_<sizeof...(InputTypes)> >(coeff_arr);
}

template <class T, class C, class U>
bool operator==(const Polynomial<T, C> &p1, const Polynomial<U, C> &p2) {
  return std::equal(p1.GetCoeffs().begin(), p1.GetCoeffs().end(),
                    p2.GetCoeffs().begin(), p2.GetCoeffs().end());
}
}

#endif
