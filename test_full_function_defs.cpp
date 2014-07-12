#include <boost/test/unit_test.hpp>
#include "full_function_defs.hh"
#include "variables.hh"
#include "trig.hh"

struct ReturnTwo : manifolds::Function
{
  auto operator()() const
  {
    return std::make_tuple(1,2);
  }
};

BOOST_AUTO_TEST_CASE(full_function_test)
{
  using namespace manifolds;
  auto f = Sin()(x);

  BOOST_CHECK_EQUAL(f(3,4,5), std::sin(3));

  //Testing automatic tuple-unpacking
  ReturnTwo rt;
  auto one = x(rt);
  auto two = y(rt);

  BOOST_CHECK_EQUAL(one(), 1);
  BOOST_CHECK_EQUAL(two(), 2);
}
