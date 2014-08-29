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

  enum class FunctionType
  {
    Data,
    Function,
      TangentVector,
      CotangentVector
  };

  template <FunctionType e>
  using FTag =
    std::integral_constant<FunctionType, e>;

  template <class INDICES, int INPUT_DIM, int OUTPUT_DIM>
  struct Function
  {
    typedef std::tuple<FTag<FunctionType::Data>> input_types;
    typedef std::tuple<FTag<FunctionType::Data>> output_types;
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
