#include "variables.hh"
#include <boost/test/unit_test.hpp>

template <int N>
struct Test
{
  bool operator==(const Test& o) const
  {
    return true;
  }
};

template <int N>
std::ostream & operator<<(std::ostream & s, Test<N>)
{
  return s << "Test<" << N << ">";
}

BOOST_AUTO_TEST_CASE(test_variables)
{
  Test<0> a;
  Test<1> b;
  Test<2> c;
  Test<3> d;
  Test<4> e;
  Test<5> f;

  using namespace manifolds;

  //This test ensures that accessing the nth coordinate
  //returns the exact same object that was passed in
  BOOST_CHECK_EQUAL(t(a,b,c,d,e,f), a);
  BOOST_CHECK_EQUAL(x(a,b,c,d,e,f), b);
  BOOST_CHECK_EQUAL(y(a,b,c,d,e,f), c);
  BOOST_CHECK_EQUAL(z(a,b,c,d,e,f), d);
  BOOST_CHECK_EQUAL(Variable<4>()(a,b,c,d,e,f), e);
  BOOST_CHECK_EQUAL(Variable<5>()(a,b,c,d,e,f), f);
}
