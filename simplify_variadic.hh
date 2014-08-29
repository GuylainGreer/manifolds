#ifndef MANIFOLDS_FUNCTIONS_SIMPLIFY_VARIADIC_HH
#define MANIFOLDS_FUNCTIONS_SIMPLIFY_VARIADIC_HH

#include "simplify.hh"
#include "typename.hh"
#include <iostream>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/sort.hpp>
#include <boost/mpl/at.hpp>

namespace manifolds
{

  template <class Tuple>
  void PrintTuple(const Tuple & t,
		  int_<
		  tuple_size<Tuple>::value>)
  {
    std::cout << '\n';
  }

  template <class Tuple, int i,
	    class = typename std::enable_if<
	      i != tuple_size<Tuple>::value
	      >::type>
  void PrintTuple(const Tuple & t,
		  int_<i>)
  {
    std::cout << get<i>(t) << ' ';
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
    return make_my_tuple(f);
  }

  template <template<class...>class Variadic,
	    class ... Functions,
	    std::size_t ... indices>
  auto FlattenVariadic(const tuple<Functions...>& v,
		       std::integer_sequence<
		       std::size_t,indices...>)
  {
#ifdef PRINT_SIMPLIFIES
    std::cout << "Flattening: ";
#endif
    auto f =
      tuple_cat(WrapIf(bool_<IsVariadic<
		       Variadic,Functions>::value>(),
		       get<indices>(v), indices)...);
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
    return Variadic<typename tuple_element<
      indices, Tuple>::type...>
      (get<indices>(t)...);
  }

  template <template<class...>class Variadic,
	    class Tuple>
  auto MakeVariadic(Tuple t)
  {
    return MakeVariadic<Variadic>(t, std::make_index_sequence<
				  tuple_size<Tuple>::value>());
  }

  template <template<class...>class Variadic,
	    std::size_t i, std::size_t j, class Tuple>
  auto MergeV(const Tuple & t, std::false_type)
  {
    static const std::size_t max = i > j ? i : j;
    static const std::size_t min = i + j - max;
    auto middle =
      Variadic<typename tuple_element<i, Tuple>::type,
	       typename tuple_element<j, Tuple>::type
	       >(get<i>(t), get<j>(t));
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
		       int_<tuple_size<Tuple>::value-1>)
  {
    return MakeVariadic<Variadic>(t);
  }

