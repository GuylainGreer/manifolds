#ifndef MANIFOLDS_FUNCTIONS_TRANSPOSE_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_TRANSPOSE_SIMPLIFICATIONS_HH

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
