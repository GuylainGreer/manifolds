#include "operators.hh"
#include <boost/test/unit_test.hpp>
#include "trig.hh"

BOOST_AUTO_TEST_CASE(operators_test)
{
  using namespace manifolds;
  Sin s;
  Tan t;
  BOOST_CHECK_EQUAL((s+t)(2.5), std::sin(2.5)+std::tan(2.5));
  BOOST_CHECK_EQUAL((s-t)(2.5), std::sin(2.5)-std::tan(2.5));
  BOOST_CHECK_EQUAL((-s)(2.5), -std::sin(2.5));
  BOOST_CHECK_EQUAL((s*t)(2.5), std::sin(2.5)*std::tan(2.5));
  BOOST_CHECK_EQUAL((s/t)(2.5), std::sin(2.5)/std::tan(2.5));
}
