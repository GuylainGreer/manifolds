#include <boost/test/unit_test.hpp>
#include "functions/addition.hh"
#include "functions/variables.hh"
#include "functions/full_function_defs.hh"
#include "functions/trig.hh"
#include "functions/polynomial.hh"
#include "functions/operators.hh"
#include <iostream>
#include "functions/streaming.hh"

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
  (void)c33;
  (void)c22;
  (void)c11;
  //auto part1 = 4_c * c22 * c22 * c11 * c33;
  //(void)part1;
  CustomVariableNamer vnamer{
      std::make_pair(1, "b1"),
      std::make_pair(2, "b2"),
      std::make_pair(4, "b3"),
      std::make_pair(6, "m1"),
      std::make_pair(7, "m2"),
      std::make_pair(8, "m3"),
    };
  //Stream2(std::cout, c11, vnamer) << "\n\n";
  //Stream2(std::cout, c22, vnamer) << "\n\n";
  //Stream2(std::cout, c33, vnamer) << "\n\n";
}
