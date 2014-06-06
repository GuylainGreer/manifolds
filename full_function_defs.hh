#ifndef MANIFOLDS_FUNCTIONS_FULL_DEFS_HH
#define MANIFOLDS_FUNCTIONS_FULL_DEFS_HH

#include "function.hh"
#include "composition.hh"
#include "is_function.hh"
#include <utility>
#include <tuple>
#include <type_traits>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/seq/for_each.hpp>

namespace manifolds {

template <class FunctionImpl>
struct FunctionCommon
{

  template <class InnerFunc>
  using prepare = typename std::remove_reference<
    typename std::remove_cv<
      InnerFunc>::type
    >::type;

  //Return a composite functionf
  template <class InnerFunc, class = typename std::enable_if<
			       is_function<InnerFunc>::value
			       >::type>
  auto operator()(InnerFunc && f) const
  {
    return CompositionImpl<
      FunctionImpl, prepare<InnerFunc>>
      (*static_cast<const FunctionImpl*>(this), f);
  }

  //Expand the tuple
  template <class ... Args>
  auto operator()(std::tuple<Args...> && t) const
  {
    return (*static_cast<const FunctionImpl*>(this))();
  }
};

}

#include "trig.hh"
#include "polynomial.hh"
#include "variables.hh"
#include "addition.hh"
#include "multiplication.hh"
#include "unary_minus.hh"

#define DEF_FULL_FUNCTION(r, data, func)			\
  struct func : BOOST_PP_CAT(func,Impl),			\
		FunctionCommon<func>				\
  {								\
    using FunctionCommon<func>::operator();			\
    template <class Arg, class ... Args, class =		\
	      typename std::enable_if<				\
                  !is_function<Arg>::value>::type>      	\
      auto operator()(Arg arg, Args ... args) const		\
    {								\
      return BOOST_PP_CAT(func,Impl)::operator()(arg,args...);	\
    }								\
  };

#define FIRST(a,b,c) a
#define SECOND(a,b,c) b
#define THIRD(a,b,c) c

#define CAT_TEMPLATE(tuple) \
  BOOST_PP_CAT(THIRD tuple, IMPL)<SECOND tuple>

namespace manifolds {
  BOOST_PP_SEQ_FOR_EACH(DEF_FULL_FUNCTION,
			~, (Sin)(Cos)(Tan)(Log)
			(Sinh)(Cosh)(Tanh)(ASin)
			(ACos)(ATan)(ASinh)(ACosh)
			(ATanh)(Exp)(Pow))

  template <class N>
  struct UnaryMinus : UnaryMinusImpl<N>,
    FunctionCommon<UnaryMinus<N>>
  {
    using UnaryMinusImpl<N>::UnaryMinusImpl;
    using FunctionCommon<UnaryMinus<N>>::operator();
    template <class Arg, class ... Args, class =
	      typename std::enable_if<
                  !is_function<Arg>::value>::type>
      auto operator()(Arg arg, Args ... args) const
    {
      return UnaryMinusImpl<N>::operator()(arg,args...);
    }
  };

  template <int N>
  struct Variable : VariableImpl<N>,
    FunctionCommon<Variable<N>>
  {
    using FunctionCommon<Variable>::operator();
    template <class Arg, class ... Args, class =
	      typename std::enable_if<
                  !is_function<Arg>::value>::type>
      auto operator()(Arg arg, Args ... args) const
    {
      return VariableImpl<N>::operator()(arg,args...);
    }
  };

  static const Variable<0> t;
  static const Variable<1> x;
  static const Variable<2> y;
  static const Variable<3> z;

#define DEF_FF_TEMPLATE(r, data, func)					\
  template <class ... Coeffs>						\
  struct func : BOOST_PP_CAT(func,Impl)<Coeffs...>,			\
		FunctionCommon<func<Coeffs...>>				\
  {									\
    using BOOST_PP_CAT(func,Impl)<Coeffs...>::BOOST_PP_CAT(func, Impl); \
    using FunctionCommon<func>::operator();				\
    template <class Arg, class ... Args, class =			\
	      typename std::enable_if<					\
		  !is_function<Arg>::value>::type>                      \
      auto operator()(Arg arg, Args ... args) const			\
      {									\
	return BOOST_PP_CAT(func,Impl)<Coeffs...>::			\
	  operator()(arg,args...);					\
      }									\
  };

  BOOST_PP_SEQ_FOR_EACH(DEF_FF_TEMPLATE,
			~, (Polynomial)(Addition)
			(Multiplication)(Composition))
}

#undef DEF_FULL_FUNCTION

#endif
