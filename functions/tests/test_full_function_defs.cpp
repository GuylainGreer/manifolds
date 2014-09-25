#include <boost/test/unit_test.hpp>
#include "functions/full_function_defs.hh"
#include "functions/variables.hh"
#include "functions/trig.hh"

struct ReturnTwo : manifolds::Function<
  manifolds::int_<1000000>, 1,2>
{
  auto operator()() const
  {
    return manifolds::make_my_tuple(1,2);
  }
};

std::ostream & operator<<(std::ostream & s, ReturnTwo)
{
  return s << "{1,2}";
}

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
