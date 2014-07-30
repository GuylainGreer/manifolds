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

  D<2> d2;
  auto ddf = d2(f);
  auto ddf2 = Derivative(d);
  BOOST_CHECK_EQUAL(ddf(1), ddf2(1));
  std::cout << f << "\n\n"
	    << Derivative(f) << "\n\n"
	    << Derivative(d) << "\n\n";
}
