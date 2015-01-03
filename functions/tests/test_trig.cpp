#include <complex>
#include "functions/trig.hh"
#include "functions/full_function_defs.hh"
#include "pointwise_equal.hh"
#include <boost/test/unit_test.hpp>
#include <limits>

BOOST_AUTO_TEST_CASE(trig_tests) {
  using namespace manifolds;
  Sin s;
  Cos c;
  Tan t;

  PointwiseEqual(s, [](auto x) { return std::sin(get<0>(x)); });
  PointwiseEqual(c, [](auto x) { return std::cos(get<0>(x)); });
  PointwiseEqual(t, [](auto x) { return std::tan(get<0>(x)); });

  Sinh sh;
  Cosh ch;
  Tanh th;

  PointwiseEqual(sh, [](auto x) { return std::sinh(get<0>(x)); });
  PointwiseEqual(ch, [](auto x) { return std::cosh(get<0>(x)); });
  PointwiseEqual(th, [](auto x) { return std::tanh(get<0>(x)); });

  std::complex<double> i(0, 1);
  BOOST_CHECK_EQUAL(std::abs(s(i)), sh(1));
  BOOST_CHECK_EQUAL(std::abs(sh(i)), s(1));
  ACos ac;
  ASin as;
  ATan at;
  double min = -0.9999;
  double max = -min;
  PointwiseEqual(as, [](auto x) { return std::asin(get<0>(x)); }, 100, 0, min,
                 max);
  PointwiseEqual(ac, [](auto x) { return std::acos(get<0>(x)); }, 100, 0, min,
                 max);
  PointwiseEqual(at, [](auto x) { return std::atan(get<0>(x)); }, 100, 0, min,
                 max);

  ACosh ach;
  ASinh ash;
  ATanh ath;

  PointwiseEqual(ash, [](auto x) { return std::asinh(get<0>(x)); }, 100, 0);
  PointwiseEqual(ach, [](auto x) { return std::acosh(get<0>(x)); }, 100, 0, 1,
                 25);
  PointwiseEqual(ath, [](auto x) { return std::atanh(get<0>(x)); }, 100, 0, min,
                 max);
}
