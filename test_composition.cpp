#include <boost/test/unit_test.hpp>
#include "composition.hh"
#include "variables.hh"
#include "trig.hh"

BOOST_AUTO_TEST_CASE(composition_test)
{
  using namespace manifolds;
  Composition<Sin, decltype(z)> a;

  BOOST_CHECK_EQUAL(a(1,2,3,4), std::sin(4));
  BOOST_CHECK_EQUAL(a(1,2,4,8,16), std::sin(8));
}
