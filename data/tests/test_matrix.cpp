#include <boost/test/unit_test.hpp>
#include "data/matrix.hh"
#include "functions/function_matrix.hh"
#include "functions/trig.hh"
#include "functions/operators.hh"
#include "functions/transpose.hh"
#include "functions/streaming.hh"
#include "data/matrix_operators.hh"

BOOST_AUTO_TEST_CASE(matrix_test) {
  using namespace manifolds;
  auto m1 = GetMatrix<2, 2>(1, 2, 3, 4);

  BOOST_CHECK_EQUAL(m1.Coeff(0, 0), 1);
  BOOST_CHECK_EQUAL(m1.Coeff(0, 1), 2);
  BOOST_CHECK_EQUAL(m1.Coeff(1, 0), 3);
  BOOST_CHECK_EQUAL(m1.Coeff(1, 1), 4);

  auto m2 = m1 + m1;

  BOOST_CHECK_EQUAL(m2.Coeff(0, 0), 2);
  BOOST_CHECK_EQUAL(m2.Coeff(0, 1), 4);
  BOOST_CHECK_EQUAL(m2.Coeff(1, 0), 6);
  BOOST_CHECK_EQUAL(m2.Coeff(1, 1), 8);

  auto m3 = m2 * m1;
  BOOST_CHECK_EQUAL(m3.Coeff(0, 0), 14);
  BOOST_CHECK_EQUAL(m3.Coeff(0, 1), 20);
  BOOST_CHECK_EQUAL(m3.Coeff(1, 0), 30);
  BOOST_CHECK_EQUAL(m3.Coeff(1, 1), 44);

  auto mf = GetFunctionMatrix(Row(Cos(), -Sin()), Row(Sin(), Cos()));
  BOOST_CHECK_EQUAL(mf(3), (GetMatrix<2, 2>(std::cos(3), -std::sin(3),
                                            std::sin(3), std::cos(3))));

  static_assert(is_function<decltype(mf)>::value, "Huh?");
  auto mf2 = transpose(mf) * mf;
  static_assert(decltype(mf2)::stateless, "What the dilly?");

  // std::cout << mf2 << "\n\n";
  // Stream2(std::cout, mf2) << "\n\n";

  BOOST_CHECK_EQUAL(mf2(4), (GetMatrix<2, 2>(1, 0, 0, 1)));
}
