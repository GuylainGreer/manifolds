#include <complex>
#include "functions/trig.hh"
#include "functions/full_function_defs.hh"
#include <boost/test/unit_test.hpp>
#include <limits>

BOOST_AUTO_TEST_CASE(trig_tests)
{
  using namespace manifolds;
  Sin s;
  Cos c;
  Tan t;

  BOOST_CHECK_EQUAL(s(0), 0);
  BOOST_CHECK_EQUAL(c(0), 1);
  BOOST_CHECK_EQUAL(t(0), 0);
  BOOST_CHECK_CLOSE(s(1) / c(1), t(1), 1E-13);
  BOOST_CHECK_CLOSE(s(2) * s(2) + c(2) * c(2),
		    1, 0);

  Sinh sh;
  Cosh ch;
  Tanh th;

  BOOST_CHECK_EQUAL(sh(0), 0);
  BOOST_CHECK_EQUAL(ch(0), 1);
  BOOST_CHECK_CLOSE(sh(1) / ch(1), th(1), 1E-13);
  BOOST_CHECK_CLOSE(sh(2) * sh(2) - ch(2) * ch(2),
		    -1, 1E-12);

  std::complex<double> i(0,1);
  BOOST_CHECK_EQUAL(std::abs(s(i)), -sh(-1));
  BOOST_CHECK_EQUAL(std::abs(sh(i)), s(1));
  ACos ac;
  ASin as;
  ATan at;
  
  BOOST_CHECK_CLOSE(ac(c(1)), 1, 1E-13);
  BOOST_CHECK_CLOSE(as(s(1)), 1, 1E-13);
  BOOST_CHECK_CLOSE(at(t(1)), 1, 1E-13);

  ACosh ach;
  ASinh ash;
  ATanh ath;
  
  BOOST_CHECK_CLOSE(ach(ch(1)), 1, 1E-13);
  BOOST_CHECK_CLOSE(ash(sh(1)), 1, 1E-13);
  BOOST_CHECK_CLOSE(ath(th(1)), 1, 1E-13);
}
