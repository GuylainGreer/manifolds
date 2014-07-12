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

  template <std::size_t rows, std::size_t cols,
	    class ... Functions>
  struct FunctionMatrix
  {
    std::tuple<Functions...> functions;

    template <class ... Rows>
    FunctionMatrix(Rows...rs):
      functions(std::tuple_cat(rs.GetFunctions()...)){}

    FunctionMatrix(std::tuple<Functions...> f):
      functions(f){}

    template <class ... Args, std::size_t ... indices>
    auto eval(std::integer_sequence<std::size_t,indices...>,
	      Args...args) const
    {
      return GetMatrix<rows,cols>
	(std::get<indices>(functions)(args...)...);
    }

    template <class ... Args>
    auto operator()(Args...args) const
    {
      return eval(std::index_sequence_for<Functions...>(), args...);
    }
  };

  template <std::size_t rows, std::size_t cols,
	    class ... Functions>
  auto GetFunctionMatrix(std::tuple<Functions...> functions)
  {
    return FunctionMatrix<rows,cols,Functions...>(functions);
  }

  template <class F, class...Fs>
  F first(F f, Fs...)
  {
    return f;
  }

  template <class ... Rows>
  auto GetFunctionMatrix(Rows...rows)
  {
    return GetFunctionMatrix<
      sizeof...(Rows),
      std::tuple_size<
	decltype(first(rows...).GetFunctions())>::value>
      (std::tuple_cat(rows.GetFunctions()...));
  }
}

#endif
