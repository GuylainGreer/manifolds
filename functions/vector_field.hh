#ifndef MANIFOLDS_FUNCTIONS_VECTOR_FIELDS_HH
#define MANIFOLDS_FUNCTIONS_VECTOR_FIELDS_HH

#include "operators.hh"
#include "derivatives.hh"

namespace manifolds
{
  template <class ... Functions>
  struct VectorField
  {
    tuple<Functions...> functions;

    VectorField(Functions ... functions):
      functions(functions...){}
    VectorField(tuple<Functions...> functions):
      functions(functions){}

    template <class F, std::size_t ... indices>
    auto eval(F f, std::integer_sequence<
	      std::size_t, indices...>) const
    {
      return AddRaw((get<indices>(functions)*
		     Derivative(f, Variable<indices>()))...);
    }

    template <class F,
	      class = typename std::enable_if<
		is_function<F>::value>::type>
    auto operator()(F f) const
    {
      return 
	Simplify(eval(f, std::index_sequence_for<Functions...>()));
    }
  };

  template <class ... Functions>
  auto GetVectorField(Functions ... functions)
  {
    return VectorField<Functions...>(functions...);
  }
}

#endif
