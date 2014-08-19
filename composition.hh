#ifndef MANIFOLDS_FUNCTION_COMPOSITION_HH
#define MANIFOLDS_FUNCTION_COMPOSITION_HH

#include "is_function.hh"
#include "function.hh"
#include "equals.hh"
#include "streaming.hh"
#include "simplify_variadic.hh"
#include "group.hh"

namespace manifolds {

template <int output_dim>
struct ToTuple
{
  template <class X, std::size_t ... indices>
  auto eval(X x, std::integer_sequence<
	    std::size_t, indices...>,
	    int_<1>) const
  {
    return x;
  }

  template <class X, std::size_t ... indices>
  auto eval(X x, std::integer_sequence<
	    std::size_t, indices...>,
	    int_<2>) const
  {
    static const int iter_rows = X::num_rows > 1;
    static const int iter_cols = X::num_cols > 1;
    static_assert(iter_rows == 1 ||
		  iter_cols == 1,
		  "I'm not sure what's going on here");
    return std::make_tuple(x.Coeff(indices * iter_rows,
				   indices * iter_cols)...);
  }

  template <class X, std::size_t ... indices>
  auto eval(X x, std::integer_sequence<
	    std::size_t, indices...>,
	    int_<4>) const
  {
    return std::make_tuple(std::get<indices>(x)...);
  }

  template <class X>
  auto operator()(X x) const
  {
    static const int tag =
      output_dim == 1 ?
      1 :
      (2 * (has_coeff_function<X>::value) +
       4 * (is_gettable<X>::value));
    return eval(x, std::make_index_sequence<output_dim>(),
		int_<tag>());
  }
};

template <class ... Functions>
struct Composition :
    Function<
  list<int_<1>, typename Functions::indices...>,
  last<Functions...>::type::input_dim,
  first<Functions...>::type::output_dim>
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
    static const int next_ins =
      nth<last-1, Functions...>::type::input_dim;
    ToTuple<next_ins> t;
    return eval(t(std::get<last>(functions)(arg, args...)),
		int_<last-1>());
  }

  auto operator()() const
  {
    static const int last = sizeof...(Functions)-1;
    static const int next_ins =
      nth<last-1, Functions...>::type::input_dim;
    ToTuple<next_ins> t;
    return eval(t(std::get<last>(functions)()),
		int_<last-1>());
  }

  template <class Arg>
  auto eval(Arg arg, int_<0>) const
  {
    return std::get<0>(functions)(arg);
  }

  template <int N, class Arg>
  auto eval(Arg arg, int_<N>) const
  {
    static const int next_ins =
      nth<N-1, Functions...>::type::input_dim;
    ToTuple<next_ins> t;
    return eval(t(std::get<N>(functions)(arg)),
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

  template <class ... InnerFuncs,
	    class = typename std::enable_if<
	      (sizeof...(InnerFuncs) > 1) &&
	      and_<is_function<InnerFuncs>...>::value
	      >::type>
  auto operator()(InnerFuncs ... ifs) const
  {
    return (*this)(GetGroup(ifs...));
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
  struct Simplification<Composition<Functions...>,4>
  {
    typedef decltype(SimplifyV<Composition>
		     (std::declval<std::tuple<Functions...>>(),
		      std::false_type{})) type;

    static type Combine(Composition<Functions...> a)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying composition\n";
#endif
      return SimplifyV<Composition>(a.GetFunctions(),
				    std::false_type{});
    }
  };

}

#endif
