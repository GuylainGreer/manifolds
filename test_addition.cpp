#include <boost/test/unit_test.hpp>
#include "addition.hh"
#include "variables.hh"
#include "full_function_defs.hh"
#include "trig.hh"

BOOST_AUTO_TEST_CASE(addition_test)
{
  using namespace manifolds;
  Addition<decltype(x), decltype(z)> a(x,z);

  BOOST_CHECK_EQUAL(a(1,2,3,4), 4);
  BOOST_CHECK_EQUAL(a(1,2,4,8,16), 5);
  auto l = Log()(x);
  static_assert(is_function<decltype(l)>::value, "");
}
