#ifndef MANIFOLDS_FUNCTION_INTERFACE_HH
#define MANIFOLDS_FUNCTION_INTERFACE_HH

#include <utility>

namespace manifolds {

  //Tag
  struct Function
  {
    static const bool stateless = false;
  };

  //Tag
  struct MultiFunction : Function
  {
  };

  template <class T>
  struct is_stateless
  {
    static const bool value = T::stateless;
    typedef std::integral_constant<
      bool, value> type;
  };

  template <class Arg, class ... Args>
  struct and_ : std::conditional<
    Arg::value, and_<Args...>, std::false_type>{};

  template <class Arg>
  struct and_<Arg> : std::conditional<
    Arg::value, std::true_type, std::false_type>{};
}
#endif
