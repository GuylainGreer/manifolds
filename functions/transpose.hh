#ifndef MANIFOLDS_FUNCTIONS_TRANSPOSE_HH
#define MANIFOLDS_FUNCTIONS_TRANSPOSE_HH

#include "function.hh"
#include "full_function_defs.hh"
#include "data/matrix.hh"

namespace manifolds {
struct Transpose : Function<int_<7>, 1, 1>, FunctionCommon<Transpose> {
  using FunctionCommon<Transpose>::operator();
  static const bool stateless = true;
  static const ComplexOutputBehaviour complex_output = SameAsInput;

  template <class T,
            class = typename std::enable_if<!is_function<T>::value>::type>
  auto eval(T t) const {
    Matrix<T::num_cols, T::num_rows, typename T::coefficient_type> t2;
    for (unsigned i = 0; i < T::num_rows; i++)
      for (unsigned j = 0; j < T::num_cols; j++)
        t2.Coeff(j, i) = t.Coeff(i, j);
    return t2;
  }
};

static const Transpose transpose = Transpose();
}

#endif
