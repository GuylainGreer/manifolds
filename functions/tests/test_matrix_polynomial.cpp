#include "functions/integral_polynomial.hh"
#include "functions/polynomial.hh"
#include "functions/operators.hh"
#include <boost/test/unit_test.hpp>
#include <complex>
#include "functions/trig.hh"
#include "functions/all_simplifications.hh"
#include "data/matrix.hh"

BOOST_AUTO_TEST_CASE(matrix_polynomial_test) {
  using namespace manifolds;

  auto m = GetMatrix<2, 2>(1.0, 0.0, 0.0, 1.0);
  auto p = GetPolynomial(m, m * 2);
  BOOST_CHECK_EQUAL(p(2), 5 * m);

  BOOST_CHECK_EQUAL(p(m), 3 * m);

  auto p2 = p * p + 2_c * p;
  auto p2_check = GetPolynomial(3 * m, 8 * m, 4 * m);
  BOOST_CHECK(p2 == p2_check);
}
