#ifndef MANIFOLDS_FUNCTIONS_FUNCTION_MATRIX_HH
#define MANIFOLDS_FUNCTIONS_FUNCTION_MATRIX_HH

#include "function.hh"
#include "matrix.hh"

namespace manifolds {

  template <class ... Functions>
  struct RowHolder
  {
    RowHolder(Functions...fs):functions(fs...){}
    std::tuple<Functions...> functions;
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
    Function<max<Functions::input_dim...>::value,
	     max<Functions::output_dim...>::value>
  {
    std::tuple<Functions...> functions;

    template <class ... Rows>
    FunctionMatrixImpl(Rows...rs):
      functions(std::tuple_cat(rs.GetFunctions()...)){}

    FunctionMatrixImpl(std::tuple<Functions...> f):
      functions(f){}

    template <class ... Args, std::size_t ... indices>
    auto eval(std::integer_sequence<std::size_t,indices...>,
	      Args...args) const
    {
      return GetMatrix<rows::value,cols::value>
	(std::get<indices>(functions)(args...)...);
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
  };

  DEF_FF_TEMPLATE(FunctionMatrix)

  template <std::size_t rows, std::size_t cols,
	    class ... Functions>
  auto GetFunctionMatrix(std::tuple<Functions...> functions)
  {
    return FunctionMatrix<
      int_<rows>,int_<cols>,Functions...>(functions);
  }

  template <class ... Rows>
  auto GetFunctionMatrix(Rows...rows)
  {
    return GetFunctionMatrix<
      sizeof...(Rows),
      std::tuple_size<
	decltype(std::declval<
		 typename first<Rows...>::type>().
		 GetFunctions())>::value>
      (std::tuple_cat(rows.GetFunctions()...));
  }

  template <class rows, class cols, class ... Functions>
  std::ostream & operator<<
  (std::ostream & s,
   FunctionMatrix<rows, cols, Functions...> f)
  {
    s << "FunctionMatrix<" << rows::value << ", "
      << cols::value << ">";
    StreamVariadic("", f, s);
    return s;
  }

  template <class ... Functions>
  auto GetRow(Functions ... functions)
  {
    return Row<Functions...>(functions...);
  }
}

#endif
