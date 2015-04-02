#include <boost/test/unit_test.hpp>
#include "functions/composition.hh"
#include "functions/full_function_defs.hh"
#include "functions/std_functions.hh"
#include "functions/variables.hh"

BOOST_AUTO_TEST_CASE(composition_test) {
  using namespace manifolds;
  Composition<Sin, decltype(t)> a;
  static_assert(is_stateless<decltype(a)>::value, "");

  BOOST_CHECK_EQUAL(a(1, 2, 3, 4), std::sin(4));
  BOOST_CHECK_EQUAL(a(1, 2, 4, 8, 16), std::sin(8));

  Composition<Sin, Cos> b;
  auto c = b(t);
  BOOST_CHECK_EQUAL(c(1, 2, 3, 4), std::sin(std::cos(4)));
}
