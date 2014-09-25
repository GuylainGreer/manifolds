#ifndef MANIFOLDS_FUNCTIONS_TUPLE_UTILITIES_HH
#define MANIFOLDS_FUNCTIONS_TUPLE_UTILITIES_HH

#include <utility>
#include "function.hh"
#include "tmp_utils.hh"
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/sort.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/size.hpp>
#include <array>
#include <tuple>

namespace manifolds {

  template <class ... Args>
  struct tuple;

  template <class F, class ... Args>
  struct tuple<F,Args...>
  {
    static const int num_elements = 1 + sizeof...(Args);
    typedef std::make_integer_sequence<
      int, 1+sizeof...(Args)> indices;
    char buffer[1+sum<sizeof(F), sizeof(Args)...>::value];
    template <class...As>
    void dummy(As...){}

    template <int i>
      struct offset : sum_n<i, sizeof(F), sizeof(Args)...>{};

    template <int ... is, class ... Fs>
    void fill_with(std::integer_sequence<int,is...>,
		   Fs ... fs)
    {
      dummy(((*((Fs*)(buffer+offset<is>::value)) = fs),0)...);
    }

    tuple()
    {
      fill_with(indices(), F(), Args()...);
    }

    tuple(F f, Args ... args)
    {
      fill_with(indices(), f, args...);
    }

    template <class ... Fs, int ... is>
    void fill_from(const tuple<Fs...> & t,
		   std::integer_sequence<
		   int,is...> i)
    {
      dummy(((*(Fs*)(buffer+offset<is>::value) =
	      *(const Fs*)(t.buffer + offset<is>::value)), 0)...);
    }

    tuple(const tuple & t)
    {
      fill_from(t, indices());
    }
  };

  template <>
  struct tuple<>
  {
    static const int num_elements = 0;
  };

  template <class ... Args>
  tuple<Args...> make_my_tuple(Args ... args)
  {
    return {args...};
  }

  template <class ... Ts, class ... Args>
  auto make_same_as(const std::tuple<Ts...> &,
		    Args ... args)
  {
    return std::make_tuple(args...);
  }

  template <class ... Ts, class ... Args>
  auto make_same_as(const tuple<Ts...> &,
		    Args ... args)
  {
    return make_my_tuple(args...);
  }

  template <int n, class ... Args>
  auto & get(tuple<Args...> & t)
  {
    typedef typename nth<n, Args...>::type type;
    return *(type*)(t.buffer+tuple<Args...>::
		    template offset<n>::value);
  }

  template <int n, class ... Args>
  const auto & get(const tuple<Args...> & t)
  {
    typedef typename nth<n, Args...>::type type;
    return *(const type*)(t.buffer+tuple<Args...>::
		    template offset<n>::value);
  }

  inline bool operator==(tuple<>, tuple<>)
  {
    return true;
  }

  template <int n, class Tuple>
  struct tuple_element;

  template <int n, class ... Args>
  struct tuple_element<n, tuple<Args...>> :
    nth<n, Args...>{};

  template <class T>
  struct tuple_size : int_<T::num_elements>{};

  template <class ... As>
  struct tuple_size<std::tuple<As...>> :
    int_<sizeof...(As)>{};

  template <class T1, class T2,
	    std::size_t ... indices1,
	    std::size_t ... indices2>
  auto tuple_cat(const T1 & t1, const T2 & t2,
		 std::integer_sequence<
		 std::size_t, indices1...>,
		 std::integer_sequence<
		 std::size_t, indices2...>)
  {
    using namespace std;
    return make_same_as(t1, get<indices1>(t1)...,
			get<indices2>(t2)...);
  }

  inline auto tuple_cat()
  {
    return tuple<>();
  }

  template <class T>
  auto tuple_cat(const T & t)
  {
    return t;
  }

  template <class T1, class T2>
  auto tuple_cat(const T1 & t1, const T2 & t2)
  {
    return tuple_cat(t1, t2, std::make_index_sequence<
		     tuple_size<T1>::value>(),
		     std::make_index_sequence<
		     tuple_size<T2>::value>());
  }

  template <class Tuple1, class Tuple2, class ... Tuples>
  auto tuple_cat(const Tuple1 & t1,
		 const Tuple2 & t2,
		 const Tuples & ... tuples)
  {
    return tuple_cat(tuple_cat(t1,t2), tuples...);
  }

  template <class Tuple, std::size_t ... befores,
	    std::size_t ... afters>
  auto remove_element_helper(const Tuple & t,
			     std::integer_sequence<
			     std::size_t, befores...>,
			     std::integer_sequence<
			     std::size_t, afters...>)
  {
    using namespace std;
    static const std::size_t offset = sizeof...(befores)+1;
    auto r = make_same_as(t, get<befores>(t)...,
			  get<afters+offset>(t)...);
    return r;
  }

