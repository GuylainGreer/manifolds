#ifndef MANIFOLDS_FUNCTIONS_IS_FUNCTION_HH
#define MANIFOLDS_FUNCTIONS_IS_FUNCTION_HH

#include "function.hh"
#include <type_traits>
#include <utility>
#include <tuple>

namespace manifolds {
  //For now just test whether first function is a function
  template <class Func>
  struct is_function
  {

    typedef typename std::remove_cv<
      typename std::remove_reference<
	Func>::type
      >::type F;
    typedef std::integral_constant<
      bool,
      std::is_base_of<
	Function, F>::value ||
      std::is_base_of<
	MultiFunction, F>::value> type;

    static const bool value = type::value;
  };

  template <class T>
  struct is_tuple: std::false_type{};

  template <class ... Args>
  struct is_tuple<std::tuple<Args...>>:
    std::true_type{};
}

#endif
