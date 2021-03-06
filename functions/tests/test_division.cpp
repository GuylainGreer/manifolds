#include "functions/division.hh"
#include <boost/test/unit_test.hpp>
#include "functions/full_function_defs.hh"
#include "functions/operators.hh"
#include "functions/std_functions.hh"
#include "pointwise_equal.hh"

BOOST_AUTO_TEST_CASE(reciprocal_test) {
  using namespace manifolds;

  Division<Sin, Cos> t;
  BOOST_CHECK_EQUAL(t(2), std::sin(2) / std::cos(2));
  auto d1 = Sin()(x) / Log()(x);
  auto d2 = Pow()(x, y) / Tan()(x);
  auto m12 = MultiplyRaw(d1, d2);
  static_assert(Simplifies<decltype(m12)>::value, "");
  auto m12s = Simplify(m12);
  PointwiseEqual(m12, m12s, pointwise_default_num_points, 1E-11, 0.1, 20);
  auto d12 = DivideRaw(d1, d2);
  static_assert(Simplifies<decltype(d12)>::value, "");
  auto d12s = Simplify(d12);
  PointwiseEqual(d12s, d12, pointwise_default_num_points, 1E-11, 0.001, 20);
}
