#ifndef MANIFOLDS_FUNCTIONS_FUNCTION_MATRIX_HH
#define MANIFOLDS_FUNCTIONS_FUNCTION_MATRIX_HH

#include "tuple_util.hh"
#include "full_function_defs.hh"
#include "function.hh"
#include "matrix.hh"

namespace manifolds {

  template <class ... Functions>
  struct RowHolder
  {
    RowHolder(Functions...fs):functions(fs...){}
    tuple<Functions...> functions;
    auto GetFunctions() const
    {
      return functions;
    }
  };

  template <class ... Functions>
  auto Row(Functions...fs)
  {
    return RowHolder<Functions...>(fs...);
  }

  template <class rows, class cols,
	    class ... Functions>
  struct FunctionMatrixImpl :
    Function<
    list<int_<4>, typename Functions::indices...>,
    max<Functions::input_dim...>::value,
    rows::value * cols::value>
  {
    static const int num_rows = rows::value;
    static const int num_cols = cols::value;
    static const bool stateless =
      and_<is_stateless<Functions>...>::value;
    tuple<Functions...> functions;
    static const ComplexOutputBehaviour complex_output =
      VariadicComplexOutput<Functions...>::value;

    FunctionMatrixImpl() = default;

    template <class ... Rows>
    FunctionMatrixImpl(Rows...rs):
      functions(tuple_cat(rs.GetFunctions()...)){}

    FunctionMatrixImpl(tuple<Functions...> f):
      functions(f){}

    template <class ... Args, std::size_t ... indices>
    auto eval(std::integer_sequence<std::size_t,indices...>,
	      Args...args) const
    {
      return GetMatrix<rows::value,cols::value>
	(get<indices>(functions)(args...)...);
    }

    template <class ... Args>
    auto operator()(Args...args) const
    {
      return eval(std::index_sequence_for<Functions...>(), args...);
    }

    auto GetFunctions() const
    {
      return functions;
    }

    auto GetOutputs() const
    {
      return functions;
    }

    template <class ... Rights>
    bool operator==(const FunctionMatrixImpl<
		    rows, cols, Rights...> & f) const
    {
      return VariadicEqual(*this, f);
    }

    template <class T>
    bool operator==(const T &) const
    {
      return false;
    }
  };

  DEF_FF_TEMPLATE(FunctionMatrix)

  template <std::size_t rows, std::size_t cols,
	    class ... Functions>
  auto GetFunctionMatrix(tuple<Functions...> functions)
  {
    return FunctionMatrix<
      int_<rows>,int_<cols>,Functions...>(functions);
  }

  template <class ... Rows>
  auto GetFunctionMatrix(Rows...rows)
  {
    return GetFunctionMatrix<
      sizeof...(Rows),
      tuple_size<
	decltype(std::declval<
		 typename first<Rows...>::type>().
		 GetFunctions())>::value>
      (tuple_cat(rows.GetFunctions()...));
  }

  template <class ... Functions>
  auto GetRow(Functions ... functions)
  {
    return Row<Functions...>(functions...);
  }

}

#endif
