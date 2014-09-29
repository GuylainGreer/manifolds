#ifndef MANIFOLDS_FUNCTIONS_FULL_DEFS_HH
#define MANIFOLDS_FUNCTIONS_FULL_DEFS_HH

#include "function.hh"
#include "composition.hh"
#include "is_function.hh"
#include <utility>
#include <type_traits>
#include <boost/preprocessor/cat.hpp>
#include "group.hh"

namespace manifolds {

template <class FunctionImpl>
struct FunctionCommon
{
  template <class InnerFunc>
  using prepare =
    typename std::remove_reference<
    typename std::remove_cv<
    typename std::remove_reference<
      InnerFunc>::type
      >::type>::type;
  template <class InnerFunc,
	    class = typename std::enable_if<
	      is_function<InnerFunc>::value
	      >::type>
  auto operator()(InnerFunc f) const
  {
    return Composition<FunctionImpl>
      (*static_cast<const FunctionImpl*>(this))(f);
  }
  template <class ... InnerFunc,
	    class = typename std::enable_if<
	      and_<is_function<InnerFunc>...>::value
	      && (sizeof...(InnerFunc) > 1)
	      >::type>
  auto operator()(InnerFunc ... f) const
  {
    return (*this)(GetGroup(f...));
  }

  template <class T, std::size_t ... Indices>
  auto helper(const T & t, std::integer_sequence<
	      std::size_t, Indices...>) const
  {
    return (*static_cast<const FunctionImpl*>(this))
      (get<Indices>(t)...);
  }
  template <class ... Args>
  auto operator()(tuple<Args...> t) const
  {
    return helper(t, std::index_sequence_for<Args...>());
  }

};

}

#define DEF_FULL_FUNCTION(func)					\
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

#define DEF_FF_STEMPLATE(func)				\
  template <class N>					\
  struct func : BOOST_PP_CAT(func,Impl)<N>,		\
		FunctionCommon<func<N>>			\
  {							\
    using BOOST_PP_CAT(func, Impl)<N>::			\
      BOOST_PP_CAT(func,Impl);				\
    using FunctionCommon<func<N>>::operator();		\
    template <class Arg, class ... Args, class =	\
	      typename std::enable_if<			\
                  !is_function<Arg>::value>::type>      \
      auto operator()(Arg arg, Args ... args) const	\
    {							\
      return BOOST_PP_CAT(func,Impl)<N>::		\
	operator()(arg,args...);			\
    }							\
  };

#define DEF_FF_TEMPLATE(func)				\
  template <class ... Coeffs>				\
  struct func : BOOST_PP_CAT(func,Impl)<Coeffs...>,	\
		FunctionCommon<func<Coeffs...>>		\
  {							\
    using BOOST_PP_CAT(func,Impl)			\
      <Coeffs...>::BOOST_PP_CAT(func, Impl);		\
    using FunctionCommon<func>::operator();		\
    template <class Arg, class ... Args, class =	\
	      typename std::enable_if<			\
		  !is_function<Arg>::value>::type>      \
      auto operator()(Arg arg, Args ... args) const	\
    {							\
      return BOOST_PP_CAT(func,Impl)<Coeffs...>::	\
	operator()(arg,args...);			\
    }							\
  };

#endif