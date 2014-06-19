#include "polynomial.hh"
#include "full_function_defs.hh"
#include "operators.hh"
#include <boost/test/unit_test.hpp>
#include <ratio>

template <int n, int d>
using r = std::ratio<n,d>;

BOOST_AUTO_TEST_CASE(polynomial_test)
{
  using namespace manifolds;

  auto p = GetPolynomial<double>(1, 0.5);
  BOOST_CHECK_EQUAL(p(0), 1);
  BOOST_CHECK_EQUAL(p(1), 1.5);
  BOOST_CHECK_EQUAL(p(2), 2);

  auto p2 = GetPolynomial<double>(2,5,6);
  auto p3 = p + p2;
  static_assert(std::is_same<
		decltype(p3),
		Polynomial<double,double,
		std::integral_constant<int,3>>>::value,
		"Failed to simplify adding two polynomials!");
  BOOST_CHECK_EQUAL(p3(0), 3);
  BOOST_CHECK_EQUAL(p3(1), 14.5);
  BOOST_CHECK_EQUAL(p3(-1), 3.5);

  auto p4 = p * p2;
  static_assert(std::is_same<
		decltype(p4),
		Polynomial<double,double,
		std::integral_constant<int,4>>>::value,
		"Failed to simplify multiplying"
		" two polynomials!");
  BOOST_CHECK_EQUAL(p4(0), p(0)*p2(0));
  BOOST_CHECK_EQUAL(p4(1), p(1)*p2(1));
  BOOST_CHECK_EQUAL(p4(2), p(2)*p2(2));
}

