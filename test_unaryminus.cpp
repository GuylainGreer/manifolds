#include <boost/test/unit_test.hpp>
#include "unary_minus.hh"
#include "full_function_defs.hh"
#include "variables.hh"

BOOST_AUTO_TEST_CASE(unary_minus_test)
{
  using namespace manifolds;
  UnaryMinus<decltype(x)> a(x);

  BOOST_CHECK_EQUAL(a(1,2,3,4), -2);
  BOOST_CHECK_EQUAL(a(1,2,4,8,16), -2);
}
