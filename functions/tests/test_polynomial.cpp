#include "functions/integral_polynomial.hh"
#include "functions/polynomial.hh"
#include "functions/full_function_defs.hh"
#include "functions/operators.hh"
#include <boost/test/unit_test.hpp>
#include <complex>
#include "functions/trig.hh"
#include "functions/all_simplifications.hh"

BOOST_AUTO_TEST_CASE(polynomial_test) {
  using namespace manifolds;

  auto p = GetPolynomial(1, 0.5);
  BOOST_CHECK_EQUAL(p(0), 1);
  BOOST_CHECK_EQUAL(p(1), 1.5);
  BOOST_CHECK_EQUAL(p(2), 2);

  auto p2 = GetPolynomial(2, 5, 6);
  auto p3 = p + p2;
  static_assert(
      std::is_same<decltype(p3), Polynomial<double, int_<3> > >::value,
      "Failed to simplify adding two polynomials!");
  BOOST_CHECK_EQUAL(p3(0), 3);
  BOOST_CHECK_EQUAL(p3(1), 14.5);
  BOOST_CHECK_EQUAL(p3(-1), 3.5);

  auto p4 = p * p2;
  static_assert(
      std::is_same<decltype(p4), Polynomial<double, int_<4> > >::value,
      "Failed to simplify multiplying"
      " two polynomials!");
  BOOST_CHECK_EQUAL(p4(0), p(0) * p2(0));
  BOOST_CHECK_EQUAL(p4(1), p(1) * p2(1));
  BOOST_CHECK_EQUAL(p4(2), p(2) * p2(2));

  auto p5 = p3(p4);

  static_assert(Simplifies<decltype(p5)>::value,
                "Failed to simplify polynomial composition");
  auto p6 = Composition<decltype(p3), decltype(p4)>(p3, p4);
  for (int i = -3; i < 4; i++) {
    BOOST_CHECK_EQUAL(p5(i), p6(i));
  }

  std::complex<double> c{ 1, 2 };
  auto pc = GetPolynomial(c, 2.0 * c, c * c);

  BOOST_CHECK_EQUAL(pc(0), c);
  using namespace std::literals;
  BOOST_CHECK_EQUAL(pc(1i), 0i);

  auto ps = manifolds::sin + manifolds::sin + manifolds::sin;

  BOOST_CHECK_EQUAL(ps(2), 3 * manifolds::sin(2));

  ps = manifolds::sin + (manifolds::sin + manifolds::sin);
  // The fact that the line above compiles means that
  // the reverse generic simplification worked and gave
  // the expected type.

  BOOST_CHECK_EQUAL(ps(2), 3 * manifolds::sin(2));

  IntegralPolynomial<std::integer_sequence<int, 1, -2, 3> > ip;
  IntegralPolynomial<std::integer_sequence<unsigned, 3, 3, 7, 4> > ip2;

  auto ip3 = ip + ip2;

  BOOST_CHECK_EQUAL(ip3(0), ip(0) + ip2(0));
  BOOST_CHECK_EQUAL(ip3(1), ip(1) + ip2(1));
  BOOST_CHECK_EQUAL(ip3(2), ip(2) + ip2(2));
  BOOST_CHECK_EQUAL(ip3(3), ip(3) + ip2(3));
  BOOST_CHECK_EQUAL(ip3(4), ip(4) + ip2(4));

  auto ip4 = ip * ip2;

  BOOST_CHECK_EQUAL(ip4(0), ip(0) * ip2(0));
  BOOST_CHECK_EQUAL(ip4(1), ip(1) * ip2(1));
  BOOST_CHECK_EQUAL(ip4(2), ip(2) * ip2(2));
  BOOST_CHECK_EQUAL(ip4(3), ip(3) * ip2(3));
  BOOST_CHECK_EQUAL(ip4(4), ip(4) * ip2(4));
  BOOST_CHECK_EQUAL(ip4(5), ip(5) * ip2(5));
  BOOST_CHECK_EQUAL(ip4(6), ip(6) * ip2(6));

  auto ip5 = ip4 + IntegralPolynomial<
                       std::integer_sequence<int, -1, -1, -1, 0, -13, -12> >();
  static_assert(decltype(ip5)::num_coeffs == 4,
                "Failed to remove extraneous coefficients");

  auto p7 = p6 * ip5;
  static_assert(!IsVariadic<Multiplication, decltype(p7)>::value,
                "Failed to simplify multiplication of polynomial "
                "and integral polynomial");
}
