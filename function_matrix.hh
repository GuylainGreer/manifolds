#ifndef MANIFOLDS_FUNCTIONS_FUNCTION_MATRIX_HH
#define MANIFOLDS_FUNCTIONS_FUNCTION_MATRIX_HH

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
    tuple<Functions...> functions;

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
#include "transpose.hh"

namespace manifolds {

  template <class r1, class c1, class r2, class c2,
	    class ... F1s, class ... F2s>
  struct Simplification<
    Composition<FunctionMatrix<r1,c1,F1s...>,
		FunctionMatrix<r2,c2,F2s...>>,
    /*com_fm_fm*/0>
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
      return make_my_tuple(get<iouts>(t1)
			     (get<iins>(t2)...)...);
    }

    typedef Composition<FunctionMatrix<r1,c1,F1s...>,
			FunctionMatrix<r2,c2,F2s...>> in_type;

    static auto Combine(in_type c)
    {
      SIMPLIFY_INFO("Simplifying composition of "
		    "function matrices\n");
      auto t = c.GetFunctions();
      return GetFunctionMatrix<r1::value, c1::value>
	(CombineHelper(get<0>(t).GetFunctions(),
		       get<1>(t).GetFunctions(),
		       std::index_sequence_for<F1s...>(),
		       std::index_sequence_for<F2s...>()));
    }

    typedef decltype(Combine(std::declval<in_type>())) type;
  };

  template <int i, bool a, class ... FMFuncs>
  struct Simplification<
    Composition<Variable<i,a>,
		FunctionMatrix<FMFuncs...>>,
    /*com_v_fm*/3>
  {
    typedef typename nth<i+2,FMFuncs...>::type type;

    static type Combine(Composition<Variable<i,a>,
			FunctionMatrix<FMFuncs...>> c)
    {
      SIMPLIFY_INFO("Simplifying composition of "
		    "variable and function matrix\n");
      return get<i>(get<1>(c.GetFunctions()).
		    GetFunctions());
    }
  };

  template <class r, class c, class ... Fs>
  struct Simplification<
    FunctionMatrix<r,c, Fs...>, /*var_fm*/1>
  {
    typedef FunctionMatrix<r,c, Fs...> in_type;
    static auto Combine(in_type m)
    {
      SIMPLIFY_INFO("Simplifying individual elements "
		    "of function matrix\n");
      auto t = m.GetFunctions();
      return GetFunctionMatrix<r::value, c::value>
	(SimplifyIndividuals(t,
			     std::make_index_sequence<
			     tuple_size<
			     decltype(t)>::value>()));
    }

    typedef decltype(Combine(std::declval<in_type>())) type;
  };

  template <class F, class P, class r, class c>
  struct Simplification<
    Multiplication<
    F, Composition<Pow,
		   FunctionMatrix<r,c,F,P>>>,
    /*mult_f1_com_pow_fm_f2_f1*/0,
    typename std::enable_if<is_stateless<F>::value>::type>
  {
    typedef Multiplication<
      F, Composition<Pow,
		     FunctionMatrix<r,c,F,P>>> in_type;
    static auto Combine(in_type f)
    {
      auto p = get<1>(get<1>
			   (get<1>(f.GetFunctions()).
			    GetFunctions()).GetFunctions());
      return Pow()(F(), AddRaw(p, 1_c));
    }

    typedef decltype(Combine(std::declval<in_type>())) type;
  };

  template <class r1, class c1, class r2, class c2,
	    class ... F1s, class ... F2s>
  struct Simplification<
    Multiplication<FunctionMatrix<r1,c1,F1s...>,
		   FunctionMatrix<r2,c2,F2s...>>,
    /*mult_fm_fm*/0>
  {
    typedef Multiplication<
      FunctionMatrix<r1,c1,F1s...>,
      FunctionMatrix<r2,c2,F2s...>> in_type;
    template <std::size_t row>
      struct Inner1
    {
      template <std::size_t col>
      struct Inner2
      {
	template <std::size_t ... is>
	static auto apply(in_type m, std::integer_sequence<
			  std::size_t, is...>)
	{
	  auto l = get<0>(m.GetFunctions()).GetFunctions();
	  auto r = get<1>(m.GetFunctions()).GetFunctions();
	  return Add((get<row*c1::value+is>(l) *
		      get<is*c2::value+col>(r))...);
	}
      };
      template <std::size_t ... is>
      static auto apply(in_type m, std::integer_sequence<
			std::size_t, is...>)
      {
	std::make_index_sequence<r2::value> s;
	return make_my_tuple(Inner2<is>::apply(m, s)...);
      }
    };

    template <std::size_t ... is>
      static auto apply(in_type m, std::integer_sequence<
			std::size_t, is...>)
    {
      std::make_index_sequence<c2::value> s;
      return tuple_cat(Inner1<is>::apply(m, s)...);
    }

    static auto Combine(in_type m)
    {
      std::make_index_sequence<r1::value> s;
      return
	GetFunctionMatrix<r1::value,c2::value>(apply(m,s));
    }

    typedef decltype(Combine(std::declval<in_type>())) type;
  };

  template <class F>
  struct Simplification<
    FunctionMatrix<int_<1>,int_<1>,F>, /*fm_1_1*/0>
  {
    typedef F type;
    static type Combine(FunctionMatrix<int_<1>,int_<1>,F> f)
    {
      return get<0>(f.GetFunctions());
    }
  };

  template <class ... Functions>
  struct Simplification<Group<Functions...>, /*var_grp*/0>
  {
    typedef FunctionMatrix<
      int_<sizeof...(Functions)>,
      int_<1>, Functions...> MType;
    typedef MType type;
    static type Combine(Group<Functions...> c)
    {
      SIMPLIFY_INFO("Simplifying group->vector\n");
      return {c.GetFunctions()};
    }
  };
}

#endif
