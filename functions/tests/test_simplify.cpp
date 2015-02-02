#include <boost/test/unit_test.hpp>
#include "functions/full_function_defs.hh"
#include "functions/addition.hh"
#include "functions/trig.hh"
#include "functions/operators.hh"
#include "functions/polynomial.hh"
#include "functions/all_simplifications.hh"

BOOST_AUTO_TEST_CASE(simplify_test) {
  using namespace manifolds;
  typedef Addition<Sin, Cos> Type;
  static_assert(std::is_same<SimplifiedType<Type>, Type>::value,
                "Failed to not simplify!");
  static_assert(std::is_same<SimplifiedType<Addition<Addition<Sin, Cos>, Tan> >,
                             Addition<Sin, Cos, Tan> >::value,
                "Failed to collapse nested additions!");
  Addition<Addition<Sin, Cos>, Tan> t;

  static_assert(
      std::is_same<decltype(Sin() + Sin()),
                   Composition<IntegralPolynomial<0, 2>, Sin> >::value,
      "Failed to make sin+sin == 2 * sin");

  auto ps = Sin() + Sin();
  BOOST_CHECK_EQUAL(get<0>(ps.GetFunctions()).GetCoeffs()[0], 0);
  BOOST_CHECK_EQUAL(get<0>(ps.GetFunctions()).GetCoeffs()[1], 2);

  auto ms = Sin() * Sin();
  BOOST_CHECK_EQUAL(get<0>(ms.GetFunctions()).GetCoeffs()[0], 0);
  BOOST_CHECK_EQUAL(get<0>(ms.GetFunctions()).GetCoeffs()[1], 0);
  BOOST_CHECK_EQUAL(get<0>(ms.GetFunctions()).GetCoeffs()[2], 1);

  auto ss = Sin()(Sin());
  auto mss = ss * ss;
  static_assert(
      std::is_same<decltype(mss),
                   Composition<IntegralPolynomial<0, 0, 1>, Sin, Sin> >::value,
      "Failed to simplify multiplication of "
      "composition of functions.");
}
