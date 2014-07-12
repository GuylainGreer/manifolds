#include "polynomial.hh"
#include "full_function_defs.hh"
#include "operators.hh"
#include <boost/test/unit_test.hpp>
#include <complex>
#include "trig.hh"

BOOST_AUTO_TEST_CASE(polynomial_test)
{
  using namespace manifolds;

  auto p = GetPolynomial(1, 0.5);
  BOOST_CHECK_EQUAL(p(0), 1);
  BOOST_CHECK_EQUAL(p(1), 1.5);
  BOOST_CHECK_EQUAL(p(2), 2);

  auto p2 = GetPolynomial(2,5,6);
  auto p3 = p + p2;
  static_assert(std::is_same<
		decltype(p3),
		Polynomial<
		double,
		int_<3>>>::value,
		"Failed to simplify adding two polynomials!");
  BOOST_CHECK_EQUAL(p3(0), 3);
  BOOST_CHECK_EQUAL(p3(1), 14.5);
  BOOST_CHECK_EQUAL(p3(-1), 3.5);

  auto p4 = p * p2;
  static_assert(std::is_same<
		decltype(p4),
		Polynomial<
		double,
		int_<4>>>::value,
		"Failed to simplify multiplying"
		" two polynomials!");
  BOOST_CHECK_EQUAL(p4(0), p(0)*p2(0));
  BOOST_CHECK_EQUAL(p4(1), p(1)*p2(1));
  BOOST_CHECK_EQUAL(p4(2), p(2)*p2(2));

  auto p5 = p3(p4);

  static_assert(std::is_same<
		decltype(p5),
		Polynomial<
		double,
		int_<7>>>::value,
		"Failed to simplify polynomial composition");
  auto p6 = Composition<decltype(p3), decltype(p4)>(p3,p4);
  for(int i = -3; i < 4; i++)
    {
      BOOST_CHECK_EQUAL(p5(i), p6(i));
    }

  std::complex<double> c{1,2};
  auto pc = GetPolynomial(c, 2.0 * c, c * c);

  BOOST_CHECK_EQUAL(pc(0), c);
  using namespace std::literals;
  BOOST_CHECK_EQUAL(pc(1i), 0i);

  auto ps = manifolds::sin + manifolds::sin + manifolds::sin;

  BOOST_CHECK_EQUAL(ps(2), 3 * manifolds::sin(2));

  ps = manifolds::sin + (manifolds::sin+manifolds::sin);
  //The fact that the line above compiles means that
  //the reverse generic simplification worked and gave
  //the expected type.

  BOOST_CHECK_EQUAL(ps(2), 3 * manifolds::sin(2));
}
