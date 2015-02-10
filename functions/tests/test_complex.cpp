#include <boost/test/unit_test.hpp>
#include "functions/all_simplifications.hh"
#include "functions/complex.hh"
#include "functions/operators.hh"
#include "functions/streaming.hh"

BOOST_AUTO_TEST_CASE(complex_test) {
  using namespace manifolds;
  static_assert(ComplexOutputType<decltype(x)>::value == NeverComplex, "");
  BOOST_CHECK_EQUAL(Simplify(Real()(x)), x);
  BOOST_CHECK_EQUAL(Simplify(Imag()(x)), zero);
  BOOST_CHECK_EQUAL(Phase(), Arg());
  auto r_phase = Simplify(Phase()(x));
  BOOST_CHECK_EQUAL(r_phase(3), 0);
  BOOST_CHECK_EQUAL(r_phase(-3), -M_PI);
  BOOST_CHECK_EQUAL(Simplify(Real()(I)), zero);
  BOOST_CHECK_EQUAL(Simplify(Imag()(I)), IP<1>());
  BOOST_CHECK_EQUAL(Simplify(Norm()(I)), IP<1>());
  BOOST_CHECK_EQUAL(Simplify(Norm()(x)), x * x);
  BOOST_CHECK_EQUAL(Simplify(Conjugate()(I)), -I);
  BOOST_CHECK_EQUAL(Simplify(Conjugate()(x)), x);
  BOOST_CHECK_EQUAL(Simplify(I(x)), I);
  BOOST_CHECK_EQUAL(Simplify(IP<1, 1, 1, 1, 1, 1>()(I)), IP<1>() + I);
  BOOST_CHECK_EQUAL(Simplify(IP<0, 0, 1>()(I)), IP<-1>());
  BOOST_CHECK_EQUAL(Simplify(Real()(IP<1>() * I + x)), x);
  BOOST_CHECK_EQUAL(Simplify(Imag()(IP<1>() * I + x)), IP<1>());
  BOOST_CHECK_EQUAL(Simplify(Real()((I + Sin()(x)) * (I + Cos()(x)))),
                    (Sin() * Cos())(x) - IP<1>());
  BOOST_CHECK_EQUAL(Simplify(IP<1, 2, 3, 4, 5, 6, 7>()(Sign())),
                    (IP<16, 12>()(Sign())));
  BOOST_CHECK_EQUAL(Simplify(GetPolynomial(1, 2, 3, 4, 5, 6, 7)(Sign())),
                    GetPolynomial(16, 12)(Sign()));
}
