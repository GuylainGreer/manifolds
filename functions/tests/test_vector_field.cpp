#include <boost/test/unit_test.hpp>
#include "functions/vector_field.hh"
#include "functions/trig.hh"
#include "functions/polynomial.hh"

BOOST_AUTO_TEST_CASE(vector_field_test)
{
  using namespace manifolds;
  auto v1 = GetVectorField(1_c, 1_c);
  auto f1 = v1(Sin()(x*y));
  auto f1_check = Cos()(x * y) * (x + y);
  BOOST_CHECK_EQUAL(f1(2, 1), f1_check(2, 1));
  BOOST_CHECK_CLOSE(f1(4, 3), f1_check(4, 3), 1E-13);
}
