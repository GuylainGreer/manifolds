#include "polynomial.hh"
#include <boost/test/unit_test.hpp>
#include <ratio>

template <int n, int d>
using r = std::ratio<n,d>;

BOOST_AUTO_TEST_CASE(polynomial_test)
{
  using namespace manifolds;

  Polynomial<double, double, r<1,1>, r<1,2>> p;
  BOOST_CHECK_EQUAL(p(0), 1);
  BOOST_CHECK_EQUAL(p(1), 1.5);
  BOOST_CHECK_EQUAL(p(2), 2);
}

