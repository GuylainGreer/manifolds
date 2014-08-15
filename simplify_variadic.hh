#ifndef MANIFOLDS_FUNCTIONS_SIMPLIFY_VARIADIC_HH
#define MANIFOLDS_FUNCTIONS_SIMPLIFY_VARIADIC_HH

#include "simplify.hh"
#include "typename.hh"
#include <tuple>
#include <iostream>

namespace manifolds
{

  template <class Tuple>
  void PrintTuple(const Tuple & t,
		  int_<
		  std::tuple_size<Tuple>::value>)
  {
    std::cout << '\n';
  }

  template <class Tuple, int i,
	    class = typename std::enable_if<
	      i != std::tuple_size<Tuple>::value
	      >::type>
  void PrintTuple(const Tuple & t,
		  int_<i>)
  {
    std::cout << std::get<i>(t) << ' ';
    PrintTuple(t, int_<i+1>());
  }

  template <class F>
  auto WrapIf(bool_<true>, F f, int i)
  {
#ifdef PRINT_SIMPLIFIES
    std::cout << "(" << i << ", true) ";
#endif
    return f.GetFunctions();
  }

  template <class F>
  auto WrapIf(bool_<false>, F f, int i)
  {
#ifdef PRINT_SIMPLIFIES
    std::cout << "(" << i << ", false) ";
#endif
    return std::make_tuple(f);
  }

  template <template<class...>class Variadic,
	   class ... Functions,
	   std::size_t ... indices>
  auto FlattenVariadic(const std::tuple<Functions...>& v,
		       std::integer_sequence<
		       std::size_t,indices...>)
  {
#ifdef PRINT_SIMPLIFIES
    std::cout << "Flattening: ";
#endif
    auto f =
      std::tuple_cat(WrapIf(bool_<IsVariadic<
			    Variadic,Functions>::value>(),
			    std::get<indices>(v), indices)...);
#ifdef PRINT_SIMPLIFIES
    std::cout << '\n';
    std::cout << "Flattened: ";PrintTuple(f,int_<0>());
#endif
    return f;
  }

  template <template<class...>class Variadic,
	    class Tuple, std::size_t ... indices>
  auto MakeVariadic(const Tuple & t,
		    std::integer_sequence<
		    std::size_t, indices...>)
  {
    return Variadic<typename std::tuple_element<
		      indices, Tuple>::type...>
      (std::get<indices>(t)...);
  }

  template <template<class...>class Variadic,
	    class Tuple>
  auto MakeVariadic(Tuple t)
  {
    return MakeVariadic<Variadic>(t, std::make_index_sequence<
				  std::tuple_size<Tuple>::value>());
  }

  template <template<class...>class Variadic,
	    std::size_t i, std::size_t j, class Tuple>
  auto MergeV(const Tuple & t, std::false_type)
  {
    static const std::size_t max = i > j ? i : j;
    static const std::size_t min = i + j - max;
    auto middle =
      Variadic<typename std::tuple_element<i, Tuple>::type,
	       typename std::tuple_element<j, Tuple>::type
	       >(std::get<i>(t), std::get<j>(t));
    return
      insert_element<i>
      (remove_element<min>
       (remove_element<max>(t)), Simplify(middle));
  }

  template <template<class...>class,
	    std::size_t i, std::size_t j, class Tuple>
  auto MergeV(const Tuple & t, std::true_type)
  {
    return t;
  }

  template <template<class...>class Variadic,
	    class Tuple>
  auto SimplifyVHelper(const Tuple & t,
		       int_<std::tuple_size<Tuple>::value-1>)
  {
    return MakeVariadic<Variadic>(t);
  }

  template <template<class...>class Variadic,
	    int i, class Tuple,
	    class = typename std::enable_if<
	      i + 1 != std::tuple_size<
		Tuple>::value
	      >::type>
  auto SimplifyVHelper(const Tuple & t, int_<i>)
  {
    typedef Variadic<
      typename std::tuple_element<
	i, Tuple>::type,
      typename std::tuple_element<
	i+1, Tuple>::type
      > VType;
    typedef typename Simplification<VType>::type SType;
    typedef typename std::is_same<
      VType, SType>::type Simplified;
    auto next =
      MergeV<Variadic,i,i+1>(t, Simplified());
    return SimplifyVHelper<Variadic>
    (next, int_<i+Simplified::value>());
  }

  template <std::size_t iter = 0>
  struct Wrapper
  {

