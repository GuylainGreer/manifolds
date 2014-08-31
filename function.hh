#ifndef MANIFOLDS_FUNCTION_INTERFACE_HH
#define MANIFOLDS_FUNCTION_INTERFACE_HH

#include <utility>
#include "tuple_util.hh"

namespace manifolds {

  template <class ... indices>
  struct list
  {
    typedef list type;
  };

  struct DataTag{};
  struct FunctionTag{};
  struct VectorTag{};
  struct TangentVectorTag{};

  template <class INDICES, int INPUT_DIM, int OUTPUT_DIM>
  struct Function
  {
    typedef std::tuple<DataTag> input_types;
    typedef std::tuple<DataTag> output_types;
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