  template <template<class...>class Variadic,
	    int i, class Tuple,
	    class = typename std::enable_if<
	      i + 1 != tuple_size<
		Tuple>::value
	      >::type>
		 auto SimplifyVHelper(const Tuple & t, int_<i>)
  {
    typedef Variadic<
    typename tuple_element<
    i, Tuple>::type,
    typename tuple_element<
    i+1, Tuple>::type
    > VType;
#ifdef PRINT_SIMPLIFIES
    VType test(get<i>(t),get<i+1>(t));
    std::cout << "Checking if " << test << " simplifies: ";
#endif
    typedef SimplifiedType<VType> SType;
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
		i + 1 != tuple_size<Tuple>::value &&
		j != tuple_size<Tuple>::value
		>::type>
    static auto MergeAllV(const Tuple & t, int_<i>, int_<j>)
    {
      typedef Variadic<
      typename tuple_element<i, Tuple>::type,
      typename tuple_element<j, Tuple>::type
      > VType;
      typedef typename Simplification<VType>::type SType;
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying:\n  " << get_cpp_name<
      typename tuple_element<i, Tuple>::type>() << "\n  "
      << get_cpp_name<
      typename tuple_element<j, Tuple>::type>() << "\ninto "
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
		i+1 != tuple_size<Tuple>::value
		>::type>
				  static auto MergeAllV(const Tuple & t, int_<i>,
							int_<tuple_size<Tuple>::value>)
    {
      return Wrapper<iter+1>::template
      MergeAllV<Variadic>(t, int_<i+1>(), int_<i+2>());
    }

    template <template<class...>class Variadic,
	      int i, class Tuple>
    static auto MergeAllV(const Tuple & t,
			  int_<tuple_size<Tuple>::value-1>,
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
    return make_my_tuple(Simplify(get<indices>(t))...);
  }

  template <class ... indices>
  struct FunctionReOrgComparator
  {
    template <class,class,class=void>
    struct apply_h;

    template <int i, int j>
    struct apply_h<int_<i>, int_<j>,
		   typename std::enable_if<
		     i != j>::type> : bool_<(i<j)>{};

    template <int i, int j, class ... Rest>
    struct apply_h<int_<i>, list<int_<j>, Rest...> >:
      bool_<(i<j)>{};

    template <int i, int j, class ... Rest>
    struct apply_h<list<int_<i>, Rest...>,
                   int_<j>>: bool_<(i<j)>{};

    template <int i, class ... Rest1, class ... Rest2>
    struct apply_h<int_<i>, list<list<Rest1...>, Rest2...>>:
      apply_h<int_<i>, list<Rest1...>>{};

    template <int i, class ... Rest1,
	      class ... Rest2>
    struct apply_h<list<list<Rest1...>, Rest2...>, int_<i>>:
      apply_h<list<Rest1...>, int_<i>>{};


    template <int i, class ... Rest3,
	      class ... Rest1, class ... Rest2>
    struct apply_h<list<int_<i>, Rest3...>,
		   list<list<Rest1...>, Rest2...>>:
      apply_h<int_<i>, list<Rest1...>>{};

    template <int i, class ... Rest3,
	      class ... Rest1, class ... Rest2>
    struct apply_h<list<list<Rest1...>, Rest2...>,
		   list<int_<i>, Rest3...>>:
      apply_h<list<Rest1...>, int_<i>>{};

    template <class ... Rest1, class ... Rest2,
	      class ... IRest1, class ... IRest2>
    struct apply_h<list<list<IRest1...>,Rest1...>,
		   list<list<IRest2...>,Rest2...> >:
    apply_h<list<IRest1...>, list<IRest2...>>{};

    template <class ... Rest1, class ... Rest2,
	      class ... IRest>
    struct apply_h<list<list<IRest...>,Rest1...>,
		   list<list<IRest...>,Rest2...> >:
    apply_h<list<Rest1...>, list<Rest2...>>{};

    template <int i, int j, class ...Rest1,
              class ... Rest2>
    struct apply_h<list<int_<i>, Rest1...>,
                   list<int_<j>, Rest2...>,
		   typename std::enable_if<
		     i != j>::type>:
    bool_<(i<j)>{};

    template <class T>
    struct apply_h<T, T>: std::false_type{};

    template <int i, class ... Rest1, class ... Rest2>
    struct apply_h<list<int_<i>, Rest1...>,
                   list<int_<i>, Rest2...>,
		   typename std::enable_if<
		     !are_same<
		       holder<Rest1...>,
		       holder<Rest2...>
		       >::value
		     >::type>:
          apply_h<list<Rest1...>, list<Rest2...>>{};

    template <class Void>
    struct apply_h<list<>, list<>,Void>:
      std::true_type{};

    template <class i1, class i2>
    struct apply : apply_h<
      typename nth<i1::value, indices...>::type,
      typename nth<i2::value, indices...>::type>{};
  };

  template <template<class...>class V, class ... Functions,
	    std::size_t ... tis>
  auto SortElements(V<Functions...> v, std::integer_sequence<
		    std::size_t, tis...>)
  {
    auto t = v.GetFunctions();
    typedef decltype(t) Tuple;
    typedef boost::mpl::range_c<
      std::size_t, 0, tuple_size<Tuple>::value> indices;
    typedef typename boost::mpl::copy<
      indices, boost::mpl::back_inserter<
		 boost::mpl::vector<>>>::type indices_v;
    typedef typename boost::mpl::sort<
      indices_v, FunctionReOrgComparator<
		   typename tuple_element<
		     tis, Tuple>::type::indices...>
      >::type sorted_indices;
    return V<typename nth<
      boost::mpl::at_c<
	sorted_indices, tis>::type::value,
	       Functions...>::type...>
      (make_my_tuple(get<boost::mpl::at_c<
		     sorted_indices, tis>::type::value>(t)...));
  }

  template <template<class...>class V, class Tuple,
	    bool b>
  auto SimplifyTheV(const Tuple & t, int_<1>, bool_<b>)
  {
    return get<0>(t);
  }

  template <template<class...>class V, class Tuple>
  auto SimplifyTheV(const Tuple & t, int_<2>, std::false_type)
  {
    return MakeVariadic<V>(t);
  }

  template <template<class...>class V, class Tuple>
  auto SimplifyTheV(const Tuple & t, int_<2>, std::true_type)
  {
    return SortElements(MakeVariadic<V>(t),
			 std::make_index_sequence<2>());
  }

  template <template<class...>class V, class Tuple,
	    int i, class=typename std::enable_if<
		     (i>2)>::type>
  auto SimplifyTheV(const Tuple & t, int_<i>, std::true_type)
  {
    auto result = Wrapper<>::template
      MergeAllV<V>(t, int_<0>(), int_<1>());
    static const int size =
      tuple_size<decltype(result)>::value;
    return SortElements(MakeVariadic<V>(result),
			std::make_index_sequence<size>());
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
			 tuple_size<Tuple>::value>());
    auto t3 =
      SimplifyIndividuals(t2, std::make_index_sequence<
			  tuple_size<
			  decltype(t2)>::value>());
    static const int size =
      tuple_size<decltype(t3)>::value;
    return SimplifyTheV<V>(t3, int_<size>(), b);
  }

  template <class TupleOut, class TupleIn,
	    std::size_t ... iouts>
  auto DistributeComposition(TupleOut to,
			     TupleIn ti,
			     std::integer_sequence<
			     std::size_t,iouts...>)
  {
    return make_my_tuple(get<iouts>(to)(ti)...);
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
    auto it = get<1>(t);
    auto ot = get<0>(t).GetFunctions();
    auto tr =
      DistributeComposition(ot, it,
			    std::make_index_sequence<
			    tuple_size<
			    decltype(ot)>::value>());
    return MakeVariadic<Variadic>(tr);
  }
}

#endif
