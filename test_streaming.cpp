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
  auto p = Log()(Sin()(x)) + Exp()(ASin()(y));
  Stream2(std::cout, p) << "\n\n";
}
