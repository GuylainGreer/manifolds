#include <boost/test/unit_test.hpp>
#include "matrix.hh"
#include "function_matrix.hh"
#include "trig.hh"
#include "operators.hh"
#include "transpose.hh"
#include "streaming2.hh"

BOOST_AUTO_TEST_CASE(matrix_test)
{
  using namespace manifolds;
  auto m1 = GetMatrix<2,2>(1,2,3,4);

  BOOST_CHECK_EQUAL(m1.Coeff(0,0), 1);
  BOOST_CHECK_EQUAL(m1.Coeff(0,1), 2);
  BOOST_CHECK_EQUAL(m1.Coeff(1,0), 3);
  BOOST_CHECK_EQUAL(m1.Coeff(1,1), 4);

  auto m2 = m1 + m1;

  BOOST_CHECK_EQUAL(m2.Coeff(0,0), 2);
  BOOST_CHECK_EQUAL(m2.Coeff(0,1), 4);
  BOOST_CHECK_EQUAL(m2.Coeff(1,0), 6);
  BOOST_CHECK_EQUAL(m2.Coeff(1,1), 8);

  auto m3 = m2 * m1;
  BOOST_CHECK_EQUAL(m3.Coeff(0,0), 14);
  BOOST_CHECK_EQUAL(m3.Coeff(0,1), 20);
  BOOST_CHECK_EQUAL(m3.Coeff(1,0), 30);
  BOOST_CHECK_EQUAL(m3.Coeff(1,1), 44);

  auto mf = GetFunctionMatrix(Row(Cos(), -Sin()),
			      Row(Sin(), Cos()));
  BOOST_CHECK_EQUAL(mf(3),
		    (GetMatrix<2,2>(std::cos(3),
			           -std::sin(3),
				    std::sin(3),
				    std::cos(3))));

  auto mf2 = transpose(mf) * mf;

  std::cout << mf2 << "\n\n";
  Stream2(std::cout, mf2) << "\n\n";

  BOOST_CHECK_EQUAL(mf2(4).Coeff(0,0), 1.0);
  BOOST_CHECK_EQUAL(mf2(4).Coeff(0,1), 0.0);
  BOOST_CHECK_EQUAL(mf2(4).Coeff(1,0), 0.0);
  BOOST_CHECK_EQUAL(mf2(4).Coeff(1,1), 1.0);
}
