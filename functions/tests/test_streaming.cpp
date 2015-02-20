#include <boost/test/unit_test.hpp>
#include "functions/streaming.hh"
#include "functions/trig.hh"
#include "functions/polynomial.hh"
#include "functions/variables.hh"
#include "functions/operators.hh"
#include "functions/all_simplifications.hh"
#include <iostream>

BOOST_AUTO_TEST_CASE(streaming_test) {
  using namespace manifolds;
  auto p1 = x * x;
  auto p = p1(IP<1>() + x * x) + x * x * x;
  Stream2(std::cout, p) << "\n\n";
}
