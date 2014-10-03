#include <boost/test/unit_test.hpp>
#include "functions/derivatives.hh"
#include "functions/streaming2.hh"

BOOST_AUTO_TEST_CASE(derivative_test)
{
  using namespace manifolds;
  BOOST_CHECK_EQUAL(Derivative(Sin()), Cos());
  auto f = Cos()(x*x) * y;
  auto d = Derivative(f);
  auto check = 2_c * -x * Sin()(x * x) * y;
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
  auto fd_check_d = Derivative(fd);
  auto fd_check_check =
    GetFunctionMatrix(GetRow(-2_c * Sin()(x*x)*y +
			     -4_c * x * x * Cos()(x*x)*y,
			     -2_c * x * Sin()(x * x)));
  std::cout << fd_check_d << "\n"
	    << fd_check_check << "\n\n";
  BOOST_CHECK_EQUAL(fd_check_d(2,2), fd_check_check(2,2));
  BOOST_CHECK_EQUAL(fd_check_d(3,4), fd_check_check(3,4));
  auto p = Pow()(x, x);
  auto pd = Derivative(p);
  auto pd_check = (Log()(x)+1_c)*Pow()(x,x);
  BOOST_CHECK_EQUAL(pd(3), pd_check(3));

  auto m = D<3>()(Log()(x));
  auto m_check =
    Derivative(Derivative(Derivative(Log()(x))));
  BOOST_CHECK_EQUAL(m, m_check);
}
