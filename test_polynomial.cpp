#include "polynomial.hh"
#include "full_function_defs.hh"
#include <boost/test/unit_test.hpp>
#include <ratio>

template <int n, int d>
using r = std::ratio<n,d>;

BOOST_AUTO_TEST_CASE(polynomial_test)
{
  using namespace manifolds;

  auto p = GetPolynomial<double>(1, 0.5);
  BOOST_CHECK_EQUAL(p(0), 1);
  BOOST_CHECK_EQUAL(p(1), 1.5);
  BOOST_CHECK_EQUAL(p(2), 2);

  auto p2 = GetPolynomial<double>(2,5,6);
  auto p3 = Simplify<Addition<decltype(p),
			      decltype(p2)>>::Combine(p,p2);
  BOOST_CHECK_EQUAL(p3(0), 3);
  BOOST_CHECK_EQUAL(p3(1), 14.5);
  BOOST_CHECK_EQUAL(p3(-1), 3.5);
}

