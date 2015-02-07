#include <complex>
#include "functions/trig.hh"
#include "functions/full_function_defs.hh"
#include "pointwise_equal.hh"
#include "functions/all_simplifications.hh"
#include <boost/test/unit_test.hpp>
#include "functions/operators.hh"
#include <limits>

BOOST_AUTO_TEST_CASE(trig_tests) {
  using namespace manifolds;
  Sin s;
  Cos c;
  Tan t;

  PointwiseEqual(s, [](auto x) { return std::sin(std::get<0>(x)); });
  PointwiseEqual(c, [](auto x) { return std::cos(std::get<0>(x)); });
  PointwiseEqual(t, [](auto x) { return std::tan(std::get<0>(x)); });

  Sinh sh;
  Cosh ch;
  Tanh th;

  PointwiseEqual(sh, [](auto x) { return std::sinh(std::get<0>(x)); });
  PointwiseEqual(ch, [](auto x) { return std::cosh(std::get<0>(x)); });
  PointwiseEqual(th, [](auto x) { return std::tanh(std::get<0>(x)); });

  std::complex<double> i(0, 1);
  BOOST_CHECK_EQUAL(std::abs(s(i)), sh(1));
  BOOST_CHECK_EQUAL(std::abs(sh(i)), s(1));
  ACos ac;
  ASin as;
  ATan at;
  double min = -0.9999;
  double max = -min;
  PointwiseEqual(as, [](auto x) { return std::asin(std::get<0>(x)); }, 100, 0,
                 min, max);
  PointwiseEqual(ac, [](auto x) { return std::acos(std::get<0>(x)); }, 100, 0,
                 min, max);
  PointwiseEqual(at, [](auto x) { return std::atan(std::get<0>(x)); }, 100, 0,
                 min, max);

  ACosh ach;
  ASinh ash;
  ATanh ath;

  PointwiseEqual(ash, [](auto x) { return std::asinh(std::get<0>(x)); }, 100,
                 0);
  PointwiseEqual(ach, [](auto x) { return std::acosh(std::get<0>(x)); }, 100, 0,
                 1, 25);
  PointwiseEqual(ath, [](auto x) { return std::atanh(std::get<0>(x)); }, 100, 0,
                 min, max);

  BOOST_CHECK_EQUAL(Simplify(ACos()(Cos())(x)), x);
  BOOST_CHECK_EQUAL(Simplify(ASin()(Sin())(x)), x);
  BOOST_CHECK_EQUAL(Simplify(ATan()(Tan())(x)), x);

  BOOST_CHECK_EQUAL(Simplify(ACosh()(Cosh())(x)), x);
  BOOST_CHECK_EQUAL(Simplify(ASinh()(Sinh())(x)), x);
  BOOST_CHECK_EQUAL(Simplify(ATanh()(Tanh())(x)), x);
  static_assert(is_all_but_last_0<0,0,2>::value, "");

  BOOST_CHECK_EQUAL(Simplify(Sin()(GetIPolynomial<0,-2>())(x)),
                    (GetIPolynomial<0,-2>()((Sin()*Cos()))(x)));
  BOOST_CHECK_EQUAL(Simplify(Cos()(GetIPolynomial<0,-2>())(x)),
                    (Cos()*Cos()-Sin()*Sin())(x));
  BOOST_CHECK_EQUAL(Simplify(Sin()(GetIPolynomial<0,3>()(x))),
                    ComposeRaw(GetIPolynomial<0,3,0,-4>(), Sin(), x));
  PointwiseEqual(Simplify(Cos()(GetIPolynomial<0,3>())(x)),
                 (Composition<Cos,IntegralPolynomial<0,3>, decltype(x)>()),
                 100, 1E-9);

  BOOST_CHECK_EQUAL(Simplify(Sin()(GetIPolynomial<0,1,1>())(x)),
                    Sin()(x) * Cos()(x*x) + Cos()(x) * Sin()(x*x));

}
