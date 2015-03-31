#include <boost/test/unit_test.hpp>
#include "data/multi_matrix.hh"

BOOST_AUTO_TEST_CASE(MultiMatrixTest) {
  using namespace manifolds;

  MultiMatrix<double, 3, 3, 2> m(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
                                 14, 15, 16, 17);

  BOOST_CHECK_EQUAL(m.Coeff(2, 1, 0), 14);
  BOOST_CHECK_EQUAL(m.Coeff(0, 1, 0), 2);
  BOOST_CHECK_EQUAL(m.Coeff(2, 2, 1), 17);

  MultiMatrixGroup<decltype(m), decltype(m)> mg(m, m);
  BOOST_CHECK_EQUAL(mg.Coeff(0, 0, 1, 0, 1, 0), 2);
  BOOST_CHECK_EQUAL(mg.Coeff(1, 0, 1, 2, 1, 0), 7 * 14);
  BOOST_CHECK_EQUAL(mg.Coeff(2, 2, 1, 0, 0, 1), 17);
  BOOST_CHECK_EQUAL(mg.Coeff(0, 2, 0, 0, 2, 0), 16);

  auto mg2 = m * m * m * (m * (m * m));
  BOOST_CHECK_EQUAL(
      mg2.Coeff(0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1), 1);
}
