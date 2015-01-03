#include <boost/test/unit_test.hpp>
#include "functions/zero.hh"
#include "functions/trig.hh"
#include "functions/operators.hh"
#include "functions/all_simplifications.hh"

BOOST_AUTO_TEST_CASE(zero_function_test) {
  using namespace manifolds;

  Zero z;
  Sin s;
  static_assert(std::is_same<decltype(s + z), Sin>::value,
                "Failed to simplify Sin + 0");

  static_assert(std::is_same<decltype(z + s), Sin>::value,
                "Failed to simplify 0 + Sin");

  static_assert(std::is_same<decltype(z + z), Zero>::value,
                "Failed to simplify 0 + 0");

  static_assert(std::is_same<decltype(s * z), Zero>::value,
                "Failed to simplify Sin * 0");

  static_assert(std::is_same<decltype(z * s), Zero>::value,
                "Failed to simplify 0 * Sin");

  static_assert(std::is_same<decltype(z * z), Zero>::value,
                "Failed to simplify 0 * 0");
}
