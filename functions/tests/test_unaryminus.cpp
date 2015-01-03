#include <boost/test/unit_test.hpp>
#include "functions/unary_minus.hh"
#include "functions/full_function_defs.hh"
#include "functions/variables.hh"

BOOST_AUTO_TEST_CASE(unary_minus_test) {
  using namespace manifolds;
  UnaryMinus<decltype(x)> a(x);

  BOOST_CHECK_EQUAL(a(1, 2, 3, 4), -1);
  BOOST_CHECK_EQUAL(a(1, 2, 4, 8, 16), -1);
}
