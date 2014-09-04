#ifndef MANIFOLDS_FUNCTIONS_TRANSPOSE_HH
#define MANIFOLDS_FUNCTIONS_TRANSPOSE_HH

#include "function.hh"
#include "full_function_defs.hh"
#include "matrix.hh"

namespace manifolds
{
  struct TransposeImpl : Function<int_<7>, 1,1>
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

  DEF_FULL_FUNCTION(Transpose)

  static const Transpose transpose = Transpose();

  inline std::ostream & operator<<(std::ostream & s, Transpose)
  {
    return s << "Transpose";
  }

}

#include "function_matrix.hh"

namespace manifolds
{

  template <class r, class c, class ... Funcs>
  struct Simplification<
    Composition<Transpose,
		FunctionMatrix<
		  r, c, Funcs...>>,
    /*com_t_fm*/0>
  {
    typedef Composition<Transpose,
			FunctionMatrix<
			  r, c, Funcs...>> in_type;
    typedef tuple<Funcs...> f_type;

    template <std::size_t col>
      struct Inner1
    {
      template <std::size_t row>
      struct Inner2
      {
	static auto apply(f_type m)
	{
	  return get<row * c::value + col>(m);
	}
      };
      template <std::size_t ... is>
      static auto apply(f_type m, std::integer_sequence<
			std::size_t, is...>)
      {
	return make_my_tuple(Inner2<is>::apply(m)...);
      }
    };
    template <std::size_t ... is>
      static auto apply(f_type m, std::integer_sequence<
			std::size_t,is...>)
    {
      std::make_index_sequence<c::value> s;
      return tuple_cat(Inner1<is>::apply(m,s)...);
    }

    static auto Combine(in_type co)
    {
      SIMPLIFY_INFO("Simplifying transpose of function matrix\n");
      std::make_index_sequence<r::value> s;
      return GetFunctionMatrix<c::value,r::value>
	(apply(get<1>(co.GetFunctions()).
	       GetFunctions(), s));
    }
  };

}
#endif
