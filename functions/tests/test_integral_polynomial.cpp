#include <boost/test/unit_test.hpp>
#include "functions/integral_polynomial.hh"
#include "functions/integral_polynomial_simplifications.hh"
#include "functions/streaming2.hh"
#include "functions/operators.hh"
#include "functions/polynomial_simplifications.hh"

BOOST_AUTO_TEST_CASE(integral_polynomial_test)
{
  using namespace manifolds;
  auto ip1 = GetIPolynomial<1,-2>();
  auto ip2 = GetIPolynomial<3, 4>();
  auto p1 = GetPolynomial(1,-2);
  auto p2 = GetPolynomial(3,4);
  //std::cout << (ip1 + ip1 * ip2) << std::endl;
  //std::cout << (p1 + p1 * p2) << std::endl;
  BOOST_CHECK_EQUAL(ip1(1), p1(1));
  BOOST_CHECK_EQUAL(ip1(2), p1(2));
  BOOST_CHECK_EQUAL(ip2(4), p2(4));
  BOOST_CHECK_EQUAL(ip2(8), p2(8));
  auto ip3 = ip1 + ip1 * ip2;
  auto p3 = p1 + p1 * p2;
  BOOST_CHECK_EQUAL(ip3(1), p3(1));
  BOOST_CHECK_EQUAL(ip3(2), p3(2));
  BOOST_CHECK_EQUAL(ip3(4), p3(4));
  BOOST_CHECK_EQUAL(ip3(8), p3(8));
}
