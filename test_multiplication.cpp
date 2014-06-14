#include <boost/test/unit_test.hpp>
#include "multiplication.hh"
#include "full_function_defs.hh"
#include "variables.hh"

BOOST_AUTO_TEST_CASE(multiplication_test)
{
  using namespace manifolds;
  Multiplication<decltype(x), decltype(z)> a(x,z);

  BOOST_CHECK_EQUAL(a(1,2,3,4), 8);
  BOOST_CHECK_EQUAL(a(1,2,4,8,16), 16);
}
