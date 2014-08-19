#ifndef MANIFOLDS_FUNCTION_INTERFACE_HH
#define MANIFOLDS_FUNCTION_INTERFACE_HH

#include <utility>
#include "tmp_utils.hh"

namespace manifolds {

  template <class ... indices>
  struct list
  {
    typedef list type;
  };

  //Tag
  template <class INDICES, int INPUT_DIM, int OUTPUT_DIM>
  struct Function
  {
    typedef INDICES indices;
    static const bool stateless = false;
    static const int input_dim = INPUT_DIM;
    static const int output_dim = OUTPUT_DIM;
    //Means you can change the order of
    //Multiplications, ie A*B == B*A
    static const bool abelian_arithmetic = true;
  };

}
#endif