  template <int index, class Tuple>
  auto remove_element(const Tuple & t)
  {
    typedef std::make_index_sequence<index> befores;
    typedef std::make_integer_sequence<
      std::size_t, tuple_size<Tuple>::value-index-1>
      afters;
    return remove_element_helper(t, befores(), afters());
  }

  template <class Tuple>
  auto remove_element(const Tuple & t,
		      boost::mpl::vector<>)
  {
    return t;
  }


  template <class A, class ... As,
	    class B, class ... Bs>
  bool operator==(const tuple<A,As...>& a,
		  const tuple<B,Bs...>& b)
  {
    return get<0>(a) == get<0>(b) &&
      remove_element<0>(a) == remove_element<0>(b);
  }

    template <class Tuple, class Vector,
              class = typename std::enable_if<
                  boost::mpl::size<Vector>::value == 0>::type>
        auto remove_elements(const Tuple & t, Vector, long)
    {
        return t;
    }

    template <class Tuple, class Vector,
              class = typename std::enable_if<
                  boost::mpl::size<Vector>::value != 0>::type>
    auto remove_elements(const Tuple & t, Vector, int)
    {
        return remove_element<
            boost::mpl::at_c<Vector, 0>::type::value>
            (remove_elements(t, typename boost::mpl::pop_front<
                                 Vector>::type(), 0));
    }

  template <int ... indices, class Tuple>
  auto remove_elements(const Tuple & t)
  {
    typedef typename boost::mpl::sort<
      boost::mpl::vector_c<int,indices...>
      >::type sorted_list;
    return remove_elements(t, sorted_list(), 0);
  }

  template <int index, class Tuple, class E,
	    std::size_t ... befores,
	    std::size_t ... afters>
  auto insert_element_helper(const Tuple & t,
			     const E & e,
			     std::integer_sequence<
			     std::size_t, befores...>,
			     std::integer_sequence<
			     std::size_t, afters...>)
  {
    using namespace std;
    auto before = make_same_as(t, get<befores>(t)...);
    auto middle = make_same_as(t, e);
    auto after  = make_same_as(t, get<afters+index>(t)...);
    return tuple_cat(before, middle, after);
  }

  template <int index, class Tuple, class E>
  auto insert_element(const Tuple & t, const E & e)
  {
    return insert_element_helper<index>
      (t, e, std::make_index_sequence<index>(),
       std::make_index_sequence<
       tuple_size<Tuple>::value - index>());
  }

  template <class Tuple, class E>
  auto push_back(const Tuple & t, const E & e)
  {
    return insert_element<
      tuple_size<Tuple>::value>(t, e);
  }

  template <class T>
  struct array_size;

  template <class T, std::size_t n>
  struct array_size<std::array<T,n>> :
    int_<n>{};

  template <class T, std::size_t n>
  struct array_size<const std::array<T,n> &> :
    int_<n>{};

  template <class T, std::size_t n>
  struct array_size<std::array<T,n> &> :
    int_<n>{};

  template <class T, std::size_t n>
  struct array_size<T[n]> :
    int_<n>{};

  template <class T, std::size_t n>
  struct array_size<T(&)[n]> :
    int_<n>{};

  template <class T, std::size_t n>
  struct array_size<const T(&)[n]> :
    int_<n>{};

  FUNC_CHECK_DEF(is_gettable,
		 decltype(get<0>(std::declval<T>())))

  template <int start, int size, class Tuple>
  auto subset_helper(const Tuple & t, int_<start>, int_<size>)
  {
      typedef boost::mpl::range_c<int, 0, start> first_r;
      typedef typename boost::mpl::copy<
          first_r, boost::mpl::back_inserter<
              boost::mpl::vector<>>>::type first_v;
      typedef typename boost::mpl::if_c<
	(start + size > tuple_size<Tuple>::value),
	boost::mpl::range_c<int,0,0>,
	boost::mpl::range_c<
	int, start + size,
	tuple_size<Tuple>::value>>::type second_r;
      typedef typename boost::mpl::copy<
          second_r, boost::mpl::back_inserter<
          boost::mpl::vector<>>>::type second_v;
    return remove_elements
        (remove_elements(t, second_v(), 0), first_v(), 0);
  }

  template <int start, class Tuple>
  auto subset_helper(const Tuple&, int_<start>, int_<0>)
  {
    return tuple<>();
  }

  template <int start, int size, class Tuple>
  auto subset(const Tuple & t)
  {
    return subset_helper(t, int_<start>(), int_<size>());
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

  template <class ... Types, std::size_t ... is>
  auto ToArray(const tuple<Types...> & t,
	       std::integer_sequence<
	       std::size_t, is...>)
  {
    return std::array<
      typename std::common_type<
	Types...>::type,
      sizeof...(Types)>{{get<is>(t)...}};
  }

  template <class ... Types>
  auto ToArray(const tuple<Types...> & t)
  {
    return ToArray(t, std::index_sequence_for<Types...>());
  }
}

#endif
