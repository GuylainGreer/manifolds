#ifndef MANIFOLDS_FUNCTIONS_SIMPLIFY_HH
#define MANIFOLDS_FUNCTIONS_SIMPLIFY_HH

#include "tuple_util.hh"

#include <boost/mpl/vector.hpp>
#include <boost/mpl/next_prior.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/not.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/mpl/distance.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/if.hpp>

namespace manifolds {
//Default, don't do anything
  template <class A, class ForEnableIf = void>
struct Simplification
{
  typedef A type;

  static type Combine(A a)
  {
    return a;
  }
};

  template <template <class...> class Variadic,
	    class Lefts, class Rights, class Results>
  struct merge_and_simplify_result;

  template <template <class...> class Variadic,
	    class ... Rights, class ... Results>
  struct merge_and_simplify_result<
    Variadic, std::tuple<>, std::tuple<Rights...>,
    std::tuple<Results...>>
  {
    typedef std::tuple<Results..., Rights...> type;
  };

  template <template <class...> class Variadic,
	    class Left, class ... Lefts,
	    class ... Results>
  struct merge_and_simplify_result<
    Variadic,
    std::tuple<Left, Lefts...>,
    std::tuple<>,
    std::tuple<Results...>>
  {
    typedef std::tuple<Left, Lefts..., Results...> type;
  };

  template <template <class...> class, class>
  struct transfer;

  template <template <class...> class Variadic,
	    class ... Args>
  struct transfer<Variadic, std::tuple<Args...>>
  {
    typedef Variadic<Args...> type;
  };

  template <template <class...> class Variadic,
	    class Left, class ... Lefts,
	    class ... Rights,
	    class ... Results>
  struct merge_and_simplify_result<
    Variadic, std::tuple<Left, Lefts...>,
    std::tuple<Rights...>, std::tuple<Results...> >
  {
    typedef boost::mpl::vector<Rights...> RightVector;

    struct CanMerge
    {
      template <class T>
      struct apply
      {
	typedef typename boost::mpl::not_<
	  typename boost::is_same<
	  typename Simplification<Variadic<Left,T> >::type,
	    Variadic<Left,T>
	    >::type>::type type;
      };
    };

    static const int merge_index =
      boost::mpl::distance<
      typename boost::mpl::begin<RightVector>::type,
      typename boost::mpl::find_if<
	RightVector,
	CanMerge
	>::type>::value;

    static const bool did_merge =
      merge_index < sizeof...(Rights);

    typedef typename merge_and_simplify_result<
      Variadic, std::tuple<Lefts...>,
      std::tuple<Rights...>,
      std::tuple<Results..., Left>
      >::type no_merge_sub_result;

    typedef decltype(remove_element<
		     did_merge?merge_index:0>
		     (std::declval<std::tuple<Rights...>>()))
		    merged_next_right;

    typedef typename Simplification<
      Variadic<
	Left,
	typename std::tuple_element<
	  did_merge?merge_index:0,
	  std::tuple<Rights...>>::type>>::type simplified_type;

    typedef typename boost::mpl::if_c<
      did_merge,
      merge_and_simplify_result<
	Variadic,
	std::tuple<Lefts...>,
	merged_next_right,
        std::tuple<Results..., simplified_type>>,
      no_merge_sub_result>::type type;

  };

  template <template <class...> class Variadic,
	    class Left, class ... Lefts,
	    class ... Rights, class ... Results,
	    int merge_index>
  typename merge_and_simplify_result<
    Variadic,std::tuple<Left,Lefts...>,
    std::tuple<Rights...>,
    std::tuple<Results...>>::type
  merge_and_simplify(std::tuple<Left, Lefts...> left,
		     std::tuple<Rights...> right,
		     std::tuple<Results...> result);

  template <template <class...> class Variadic,
	    class Left, class ... Lefts,
	    class ... Rights, class ... Results,
	    int merge_index>
  typename merge_and_simplify_result<
    Variadic,std::tuple<Left,Lefts...>,
    std::tuple<Rights...>,
    std::tuple<Results...>>::type
    merge_and_simplify(std::tuple<Left, Lefts...> left,
		       std::tuple<Rights...> right,
		       std::tuple<Results...> result,
		       std::integral_constant<int, merge_index>);

  template <template <class...> class Variadic,
	    class Left, class ... Lefts,
	    class ... Rights, class ... Results>
  typename merge_and_simplify_result<
    Variadic,std::tuple<Left,Lefts...>,
    std::tuple<Rights...>,
    std::tuple<Results...>>::type
    merge_and_simplify(std::tuple<Left, Lefts...> left,
		       std::tuple<Rights...> right,
		       std::tuple<Results...> result,
		       std::integral_constant<int, -1>);

