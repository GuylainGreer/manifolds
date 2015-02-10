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

  PointwiseEqual(sin_, [](auto x) { return std::sin(std::get<0>(x)); });
  PointwiseEqual(cos_, [](auto x) { return std::cos(std::get<0>(x)); });
  PointwiseEqual(tan_, [](auto x) { return std::tan(std::get<0>(x)); });

  PointwiseEqual(sinh_, [](auto x) { return std::sinh(std::get<0>(x)); });
  PointwiseEqual(cosh_, [](auto x) { return std::cosh(std::get<0>(x)); });
  PointwiseEqual(tanh_, [](auto x) { return std::tanh(std::get<0>(x)); });

  std::complex<double> i(0, 1);
  BOOST_CHECK_EQUAL(std::abs(sin_(i)), sinh_(1));
  BOOST_CHECK_EQUAL(std::abs(sinh_(i)), sin_(1));

  double min = -0.9999;
  double max = -min;
  PointwiseEqual(asin_, [](auto x) { return std::asin(std::get<0>(x)); }, 100,
                 0, min, max);
  PointwiseEqual(acos_, [](auto x) { return std::acos(std::get<0>(x)); }, 100,
                 0, min, max);
  PointwiseEqual(atan_, [](auto x) { return std::atan(std::get<0>(x)); }, 100,
                 0, min, max);

  PointwiseEqual(asinh_, [](auto x) { return std::asinh(std::get<0>(x)); }, 100,
                 0);
  PointwiseEqual(acosh_, [](auto x) { return std::acosh(std::get<0>(x)); }, 100,
                 0, 1, 25);
  PointwiseEqual(atanh_, [](auto x) { return std::atanh(std::get<0>(x)); }, 100,
                 0, min, max);

  BOOST_CHECK_EQUAL(Simplify(acos_(cos_(x))), x);
  BOOST_CHECK_EQUAL(Simplify(asin_(sin_(x))), x);
  BOOST_CHECK_EQUAL(Simplify(atan_(tan_(x))), x);

  BOOST_CHECK_EQUAL(Simplify(acosh_(cosh_(x))), x);
  BOOST_CHECK_EQUAL(Simplify(asinh_(sinh_(x))), x);
  BOOST_CHECK_EQUAL(Simplify(atanh_(tanh_(x))), x);
  static_assert(is_all_but_last_0<0, 0, 2>::value, "");

  BOOST_CHECK_EQUAL(Simplify(sin_(GetIPolynomial<0, -2>())(x)),
                    (GetIPolynomial<0, -2>()((sin_ * cos_))(x)));
  BOOST_CHECK_EQUAL(Simplify(Cos()(GetIPolynomial<0, -2>())(x)),
                    (cos_ * cos_ - sin_ * sin_)(x));
  BOOST_CHECK_EQUAL(Simplify(sin_(GetIPolynomial<0, 3>()(x))),
                    ComposeRaw(GetIPolynomial<0, 3, 0, -4>(), sin_, x));
  PointwiseEqual(Simplify(cos_(GetIPolynomial<0, 3>())(x)),
                 (Composition<Cos, IntegralPolynomial<0, 3>, decltype(x)>()),
                 100, 1E-9);

  BOOST_CHECK_EQUAL(Simplify(sin_(GetIPolynomial<0, 1, 1>())(x)),
                    sin_(x) * cos_(x * x) + cos_(x) * sin_(x * x));

  BOOST_CHECK_EQUAL(Simplify(tan_(GetIPolynomial<0, -1>()(x))),
                    Simplify(-tan_(x)));

  BOOST_CHECK_EQUAL(Simplify(tan_(x + x)), Simplify(((tan_+tan_)/(GetIPolynomial<1>() + tan_*tan_))(x)));

  BOOST_CHECK_EQUAL(Simplify((sqrt_*sqrt_)(x)), x);
}
