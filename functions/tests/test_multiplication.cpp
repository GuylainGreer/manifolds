#include <boost/test/unit_test.hpp>
#include "functions/multiplication.hh"
#include "functions/full_function_defs.hh"
#include "functions/variables.hh"
#include "functions/polynomial.hh"
#include "functions/addition.hh"
#include "functions/operators.hh"
#include "functions/streaming2.hh"

BOOST_AUTO_TEST_CASE(multiplication_test)
{
  using namespace manifolds;
  Multiplication<decltype(x), decltype(z)> a(x,z);

  BOOST_CHECK_EQUAL(a(1,2,3,4), 3);
  BOOST_CHECK_EQUAL(a(1,2,4,8,16), 4);

  auto f = Cos()(x * x) * y;
  BOOST_CHECK_EQUAL(f(2,1), std::cos(4.0));
  auto f2 = -2_c * x * Sin()(x * x) * y;
  BOOST_CHECK_EQUAL((-2_c)(1), -2);
  BOOST_CHECK_EQUAL((-2_c * x  * y)(2, 1), -4);
  BOOST_CHECK_EQUAL((Sin()(x*x))(2), std::sin(4));
  BOOST_CHECK_EQUAL((-2_c * Sin())(2), -2 * std::sin(2));
  //BOOST_CHECK_EQUAL(f2(2,1), -4 * std::sin(4));
}
