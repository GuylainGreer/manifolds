#ifndef MANIFOLDS_FUNCTION_COMPOSITION_HH
#define MANIFOLDS_FUNCTION_COMPOSITION_HH

#include "is_function.hh"
#include "function.hh"
#include "equals.hh"
#include "group.hh"

namespace manifolds {

  template <class C, bool>
  struct Delay
  {
    C c;
    Delay(const C & c):c(c){}
    C Simplify() const{return c;}
  };

  template <class C>
  Delay<C, true> GetDelay(const C & c)
  {
    return {c};
  }

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
    return make_my_tuple(x.Coeff(indices * iter_rows,
				   indices * iter_cols)...);
  }

  template <class X, std::size_t ... indices>
  auto eval(X x, std::integer_sequence<
	    std::size_t, indices...>,
	    int_<4>) const
  {
    return make_my_tuple(get<indices>(x)...);
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

  template <ComplexOutputBehaviour ... behaviours>
  struct ComposedBehaviour;

  template <ComplexOutputBehaviour left,
	    ComplexOutputBehaviour right>
  struct BehaviourMerger :
    std::integral_constant<
    ComplexOutputBehaviour, right>
  {
    static_assert(left == SameAsInput,
		  "Internal error");
  };

  template <ComplexOutputBehaviour right>
  struct BehaviourMerger<AlwaysComplex, right> :
    std::integral_constant<
    ComplexOutputBehaviour, AlwaysComplex>{};

  template <ComplexOutputBehaviour right>
  struct BehaviourMerger<NeverComplex, right> :
    std::integral_constant<
    ComplexOutputBehaviour, NeverComplex>{};

  template <ComplexOutputBehaviour curr,
	    ComplexOutputBehaviour ... nexts>
  struct ComposedBehaviour<curr, nexts...> :
    BehaviourMerger<
    curr,
    ComposedBehaviour<nexts...>::value>{};

  template <ComplexOutputBehaviour last>
  struct ComposedBehaviour<last>:
    std::integral_constant<
    ComplexOutputBehaviour, last>{};

template <class ... Functions>
struct Composition :
    Function<
  list<int_<1>, typename Functions::indices...>,
  last<Functions...>::type::input_dim,
  first<Functions...>::type::output_dim>
{
  static const bool stateless =
    and_<is_stateless<Functions>...>::value;
  static const ComplexOutputBehaviour complex_output =
    ComposedBehaviour<Functions::complex_output...>::value;

  static_assert(sizeof...(Functions)>0,
		"Cannot have empty composition!");
  Composition(){}
  Composition(const Functions & ... functions):
    functions(functions...){}
  Composition(const tuple<Functions...> & f):
    functions(f){}

  template <class Arg, class ... Args,
	    class = typename std::enable_if<
	      !is_function<Arg>::value
	      >::type>
  auto operator()(Arg arg, Args ... args) const
  {
    static const int last = sizeof...(Functions)-1;
    return eval(int_<last>(), arg, args...);
  }

  auto operator()() const
  {
    static const int last = sizeof...(Functions)-1;
    ToTuple<nth<
      last-1, Functions...>::type::input_dim> t;
    return eval(int_<last-1>(),
		t(get<last>(functions)()));
  }

  template <class ... Args>
  auto eval(int_<0>, Args ... args) const
  {
    return get<0>(functions)(args...);
  }

  template <int N, class ... Args>
  auto eval(int_<N>, Args ... args) const
  {
    static const int next_ins =
      nth<N-1, Functions...>::type::input_dim;
    ToTuple<next_ins> t;
    return eval(int_<N-1>(),
		t(get<N>(functions)(args...)));
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
    return GetDelay(Composition<
		    Composition,
		    prepare<InnerFunc>>(*this, f)).Simplify();
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
    get<N>(functions).Print(s);
    s << "(";
    stream(s, int_<N+1>());
    s << ")";
  }

  template <class ... Rights>
  bool operator==(const Composition<Rights...> & t) const
  {
    return VariadicEqual(*this, t);
  }

  template <class T>
  bool operator==(const T &) const
  {
    return false;
  }

  void Print(std::ostream & s)
  {
    stream(s, int_<0>());
  }
private:
  tuple<Functions...> functions;
};

  template <class> struct is_composition : std::false_type{};

  template <class...Args>
  struct is_composition<Composition<Args...>> :
    std::true_type{};

  template <class ... Fs>
  auto ComposeRaw(Fs ... fs)
  {
    return Composition<Fs...>(fs...);
  }

template <class ... Functions>
template <class ... Args>
Composition<Group<Functions...>, Args...> Group<Functions...>::compose(Args ... args) const
{
    return ComposeRaw(*this, args...);
}
}

#endif
