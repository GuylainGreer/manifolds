#include <boost/test/unit_test.hpp>
#include "full_function_defs.hh"
#include "simplify.hh"
#include "addition.hh"
#include "trig.hh"
#include "operators.hh"
#include "polynomial.hh"

BOOST_AUTO_TEST_CASE(simplify_test)
{
  using namespace manifolds;
  typedef Addition<Sin,Cos> Type;
  static_assert(std::is_same<
		Simplification<Type>::type,
		Type>::value, "Failed to not simplify!");

  static_assert(std::is_same<
		Simplification<Addition<Addition<Sin,Cos>, Tan>>::type,
		Addition<Sin, Cos, Tan>>::value,
		"Failed to collapse nested additions!");
  Addition<Addition<Sin,Cos>, Tan> t;
  auto t2 = Simplification<decltype(t)>::Combine(t);
  static_assert(std::is_same<
		decltype(t2),
		Addition<Sin,Cos,Tan>>::value,
		"Simplification<>::Combine failed");

  /*  static_assert(std::is_same<
		decltype(Sin()+Sin()),
		Composition<Polynomial<double,double,
		std::integral_constant<int,2>>,Sin>>::value,
		"Failed to make sin+sin == 2 * sin");
  */}
