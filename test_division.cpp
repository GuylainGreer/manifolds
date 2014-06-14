#include "division.hh"
#include <boost/test/unit_test.hpp>
#include "full_function_defs.hh"
#include "trig.hh"

BOOST_AUTO_TEST_CASE(reciprocal_test)
{
  using namespace manifolds;

  Division<Sin, Cos> t;
  BOOST_CHECK_EQUAL(t(2), std::sin(2)/std::cos(2));
}