  template <template <class...> class Variadic,
	    class ... Rights, class ... Results>
  typename merge_and_simplify_result<
    Variadic, std::tuple<>, std::tuple<Rights...>,
    std::tuple<Results...>>::type
    merge_and_simplify(std::tuple<>,
		       std::tuple<Rights...> right,
		       std::tuple<Results...> result)
  {
    return std::tuple_cat(result, right);
  }

  template <template <class...> class Variadic,
	    class Left, class ... Lefts,
	    class ... Rights, class ... Results>
  typename merge_and_simplify_result<
    Variadic,std::tuple<Left,Lefts...>,
    std::tuple<Rights...>,
    std::tuple<Results...>>::type
  merge_and_simplify(std::tuple<Left, Lefts...> left,
		     std::tuple<Rights...> right,
		     std::tuple<Results...> result)
  {
    static const int merge_index =
      merge_and_simplify_result<
      Variadic,std::tuple<Left,Lefts...>,
      std::tuple<Rights...>,
      std::tuple<Results...>>::merge_index;
    static const bool did_merge =
      merge_and_simplify_result<
      Variadic,std::tuple<Left,Lefts...>,
      std::tuple<Rights...>,
      std::tuple<Results...>>::did_merge;
    return merge_and_simplify<Variadic>
      (left, right, result,
       std::integral_constant<int, did_merge?merge_index:-1>());
  }

  template <template <class...> class Variadic,
	    class Left, class ... Lefts,
	    class ... Rights, class ... Results,
	    int merge_index>
  typename merge_and_simplify_result<
    Variadic,std::tuple<Left,Lefts...>,
    std::tuple<Rights...>,
    std::tuple<Results...>>::type
    merge_and_simplify(std::tuple<Left, Lefts...> left,
		       std::tuple<Rights...> right,
		       std::tuple<Results...> result,
		       std::integral_constant<int, merge_index>)
  {
    auto merger = Simplification<
      Variadic<Left,typename std::tuple_element<
		      merge_index, std::tuple<Rights...>>::type>
      >::Combine(std::get<0>(left), std::get<merge_index>(right));
    return merge_and_simplify<Variadic>
      (remove_element<0>(left),
       remove_element<merge_index>(right),
       std::tuple_cat(result,
		      std::make_tuple(merger)));
  }

  template <template <class...> class Variadic,
	    class Left, class ... Lefts,
	    class ... Rights, class ... Results>
  typename merge_and_simplify_result<
    Variadic,std::tuple<Left,Lefts...>,
    std::tuple<Rights...>,
    std::tuple<Results...>>::type
    merge_and_simplify(std::tuple<Left, Lefts...> left,
		       std::tuple<Rights...> right,
		       std::tuple<Results...> result,
		       std::integral_constant<int, -1>)
  {
    return merge_and_simplify<Variadic>
      (remove_element<0>(left), right,
       std::tuple_cat(result,
		      std::make_tuple(std::get<0>(left))));
  }

  template <template <class...> class Variadic,
	    class ... Args1, class ... Args2>
  struct Simplification<Variadic<Variadic<Args1...>, Args2...> >
  {
    typedef typename merge_and_simplify_result<
      Variadic,
      std::tuple<Args1...>,
      std::tuple<Args2...>,
      std::tuple<>>::type tuple_type;
    typedef typename transfer<Variadic, tuple_type>::type type;

    static type Combine(Variadic<Variadic<Args1...>,
			Args2...> input)
    {
      auto tup = merge_and_simplify<Variadic>
	(std::get<0>(input.GetFunctions()).GetFunctions(),
	 remove_element<0>(input.GetFunctions()),
	 std::tuple<>());
      return type(tup);
    }
  };

  template <template <class...> class Variadic,
	    class ... Args1, class ... Args2>
  struct Simplification<Variadic<Variadic<Args1...>,
			   Variadic<Args2...>>> :
    Simplification<Variadic<Variadic<Args1...>, Args2...>>
  {
    typedef Simplification<
      Variadic<Variadic<Args1...>, Args2...>> base;
    typedef typename base::type type;

    template <size_t ... indices>
    static type Combine(Variadic<Variadic<Args1...>,
			Variadic<Args2...>> input,
			std::integer_sequence<
			size_t,indices...>)
    {
      typedef std::tuple<Args2...> tup;
      typedef Variadic<Variadic<Args1...>,
	std::tuple_element<indices, tup>...> r_type;
      return r_type(std::get<0>(input),
		    std::get<indices>(std::get<1>(input).
				      GetFunctions())...);
    }

    static type Combine(Variadic<Variadic<Args1...>,
			Variadic<Args2...>> input)
    {
      return Combine(input,
		     std::make_index_sequence<
		     sizeof...(Args2)>());
    }
  };

  template <class F>
  auto Simplify(F f)
  {
    return Simplification<F>::Combine(f);
  }
}

#endif
