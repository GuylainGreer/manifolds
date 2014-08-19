#include <boost/test/unit_test.hpp>
#include "streaming2.hh"
#include "trig.hh"
#include "polynomial.hh"
#include "variables.hh"
#include "operators.hh"
#include <iostream>

BOOST_AUTO_TEST_CASE(streaming_test)
{
  using namespace manifolds;
  auto p1 = x * x;
  auto p = p1(x - 1_c) + x * x * x;
  Stream2(std::cout, p) << "\n\n";
}
