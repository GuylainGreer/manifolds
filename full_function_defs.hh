#ifndef MANIFOLDS_FUNCTIONS_FULL_DEFS_HH
#define MANIFOLDS_FUNCTIONS_FULL_DEFS_HH

#include "function.hh"
#include "composition.hh"
#include "is_function.hh"
#include <utility>
#include <tuple>
#include <type_traits>
#include <boost/preprocessor/cat.hpp>
#include "simplify.hh"

namespace manifolds {

  template <class...Funcs>
  struct Group : MultiFunction
  {
    std::tuple<Funcs...> functions;

    Group(Funcs...fs):functions{fs...}{}

    template <std::size_t...is, class...Args>
    auto eval(std::integer_sequence<std::size_t,is...>,
	      Args...args) const
    {
      return std::make_tuple(std::get<is>(functions)(args...)...);
    }

    template <class ... Args>
    auto operator()(Args...args) const
    {
      return eval(std::index_sequence_for<Funcs...>(),
		  args...);
    }

    auto GetFunctions() const
    {
      return functions;
    }
  };

  template <class ... Funcs>
  std::ostream & operator<<(std::ostream & s, Group<Funcs...> g)
  {
    StreamVariadic("Group", g, s);
    return s;
  }

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
  auto operator()(InnerFunc && f) const
  {
    return Simplify(Composition<
		    FunctionImpl, prepare<InnerFunc>>
		    (*static_cast<const FunctionImpl*>(this), f));
  }
  template <class ... InnerFunc,
	    class = typename std::enable_if<
	      and_<is_function<InnerFunc>...>::value
	      && (sizeof...(InnerFunc) > 1)
	      >::type>
  auto operator()(InnerFunc ... f) const
  {
    Group<InnerFunc...> g{f...};
    return Simplify(Composition<
		    FunctionImpl, decltype(g)>
		    (*static_cast<const FunctionImpl*>(this), g));
  }
  template <class T, std::size_t ... Indices>
  auto helper(const T & t, std::integer_sequence<
	      std::size_t, Indices...>) const
  {
    return (*static_cast<const FunctionImpl*>(this))
      (std::get<Indices>(t)...);
  }
  template <class ... Args>
  auto operator()(std::tuple<Args...> t) const
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
