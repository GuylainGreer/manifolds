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
    static const int num_rows = rows::value;
    static const int num_cols = cols::value;
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

#include "variables.hh"
#include "trig.hh"
#include "polynomial.hh"

namespace manifolds {

  template <class r1, class c1, class r2, class c2,
	    class ... F1s, class ... F2s>
  struct Simplification<
    Composition<FunctionMatrix<r1,c1,F1s...>,
		FunctionMatrix<r2,c2,F2s...>>>
  {
    typedef FunctionMatrix<r1,c1,F1s...> FMOut;
    typedef FunctionMatrix<r2,c2,F2s...> FMIn;
    static_assert(r2::value == 1 || c2::value == 1,
		  "Inner function of composition must "
		  "be a row or column vector.");
    static const int dims =
      r2::value == 1 ? c2::value : r2::value;
    static_assert(dims == FMOut::input_dim,
		  "Dimensionality of inner vector "
		  "must match input dimensionality of "
		  "outer vector.");

    template <class TupleO, class TupleI,
	      std::size_t ... iouts,
	      std::size_t ... iins>
    static auto CombineHelper(TupleO t1, TupleI t2,
			      std::integer_sequence<
			      std::size_t, iouts...>,
			      std::integer_sequence<
			      std::size_t, iins...>)
    {
      return std::make_tuple(std::get<iouts>(t1)
			     (std::get<iins>(t2)...)...);
    }

    typedef Composition<FunctionMatrix<r1,c1,F1s...>,
			FunctionMatrix<r2,c2,F2s...>> in_type;

    static auto Combine(in_type c)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying composition of function matrices\n";
#endif
      auto t = c.GetFunctions();
      return GetFunctionMatrix<r1::value, c1::value>
	(CombineHelper(std::get<0>(t).GetFunctions(),
		       std::get<1>(t).GetFunctions(),
		       std::index_sequence_for<F1s...>(),
		       std::index_sequence_for<F2s...>()));
    }

    typedef decltype(Combine(std::declval<in_type>())) type;
  };

  template <int i, bool a, class ... FMFuncs>
  struct Simplification<
    Composition<Variable<i,a>,
		FunctionMatrix<FMFuncs...>>>
  {
    typedef typename nth<i+2,FMFuncs...>::type type;

    static type Combine(Composition<Variable<i,a>,
			FunctionMatrix<FMFuncs...>> c)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying composition of "
	"variable and function matrix\n";
#endif
      return std::get<i>(std::get<1>(c.GetFunctions()).
			 GetFunctions());
    }
  };

  template <class r, class c, class ... Fs>
  struct Simplification<
    FunctionMatrix<r,c, Fs...>, 1>
  {
    typedef FunctionMatrix<r,c, Fs...> in_type;
    static auto Combine(in_type m)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying individual elements "
	"of function matrix\n";
#endif
      auto t = m.GetFunctions();
      return GetFunctionMatrix<r::value, c::value>
	(SimplifyIndividuals(t,
			     std::make_index_sequence<
			     std::tuple_size<
			     decltype(t)>::value>()));
    }

    typedef decltype(Combine(std::declval<in_type>())) type;
  };

  template <class F, class P, class r, class c>
  struct Simplification<
    Multiplication<
    F, Composition<Pow,
		   FunctionMatrix<r,c,F,P>>>, 0,
    typename std::enable_if<is_stateless<F>::value>::type>
  {
    typedef Multiplication<
      F, Composition<Pow,
		     FunctionMatrix<r,c,F,P>>> in_type;
    static auto Combine(in_type f)
    {
      auto p = std::get<1>(std::get<1>
			   (std::get<1>(f.GetFunctions()).
			    GetFunctions()).GetFunctions());
      return Pow()(F(), Add(p, 1_c));
    }

    typedef decltype(Combine(std::declval<in_type>())) type;
  };
}

#endif
