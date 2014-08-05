#ifndef MANIFOLDS_FUNCTIONS_IS_FUNCTION_HH
#define MANIFOLDS_FUNCTIONS_IS_FUNCTION_HH

#include "function.hh"
#include <type_traits>
#include <utility>
#include <tuple>

namespace manifolds {
  //For now just test whether first function is a function
  template <class F>
  std::true_type is_function_helper(int,int_<F::input_dim> =
				    int_<F::input_dim>(),
				    int_<F::output_dim> =
				    int_<F::output_dim>(),
				    bool_<F::stateless> =
				    bool_<F::stateless>(),
				    bool_<F::abelian_arithmetic> =
				    bool_<F::abelian_arithmetic>());
  template <class>
  std::false_type is_function_helper(long);

  template <class F>
  using is_function =
    decltype(is_function_helper<F>(0));

  template <class T>
  struct is_tuple: std::false_type{};

  template <class ... Args>
  struct is_tuple<std::tuple<Args...>>:
    std::true_type{};
}

#endif
