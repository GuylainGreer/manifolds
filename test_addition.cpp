#include <boost/test/unit_test.hpp>
#include "addition.hh"
#include "variables.hh"
#include "full_function_defs.hh"
#include "trig.hh"
#include "polynomial.hh"
#include "operators.hh"
#include <iostream>
#include "streaming2.hh"

BOOST_AUTO_TEST_CASE(addition_test)
{
  using namespace manifolds;
  Variable<1> b1;
  Variable<2> b2;
  Variable<4> b3;
  Variable<6> m1;
  Variable<7> m2;
  Variable<8> m3;
  auto c33 = b1 + m1 * t;
  auto c22 = b2 + m2 * t;
  auto c11 = b3 + m3 * t;
  auto part1 = 4_c * c22 * c22 * c11 * c33;
  CustomVariableNamer vnamer{
      std::make_pair(1, "b1"),
      std::make_pair(2, "b2"),
      std::make_pair(4, "b3"),
      std::make_pair(6, "m1"),
      std::make_pair(7, "m2"),
      std::make_pair(8, "m3"),
    };
  Stream2(std::cout, part1, vnamer) << "\n\n";
}
