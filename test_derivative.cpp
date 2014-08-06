#include <boost/test/unit_test.hpp>
#include "derivatives.hh"

BOOST_AUTO_TEST_CASE(derivative_test)
{
  using namespace manifolds;
  BOOST_CHECK_EQUAL(Derivative(Sin()), Cos());
  auto f = Cos()(x*x) * y;
  auto d = Derivative(f);
  auto check = -2_c * x * Sin()(x * x) * y;
  BOOST_CHECK_EQUAL(d(2, 1), check(2, 1));

  D<2> d2;
  auto ddf = d2(f);
  auto ddf2 = Derivative(d);
  BOOST_CHECK_EQUAL(ddf(1, 1), ddf2(1, 1));
  BOOST_CHECK_EQUAL(ddf(2, 1), ddf2(2, 1));
  BOOST_CHECK_EQUAL(ddf(4, 1), ddf2(4, 1));
  BOOST_CHECK_EQUAL(ddf(7, 1), ddf2(7, 1));
  auto fd = FullDerivative(f);
  auto fd_check =
    GetFunctionMatrix(GetRow(d, Cos()(x*x)));
  BOOST_CHECK_EQUAL(fd(5,6), fd_check(5,6));
  BOOST_CHECK_EQUAL(fd(7,7), fd_check(7,7));
}
