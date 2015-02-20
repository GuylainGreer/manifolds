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
#include <utility>

namespace manifolds {

template <class A, class B, bool = (A::value < B::value)>
struct max_t : B {};

template <class A, class B>
struct max_t<A, B, false> : A {};

template <class F, class = void>
struct HighestVariable :
    std::integral_constant<int, -1>{};

template <class F, class ... Fs>
struct HighestVariable<std::tuple<F, Fs...> > :
    max_t<HighestVariable<F>, HighestVariable<std::tuple<Fs...> > >{};

template <>
struct HighestVariable<std::tuple<> > :
    std::integral_constant<int, -1>{};

template <int i, bool a, bool c>
struct HighestVariable<Variable<i, a, c> > :
        std::integral_constant<int, i>{};

template <class F>
struct HighestVariable<F, IsFunctionContainer<F> > :
    HighestVariable<decltype(std::declval<F>().GetFunctions())>{};

    template <class F, class = void>
    struct Factors
    {
        typedef typename std::conditional<
            F::stateless,
            boost::mpl::map<boost::mpl::pair<F, int_<1> > >,
            boost::mpl::map<> >::type type;
    };

    template <class Map, class Key, bool = boost::mpl::has_key<Map, Key>::type::value,
              bool = is_stateless<Key>::value>
    struct InsertOrIncrement
    {
        typedef typename boost::mpl::insert<
            typename boost::mpl::erase_key<Map, Key>::type,
            boost::mpl::pair<
                Key,
                int_<boost::mpl::at<Map, Key>::type::value + 1> > >::type type;
    };

    template <class Map, class Key, bool stateless>
    struct InsertOrIncrement<Map, Key, false, stateless>
    {
        typedef typename std::conditional<
            stateless,
            boost::mpl::insert<
                Map, boost::mpl::pair<Key, int_<1> > >,
            boost::mpl::map<> >::type::type type;
    };

    template <int N>
    struct PowerSetIndices;

    template <template <class...> class Container, class ... Funcs>
    struct Factors<Container<Funcs...>, IsReorderableFunctionContainer<Container<Funcs...> > >
    {
        template <class>
        struct SubFactors;

        template <class ... Fs>
        struct SubFactors<std::tuple<Fs...> >
        {
            typedef boost::mpl::vector<typename Factors<Fs>::type...> type;
        };

        struct Merger2
        {
            template <class Map, class Next>
            struct apply
            {
                typedef typename InsertOrIncrement<Map, typename Next::first>::type type;
            };
        };

        struct Merger1
        {
            template <class OverallMap, class CurrentMap>
            struct apply
            {
                typedef typename boost::mpl::fold<
                    CurrentMap, OverallMap, Merger2
                    >::type type;
            };
        };

        typedef typename boost::mpl::fold<
            typename SubFactors<
                decltype(std::declval<Container<Funcs...> >().GetFunctions())>::type,
                boost::mpl::map<>,
            Merger1>::type Individuals;

        typedef Individuals type;
    };

    template <class ... Fs>
    struct Factors<Composition<Fs...> >
    {
        template <class>
        struct SubFactors;

        template <class F, class ... F2s>
        struct SubFactors<Composition<F, F2s...> >
        {
            typedef typename SubFactors<Composition<F2s...> >::type SubType;
            typedef InsertOrIncrement<SubType, Composition<F, F2s...> > InsertExpr;
            typedef typename std::conditional<
                is_stateless<Composition<F, F2s...> >::value,
                InsertExpr, SubType>::type::type type;
        };

        template <class F>
        struct SubFactors<Composition<F> >
        {
            typedef typename std::conditional<
                is_variable<F>::value,
                boost::mpl::map<>,
                boost::mpl::map<boost::mpl::pair<F, int_<1> > > >::type type;
        };

        typedef typename SubFactors<Composition<Fs...> >::type type;
    };

        template <class F>
        struct Factors<F, Variable_c<F> >
        {
            typedef boost::mpl::map<> type; 
        };

template <class F>
auto Factor(F f)
{
}

}

#endif
