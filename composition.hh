#ifndef MANIFOLDS_FUNCTION_COMPOSITION_HH
#define MANIFOLDS_FUNCTION_COMPOSITION_HH

#include "is_function.hh"
#include "function.hh"
#include "equals.hh"
#include "streaming.hh"
#include "simplify_variadic.hh"

namespace manifolds {

template <class ... Functions>
struct Composition : MultiFunction
{
  static const bool stateless =
    and_<is_stateless<Functions>...>::value;

  static_assert(sizeof...(Functions)>0,
		"Cannot have empty composition!");
  Composition(){}
  Composition(const Functions & ... functions):
    functions(functions...){}
  Composition(const std::tuple<Functions...> & f):
    functions(f){}

  template <class Arg, class ... Args,
	    class = typename std::enable_if<
	      !is_function<Arg>::value
	      >::type>
  auto operator()(Arg arg, Args ... args) const
  {
    static const int last = sizeof...(Functions)-1;
    return eval(std::get<last>(functions)(arg, args...),
		int_<last-1>());
  }

  auto operator()() const
  {
    static const int last = sizeof...(Functions)-1;
    return eval(std::get<last>(functions)(),
		int_<last-1>());
  }

  template <class Arg>
  auto eval(Arg arg, int_<0>) const
  {
    typedef decltype(std::get<0>(functions)(arg)) result;
    return std::forward<result>(std::get<0>(functions)(arg));
  }

  template <int N, class Arg>
  auto eval(Arg arg, int_<N>) const
  {
    typedef decltype(std::get<N>(functions)(arg)) result;
    return eval(std::forward<result>
		(std::get<N>(functions)(arg)),
		int_<N-1>());
  }

  template <class InnerFunc>
  using prepare = typename std::remove_reference<
    typename std::remove_cv<
      InnerFunc>::type
    >::type;

  //Return a composite function
  template <class InnerFunc, class = typename std::enable_if<
			       is_function<InnerFunc>::value
			       >::type>
  auto operator()(InnerFunc f) const
  {
    return Simplify(Composition<
		    Composition,
		    prepare<InnerFunc>>(*this, f));
  }

  auto GetFunctions() const
  {
    return functions;
  }

  void stream(std::ostream & s,
			int_<sizeof...(Functions)>)
  {
  }

  template <int N>
  void stream(std::ostream & s, int_<N>)
  {
    std::get<N>(functions).Print(s);
    s << "(";
    stream(s, int_<N+1>());
    s << ")";
  }

  template <class T>
  bool operator==(const T & t) const
  {
    return VariadicEqual(*this, t);
  }

  void Print(std::ostream & s)
  {
    stream(s, int_<0>());
  }
private:
  std::tuple<Functions...> functions;
};

  template <class> struct is_composition : std::false_type{};

  template <class...Args>
  struct is_composition<Composition<Args...>> :
    std::true_type{};

  template <class Arg, class ... Args>
  std::ostream & operator<<(std::ostream & s,
			    Composition<Arg,Args...> c)
  {
    StreamVariadic("Composition", c, s);
    return s;
  }

  template <class ... Functions>
  struct Simplification<
    Composition<Functions...>,
    typename std::enable_if<(sizeof...(Functions)>2)>::type>
  {
    typedef decltype(SimplifyV<Composition>
		     (std::declval<std::tuple<Functions...>>(),
		      std::false_type{})) type;

    static type Combine(Composition<Functions...> a)
    {
      return SimplifyV<Composition>(a.GetFunctions(),
				    std::false_type{});
    }
  };

  template <class F, class ... Functions>
  struct Simplification<
    Composition<F, Composition<Functions...>>>
  {
    typedef Composition<F, Functions...> type;

    static type Combine(Composition<F, Composition<Functions...>> c)
    {
      auto t = c.GetFunctions();
      return {std::tuple_cat(remove_element<1>(t),
			     std::get<1>(t).GetFunctions())};
    }
  };

}

#endif
