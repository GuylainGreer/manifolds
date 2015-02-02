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

template <class FunctionImpl> struct FunctionCommon {
  template <class InnerFunc>
  using prepare = typename std::remove_reference<typename std::remove_cv<
      typename std::remove_reference<InnerFunc>::type>::type>::type;

  template <class InnerFunc>
  auto eval_dispatch(std::true_type, InnerFunc f) const {
    return Composition<FunctionImpl, InnerFunc>{
      *static_cast<const FunctionImpl *>(this), f
    };
  }
  template <class Var> auto eval_dispatch(std::false_type, Var x) const {
    return static_cast<const FunctionImpl *>(this)->eval(x);
  }
  template <class T> auto operator()(T t) const {
    return eval_dispatch(is_function<T>(), t);
  }
  template <class... InnerFuncs,
            class = typename std::enable_if<(sizeof...(InnerFuncs) > 1)>::type>
  auto eval_dispatch(std::true_type, InnerFuncs... fs) const {
    auto g = GetGroup(fs...);
    return Composition<FunctionImpl, decltype(g)>(
        *static_cast<const FunctionImpl *>(this), g);
  }
  template <class... Vars,
            class = typename std::enable_if<(sizeof...(Vars) > 1)>::type>
  auto eval_dispatch(std::false_type, Vars... xs) const {
    return static_cast<const FunctionImpl *>(this)->eval(xs...);
  }
  template <class... InnerFunc,
            class = typename std::enable_if<(sizeof...(InnerFunc) > 1)>::type>
  auto operator()(InnerFunc... f) const {
    return eval_dispatch(and_<is_function<InnerFunc>...>(), f...);
  }

  template <class T, std::size_t... Indices>
  auto helper(const T &t,
              std::integer_sequence<std::size_t, Indices...>) const {
    return (*static_cast<const FunctionImpl *>(this))(std::get<Indices>(t)...);
  }
  template <class... Args> auto operator()(tuple<Args...> t) const {
    return helper(t, std::index_sequence_for<Args...>());
  }
};
}

#endif
