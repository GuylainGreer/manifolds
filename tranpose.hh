#ifndef MANIFOLDS_FUNCTIONS_TRANSPOSE_HH
#define MANIFOLDS_FUNCTIONS_TRANSPOSE_HH

#include "function.hh"
#include "matrix.hh"

namespace manifolds
{
  struct Transpose : Function
  {
    static const bool stateless = true;

    template <class T>
    auto operator()(T t) const
    {
      Matrix<T::num_cols, T::num_rows,
	     typename T::coefficient_type> t2;
      for(unsigned i = 0; i < T::num_rows; i++)
	for(unsigned j = 0; j < T::num_cols; j++)
	  t2.Coeff(j,i) = t.Coeff(i,j);
      return t2;
    }
  };

  static const Transpose transpose = Transpose();
}

#endif
