#include <boost/test/unit_test.hpp>
#include "full_function_defs.hh"
#include "variables.hh"
#include "trig.hh"

BOOST_AUTO_TEST_CASE(full_function_test)
{
  using namespace manifolds;
  auto f = Sin()(t);

  BOOST_CHECK_EQUAL(f(3,4,5), std::sin(3));
}
