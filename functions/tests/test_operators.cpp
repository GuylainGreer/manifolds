#include "functions/operators.hh"
#include <boost/test/unit_test.hpp>
#include "functions/trig.hh"

BOOST_AUTO_TEST_CASE(operators_test)
{
  using namespace manifolds;
  Sin s;
  Tan t;
  BOOST_CHECK_EQUAL(s + t, (Addition<Sin,Tan>{}));
  BOOST_CHECK_EQUAL(s - t, (Addition<Sin,UnaryMinus<Tan>>{}));
  BOOST_CHECK_EQUAL(-s, (UnaryMinus<Sin>{}));
  BOOST_CHECK_EQUAL(s * t, (Multiplication<Sin,Tan>{}));
  BOOST_CHECK_EQUAL(s / t, (Division<Sin,Tan>{}));
}
