#ifndef MANIFOLDS_FUNCTIONS_FACTOR_EXPRESSIONS_HH
#define MANIFOLDS_FUNCTIONS_FACTOR_EXPRESSIONS_HH

#include "variables.hh"
#include "concepts.hh"
#include <boost/mpl/map.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/erase_key.hpp>
#include <boost/mpl/sort.hpp>
#include <boost/mpl/unique.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/less.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/zip_view.hpp>
#include <utility>

namespace boost {
namespace mpl {
template <class T, T t>
struct less_tag<std::integral_constant<T, t> > : less_tag<integral_c<T, t> > {};
}
}

namespace manifolds {

template <class A, class B, bool = (A::value < B::value)> struct max_t : B {};

template <class A, class B> struct max_t<A, B, false> : A {};

template <class F, class = void>
struct HighestVariable : std::integral_constant<int, -1> {};

template <class F, class... Fs>
struct HighestVariable<
    std::tuple<F, Fs...> > : max_t<HighestVariable<F>,
                                   HighestVariable<std::tuple<Fs...> > > {};

template <>
struct HighestVariable<std::tuple<> > : std::integral_constant<int, -1> {};

template <int i, bool a, bool c>
struct HighestVariable<Variable<i, a, c> > : std::integral_constant<int, i> {};

template <class F>
struct HighestVariable<F, IsFunctionContainer<F> > : HighestVariable<decltype(
                                                         std::declval<F>()
                                                             .GetFunctions())> {
};

template <class F, class = void> struct Factors {
  typedef typename std::conditional<
      F::stateless, boost::mpl::map<boost::mpl::pair<F, int_<1> > >,
      boost::mpl::map<> >::type type;
};

template <class Map, class Key,
          bool = boost::mpl::has_key<Map, Key>::type::value,
          bool = is_stateless<Key>::value>
struct InsertOrIncrement {
  typedef typename boost::mpl::insert<
      typename boost::mpl::erase_key<Map, Key>::type,
      boost::mpl::pair<
          Key, int_<boost::mpl::at<Map, Key>::type::value + 1> > >::type type;
};

template <class Map, class Key, bool stateless>
struct InsertOrIncrement<Map, Key, false, stateless> {
  typedef typename std::conditional<
      stateless, boost::mpl::insert<Map, boost::mpl::pair<Key, int_<1> > >,
      boost::mpl::map<> >::type::type type;
};

template <class, class, int> struct PowerSetIndicesImpl;

template <class... Sequences, std::size_t... indices, int count>
struct PowerSetIndicesImpl<std::tuple<Sequences...>,
                           std::integer_sequence<std::size_t, indices...>,
                           count> {
  template <class, std::size_t> struct Add;

  template <std::size_t... is, std::size_t i>
  struct Add<std::integer_sequence<std::size_t, is...>, i> {
    typedef std::integer_sequence<std::size_t, i, is...> type;
  };

  template <class Sequence, std::size_t... is> struct AddAll {
    typedef std::tuple<typename Add<Sequence, is>::type...> type;
  };

  typedef decltype(std::tuple_cat(
      typename AddAll<Sequences, indices...>::type()...,
      std::tuple<std::integer_sequence<std::size_t, indices>...>())) NextIter;
  typedef typename PowerSetIndicesImpl<
      NextIter, std::integer_sequence<std::size_t, indices...>, count - 1>::type
  type;
  typedef typename PowerSetIndicesImpl<
      NextIter, std::integer_sequence<std::size_t, indices...>,
      count - 1>::type_old type_old;
};

template <class Vector, class Obj, class... Objs> struct CreateVector {
  typedef typename boost::mpl::push_back<
      typename CreateVector<Vector, Objs...>::type, Obj>::type type;
};

template <class Vector, class Obj> struct CreateVector<Vector, Obj> {
  typedef boost::mpl::vector<Obj> type;
};

template <class Container, class Tuple> struct TupleToContainer;

template <class Container, class... Ts>
struct TupleToContainer<Container, std::tuple<Ts...> > : CreateVector<Container,
                                                                      Ts...> {};

template <class... Sets, std::size_t... indices>
struct PowerSetIndicesImpl<std::tuple<Sets...>,
                           std::integer_sequence<std::size_t, indices...>, 0> {
  template <class> struct Sort;

  struct Samer {
    template <class A, class B> using apply = std::is_same<A, B>;
  };

  template <class... Objects> struct Sort<std::tuple<Objects...> > {
    struct VecComp {

      template <class Left, class Right, class = void> struct apply {
        static const bool fronts_same = (boost::mpl::front<Left>::type::value ==
                                         boost::mpl::front<Right>::type::value);
        typedef typename eval_if_t<
            fronts_same, apply<typename boost::mpl::pop_front<Left>::type,
                               typename boost::mpl::pop_front<Right>::type>,
            bool_<(boost::mpl::front<Left>::type::value <
                   boost::mpl::front<Right>::type::value)> >::type type;
      };

      template <class Left, class Right>
      struct apply<Left, Right,
                   typename std::enable_if<
                       !boost::mpl::size<Left>::type::value ||
                       !boost::mpl::size<Right>::type::value>::type> {
        typedef bool_<(boost::mpl::size<Left>::type::value <
                       boost::mpl::size<Right>::type::value)> type;
      };
    };

    typedef typename boost::mpl::sort<
        typename CreateVector<boost::mpl::vector<>, Objects...>::type,
        VecComp>::type Sorted;
    typedef typename boost::mpl::unique<Sorted, Samer>::type type;
  };

  template <std::size_t... is>
  struct Sort<std::integer_sequence<std::size_t, is...> > {
    typedef typename CreateVector<
        boost::mpl::vector<>, std::integral_constant<std::size_t, is>...>::type
    Vector;
    typedef typename boost::mpl::sort<Vector>::type Sorted;
    typedef typename boost::mpl::unique<Sorted, Samer>::type type;
  };

  struct AppendSeq {
    template <class, class> struct apply;

    template <std::size_t... is, class Next>
    struct apply<std::integer_sequence<std::size_t, is...>, Next> {
      typedef std::integer_sequence<std::size_t, is..., Next::value> type;
    };
  };

  template <class Set>
  using Transfer = typename boost::mpl::fold<
      Set, std::integer_sequence<std::size_t>, AppendSeq>::type;

  typedef std::tuple<Transfer<typename Sort<Sets>::type>...> type_old;

  typedef typename Sort<std::tuple<typename Sort<Sets>::type...> >::type
  SortedVector;

  struct AppendTuple {
    template <class, class> struct apply;
    template <class... Objects, class Next>
    struct apply<std::tuple<Objects...>, Next> {
      typedef std::tuple<Objects..., Transfer<Next> > type;
    };
  };

  typedef typename boost::mpl::fold<SortedVector, std::tuple<>,
                                    AppendTuple>::type type;
};

template <int N> struct PowerSetIndices {
  typedef typename PowerSetIndicesImpl<
      std::tuple<>, std::make_index_sequence<N>, N>::type type;
  typedef typename PowerSetIndicesImpl<
      std::tuple<>, std::make_index_sequence<N>, N>::type_old type_old;
};

template <template <class...> class Container, class... Funcs>
struct Factors<Container<Funcs...>,
               IsReorderableFunctionContainer<Container<Funcs...> > > {
  template <class> struct SubFactors;

  template <class... Fs> struct SubFactors<std::tuple<Fs...> > {
    typedef boost::mpl::vector<typename Factors<Fs>::type...> type;
  };

  struct Merger2 {
    template <class Map, class Next> struct apply {
      typedef typename InsertOrIncrement<Map, typename Next::first>::type type;
    };
  };

  struct Merger1 {
    template <class OverallMap, class CurrentMap> struct apply {
      typedef typename boost::mpl::fold<CurrentMap, OverallMap, Merger2>::type
      type;
    };
  };

  typedef typename boost::mpl::fold<
      typename SubFactors<
          decltype(std::declval<Container<Funcs...> >().GetFunctions())>::type,
      boost::mpl::map<>, Merger1>::type Individuals;

  typedef typename PowerSetIndices<sizeof...(Funcs)>::type power_set_indices;

  struct AddCombo {
    template <class Map, class Next> struct apply {};

    template <class Map, std::size_t... indices>
    struct apply<Map, std::integer_sequence<std::size_t, indices...> > {
      struct Adder {
        template <class M, class N> struct apply {
          typedef typename InsertOrIncrement<
              M, typename nth<N::value, Funcs...>::type>::type type;
        };
      };
      typedef InsertOrIncrement<
          Map, Container<typename nth<indices, Funcs...>::type...> > Inserted;
      typedef typename eval_if_t<(sizeof...(indices) > 1), Inserted, Map>::type
      type;
    };
  };

  typedef typename boost::mpl::fold<
      typename TupleToContainer<boost::mpl::vector<>, power_set_indices>::type,
      Individuals, AddCombo>::type type;
};

template <class... Fs> struct Factors<Composition<Fs...> > {
  template <class> struct SubFactors;

  template <class F, class... F2s> struct SubFactors<Composition<F, F2s...> > {
    typedef typename SubFactors<Composition<F2s...> >::type SubType;
    typedef InsertOrIncrement<SubType, Composition<F, F2s...> > InsertExpr;
    typedef typename std::conditional<
        is_stateless<Composition<F, F2s...> >::value, InsertExpr,
        SubType>::type::type type;
  };

  template <class F> struct SubFactors<Composition<F> > {
    typedef typename std::conditional<
        is_variable<F>::value, boost::mpl::map<>,
        boost::mpl::map<boost::mpl::pair<F, int_<1> > > >::type type;
  };

  typedef typename SubFactors<Composition<Fs...> >::type type;
};

template <class F> struct Factors<F, Variable_c<F> > {
  typedef boost::mpl::map<> type;
};

template <class FactorsMap> struct Filter1OffFactors {
  struct Adder {
    template <class Map, class Next> struct apply {
      typedef typename eval_if_t<(Next::second::value > 1),
                                 boost::mpl::insert<Map, typename Next::first>,
                                 Map>::type type;
    };
  };

  typedef typename boost::mpl::fold<FactorsMap, boost::mpl::set<>, Adder>::type
  type;
};

template <class F> struct AssignVariables {
  static const std::size_t nHighestVarIndex = HighestVariable<F>::type::value;

  typedef typename Factors<F>::type AllFactors;
  typedef typename Filter1OffFactors<AllFactors>::type Factors;

  typedef boost::mpl::range_c<std::size_t, nHighestVarIndex + 1,
                              nHighestVarIndex + 1 +
                                  boost::mpl::size<Factors>::type::value>
  NewVarIndices;

  struct Add {
    template <class Map, class T> struct apply {
      typedef typename boost::mpl::insert<
          Map, boost::mpl::pair<
                   typename boost::mpl::at_c<T, 0>::type,
                   Variable<boost::mpl::at_c<T, 1>::type::value> > >::type type;
    };
  };

  typedef typename boost::mpl::fold<
      boost::mpl::zip_view<boost::mpl::vector<Factors, NewVarIndices> >,
      boost::mpl::map<>, Add>::type type;
};

template <class ExprToVarMap, class F>
auto FactorSingle(F f, boost::mpl::false_) {
  return f;
}

template <class ExprToVarMap, class F>
auto FactorSingle(F f, boost::mpl::true_) {
  return typename boost::mpl::at<ExprToVarMap, F>::type{};
}

template <class ExprToVarMap, class F> auto FactorSingle(F f) {
  FactorSingle(f, typename boost::mpl::has_key<ExprToVarMap, F>::type());
}

template <class ExprToVarMap, class F> auto ApplyFactoring(F f) {
  return FactorSingle(f);
}

template <template <class...> class Container, class ExprToVarMap>
struct GetContainerContents {
  struct AddTo {
    template <class Vector, class Next> struct apply {
      typedef Vector type;
    };

    template <class Vector, class... Funcs>
    struct apply<Vector, Container<Funcs...> > {
      typedef typename boost::mpl::push_back<
          Vector, boost::mpl::vector<Funcs...> >::type type;
    };
  };
};

template <class ExprToVarMap, template <class...> class Container, class... Fs,
          class = IsReorderableFunctionContainer<Container<Fs...> > >
auto ApplyFactoring(Container<Fs...> c) {}
}

#endif
