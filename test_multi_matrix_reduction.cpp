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
}
