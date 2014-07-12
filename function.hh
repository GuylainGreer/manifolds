#ifndef MANIFOLDS_FUNCTION_INTERFACE_HH
#define MANIFOLDS_FUNCTION_INTERFACE_HH

#include <utility>

namespace manifolds {

  //Tag
  struct Function
  {
    static const bool stateless = false;
    //Means you can change the order of
    //Multiplications, ie A*B == B*A
    static const bool abelian_arithmetic = true;
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

  template <class T>
  struct has_abelian_arithmetic
  {
    static const bool value = T::abelian_arithmetic;
    typedef std::integral_constant<
      bool, value> type;
  };

  template <class Arg, class ... Args>
  struct and_ : std::conditional<
    Arg::value, and_<Args...>, std::false_type>::type{};

  template <class Arg>
  struct and_<Arg> : std::conditional<
    Arg::value, std::true_type, std::false_type>::type{};

  template <int N>
  using int_ = std::integral_constant<int, N>;

  template <bool B>
  using bool_ = std::integral_constant<bool, B>;
}
#endif
