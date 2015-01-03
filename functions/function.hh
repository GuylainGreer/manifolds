#ifndef MANIFOLDS_FUNCTION_INTERFACE_HH
#define MANIFOLDS_FUNCTION_INTERFACE_HH

#include <utility>
#include "tmp_utils.hh"

namespace manifolds {

template <class... indices> struct list {
  typedef list type;
};

enum ComplexOutputBehaviour {
  NeverComplex,
  AlwaysComplex,
  SameAsInput
};

template <class T>
struct ComplexOutputType
    : std::integral_constant<ComplexOutputBehaviour, T::complex_output> {};

template <class... Functions>
    struct VariadicComplexOutput
    : std::integral_constant < ComplexOutputBehaviour,
    (or_<bool_<Functions::complex_output == AlwaysComplex>...>::value)
        ? AlwaysComplex
        : (or_<bool_<Functions::complex_output == SameAsInput>...>::value
               ? SameAsInput
               : NeverComplex) > {};

template <class INDICES, int INPUT_DIM, int OUTPUT_DIM> struct Function {
  typedef INDICES indices;
  static const int input_dim = INPUT_DIM;
  static const int output_dim = OUTPUT_DIM;
  // Defaulting to safe value
  static const bool stateless = false;
  static const ComplexOutputBehaviour complex_output = AlwaysComplex;
  // Means you can change the order of
  // Multiplications, ie A*B == B*A
  // Defaulting to most common value expected
  static const bool abelian_arithmetic = true;
};
}
#endif
