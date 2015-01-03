#include <boost/test/unit_test.hpp>
#include "functions/integral_polynomial.hh"
#include "functions/integral_polynomial_simplifications.hh"
#include "functions/streaming.hh"
#include "functions/operators.hh"
#include "functions/polynomial_simplifications.hh"
#include "pointwise_equal.hh"

BOOST_AUTO_TEST_CASE(integral_polynomial_test) {
  using namespace manifolds;
  auto ip1 = GetIPolynomial<1, -2>();
  auto ip2 = GetIPolynomial<3, 4>();
  auto p1 = GetPolynomial(1, -2);
  auto p2 = GetPolynomial(3, 4);
  // std::cout << (ip1 + ip1 * ip2) << std::endl;
  // std::cout << (p1 + p1 * p2) << std::endl;
  PointwiseEqual(ip1, p1, 2);
  PointwiseEqual(ip2, p2, 2);
  auto ip3 = ip1 + ip1 * ip2;
  auto p3 = p1 + p1 * p2;
  PointwiseEqual(ip3, p3, 4);
}