    template <template<class...>class Variadic,
	      int i, class Tuple, int j,
	      class = typename std::enable_if<
		i + 1 != std::tuple_size<Tuple>::value &&
		j != std::tuple_size<Tuple>::value
		>::type>
    static auto MergeAllV(const Tuple & t, int_<i>, int_<j>)
    {
      typedef Variadic<
	typename std::tuple_element<i, Tuple>::type,
	typename std::tuple_element<j, Tuple>::type
	> VType;
      typedef typename Simplification<VType>::type SType;
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying:\n  " << get_cpp_name<
      typename std::tuple_element<i, Tuple>::type>() << "\n  "
      << get_cpp_name<
      typename std::tuple_element<j, Tuple>::type>() << "\ninto "
      << get_cpp_name<SType>() << "\n";
#endif
      typedef typename std::is_same<
	SType, VType>::type Simplified;
      auto next = MergeV<Variadic, i, j>(t, Simplified());
      return Wrapper<iter+1>::template
	MergeAllV<Variadic>(next, int_<i>(),
			    int_<j+Simplified::value>());
    }

    template <template<class...>class Variadic,
	      int i, class Tuple,
	      class = typename std::enable_if<
		i+1 != std::tuple_size<Tuple>::value
		>::type>
    static auto MergeAllV(const Tuple & t, int_<i>,
			  int_<std::tuple_size<Tuple>::value>)
    {
      return Wrapper<iter+1>::template
	MergeAllV<Variadic>(t, int_<i+1>(), int_<i+2>());
    }

    template <template<class...>class Variadic,
	      int i, class Tuple>
    static auto MergeAllV(const Tuple & t,
			  int_<std::tuple_size<Tuple>::value-1>,
			  int_<i>)
    {
      return t;
    }
  };

  template <class Tuple, std::size_t ... indices>
  auto SimplifyIndividuals(const Tuple & t,
			   std::integer_sequence<std::size_t,
			   indices...>)
  {
    return std::make_tuple(Simplify(std::get<indices>(t))...);
  }

  template <template<class...>class V, class Tuple,
	    bool b>
  auto SimplifyTheV(const Tuple & t, int_<1>, bool_<b>)
  {
    return std::get<0>(t);
  }

  template <template<class...>class V, class Tuple,
	    bool b>
  auto SimplifyTheV(const Tuple & t, int_<2>, bool_<b>)
  {
    return MakeVariadic<V>(t);
  }

  template <template<class...>class V, class Tuple,
	    int i, class=typename std::enable_if<
		     (i>2)>::type>
  auto SimplifyTheV(const Tuple & t, int_<i>, std::true_type)
  {
    auto result = Wrapper<>::template
      MergeAllV<V>(t, int_<0>(), int_<1>());
    return MakeVariadic<V>(result);
  }

  template <template<class...>class V,
	    class Tuple, int i,
	    class=typename std::enable_if<
	      (i > 2)>::type>
  auto SimplifyTheV(const Tuple & t, int_<i>, std::false_type)
  {
    return SimplifyVHelper<V>(t, int_<0>());
  }

  template <template<class...>class V, class Tuple,
	    bool abelian>
  auto SimplifyV(const Tuple & t, bool_<abelian> b)
  {
    auto t2 =
      FlattenVariadic<V>(t, std::make_index_sequence<
			 std::tuple_size<Tuple>::value>());
    auto t3 =
      SimplifyIndividuals(t2, std::make_index_sequence<
			  std::tuple_size<
			  decltype(t2)>::value>());
    static const int size =
      std::tuple_size<decltype(t3)>::value;
    return SimplifyTheV<V>(t3, int_<size>(), b);
  }

  template <class TupleOut, class TupleIn,
	    std::size_t ... iouts>
  auto DistributeComposition(TupleOut to,
			     TupleIn ti,
			     std::integer_sequence<
			     std::size_t,iouts...>)
  {
    return std::make_tuple(std::get<iouts>(to)(ti)...);
  }

  //Careful when calling this!
  //Outer needs to be an instance of Variadic<...>
  //and Inner needs to be an instance of FunctionMatrix<...>
  template <template<class...>class Variadic,
	    template<class...>class Composition,
	    class Outer, class Inner>
  auto DistributeComposition(Composition<Outer,Inner> c)
  {
    auto t = c.GetFunctions();
    auto it = std::get<1>(t);
    auto ot = std::get<0>(t).GetFunctions();
    auto tr =
      DistributeComposition(ot, it,
			    std::make_index_sequence<
			    std::tuple_size<
			    decltype(ot)>::value>());
    return MakeVariadic<Variadic>(tr);
  }
}

#endif
