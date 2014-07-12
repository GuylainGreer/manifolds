#include <boost/test/unit_test.hpp>
#include "derivatives.hh"

BOOST_AUTO_TEST_CASE(derivative_test)
{
  using namespace manifolds;
  BOOST_CHECK_EQUAL(Derivative(Sin()), Cos());
  auto f = Cos()(x*x);
  auto d = Derivative(f);
  auto check = -2_c * x * Sin()(x * x);
  BOOST_CHECK_EQUAL(d(2), check(2));
}
