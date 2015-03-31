#include "functions/transpose.hh"
#include <boost/test/unit_test.hpp>
#include "data/matrix.hh"

BOOST_AUTO_TEST_CASE(tranpose_test) {
  using namespace manifolds;
  auto m1 = GetRowMatrix<3, 2>(1, 2, 3, 4, 5, 6);
  auto m2 = GetColMatrix<2, 3>(1, 2, 3, 4, 5, 6);
  auto check = GetMatrix<2, 3>(1, 3, 5, 2, 4, 6);
  BOOST_CHECK_EQUAL(check, m2);
  BOOST_CHECK_EQUAL(transpose(m1), m2);
}
