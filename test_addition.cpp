#include <boost/test/unit_test.hpp>
#include "addition.hh"
#include "variables.hh"
#include "full_function_defs.hh"
#include "trig.hh"
#include "polynomial.hh"
#include "operators.hh"
#include <iostream>

BOOST_AUTO_TEST_CASE(addition_test)
{
  using namespace manifolds;
  Addition<decltype(x), decltype(z)> a(x,z);
  static_assert(is_function<Variable<0>>::value, "");
  BOOST_CHECK_EQUAL(a(1,2,3,4), 4);
  BOOST_CHECK_EQUAL(a(1,2,4,8,16), 5);
  auto l = Log()(x);
  static_assert(is_function<decltype(l)>::value, "");
  static_assert(!IsVariadic<Composition, Cos>::value, "");
  Variable<0> b0;
  Variable<1> b1;
  Variable<2> b2;
  Variable<4> b3;
  Variable<5> m0;
  Variable<6> m1;
  Variable<7> m2;
  Variable<8> m3;
  auto c12i = t;
  auto c12r = b0 + m0 * t;
  auto c33 = b1 + m1 * t;
  auto c22 = b2 + m2 * t;
  auto c11 = b3 + m3 * t;
  auto f =
      4_c * c22 * c22 * c11 * c33 -
      8_c * c22 * (c11 + c33) * c11 * c33 +
      (c11 + c33) * (c11 + c33) * c11 * c33 -
      (c12r * c12r + c12i * c12i) * (c11 + c33) * (c11 * c33);
  std::cout << "\n\n" << f << "\n\n";
}
