#ifndef MANIFOLDS_FUNCTIONS_SIMPLIFY_VARIADIC_HH
#define MANIFOLDS_FUNCTIONS_SIMPLIFY_VARIADIC_HH

#include "simplify.hh"
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

  template <template<class...>class Variadic,
	    class Tuple, std::size_t ... indices>
  auto SimplifyVMakeVariadic(const Tuple & t,
			     std::integer_sequence<
			     std::size_t, indices...>)
  {
    return Variadic<typename std::tuple_element<
		      indices, Tuple>::type...>
      (std::get<indices>(t)...);
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
    return SimplifyVMakeVariadic<Variadic>
      (t, std::make_index_sequence<
       std::tuple_size<Tuple>::value>());
  }

  template <template<class...>class Variadic,
	    int i, class Tuple,
	    class = typename std::enable_if<
	      i + 1 != std::tuple_size<
		Tuple>::value
	      >::type>
  auto SimplifyVHelper(const Tuple & t,
		       int_<i>)
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

  template <template<class...>class Variadic,
	    class Tuple>
  auto SimplifyV(const Tuple & t, std::false_type)
  {
    return SimplifyVHelper<Variadic>(t, int_<0>());
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

  template <template<class...>class Variadic, class Tuple>
  auto SimplifyV(const Tuple & t, std::true_type)
  {
    auto result = Wrapper<>::template
      MergeAllV<Variadic>(t, int_<0>(), int_<1>());
    return SimplifyVMakeVariadic<Variadic>
      (result, std::make_index_sequence<
       std::tuple_size<decltype(result)>::value>());
  }
}

#endif
