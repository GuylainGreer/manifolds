#include <boost/test/unit_test.hpp>
#include "multi_matrix_reduction.hh"
#include <boost/mpl/distance.hpp>

BOOST_AUTO_TEST_CASE(multi_matrix_reduction_test)
{
  using namespace manifolds;
  Reduction<ReduxPair<0,1>> r;
  MultiMatrix<double,3,3,1> m(0,1,2,
			      0,1,4,
			      1,2,9);
  BOOST_CHECK_EQUAL(r(m).Coeff(0), 10);
  Reduction<ReduxPair<1,2>> r2;
  MultiMatrix<double,1,3> m2(0,1,2);
  MultiMatrix<double,3,1> m3(0,1,2);
  auto m4 = m2*m3;
  static_assert(decltype(m4)::dimension<0>::value == 1,
		"Error conglomerating MultiMatrices' dimensions");
  static_assert(decltype(m4)::dimension<1>::value == 3,
		"Error conglomerating MultiMatrices' dimensions");
  static_assert(decltype(m4)::dimension<2>::value == 3,
		"Error conglomerating MultiMatrices' dimensions");
  static_assert(decltype(m4)::dimension<3>::value == 1,
		"Error conglomerating MultiMatrices' dimensions");
  BOOST_CHECK_EQUAL(r2(m4).Coeff(0, 0), 5);
  auto m5 = r2(m3*m2);
  BOOST_CHECK_EQUAL(m5.Coeff(0,0), 0);
  BOOST_CHECK_EQUAL(m5.Coeff(0,1), 0);
  BOOST_CHECK_EQUAL(m5.Coeff(0,2), 0);
  BOOST_CHECK_EQUAL(m5.Coeff(1,0), 0);
  BOOST_CHECK_EQUAL(m5.Coeff(1,1), 1);
  BOOST_CHECK_EQUAL(m5.Coeff(1,2), 2);
  BOOST_CHECK_EQUAL(m5.Coeff(2,0), 0);
  BOOST_CHECK_EQUAL(m5.Coeff(2,1), 2);
  BOOST_CHECK_EQUAL(m5.Coeff(2,2), 4);
}
