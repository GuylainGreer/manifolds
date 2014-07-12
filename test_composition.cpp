#include <boost/test/unit_test.hpp>
#include "composition.hh"
#include "full_function_defs.hh"
#include "trig.hh"
#include "variables.hh"

BOOST_AUTO_TEST_CASE(composition_test)
{
  using namespace manifolds;
  Composition<Sin, decltype(t)> a;
  static_assert(decltype(a)::stateless, "");
  BOOST_CHECK_EQUAL(a(1,2,3,4), std::sin(4));
  BOOST_CHECK_EQUAL(a(1,2,4,8,16), std::sin(8));

  Composition<Sin, Cos> b;
  auto c = b(t);
  BOOST_CHECK_EQUAL(c(1,2,3,4), std::sin(std::cos(4)));
}
