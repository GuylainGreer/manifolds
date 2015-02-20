#ifndef MANIFOLDS_FUNCTIONS_CONCEPTS_HH
#define MANIFOLDS_FUNCTIONS_CONCEPTS_HH

#include "variables.hh"
#include "composition.hh"
#include "addition.hh"
#include "multiplication.hh"

namespace manifolds {
    template <class T>
    struct is_variable : std::false_type{};

    template <int N, bool a, bool c>
    struct is_variable<Variable<N, a, c> >:
        std::true_type{};

    template <class, class>
    struct is_same_v : std::false_type{};

    template <template <class...> class T,
              class ... T1s, class ... T2s>
    struct is_same_v<T<T1s...>, T<T2s...> > : std::true_type{};

    template <class F, class = void>
    struct is_reorderable : std::false_type{};

    template <class F>
    struct is_reorderable<F, typename std::enable_if<is_same_v<F, Multiplication<> >::value>::type> :
        bool_<F::abelian_arithmetic>{};

    template <class F>
    struct is_reorderable<F, typename std::enable_if<is_same_v<F, Addition<> >::value>::type> :
        std::true_type{};

    template <class F>
    struct is_composition : std::false_type{};

    template <class ... Fs>
    struct is_composition<Composition<Fs...> > : std::true_type{};

    template <class...>
    using void_t = void;

    template <class F>
    using Variable_c = typename std::enable_if<is_variable<F>::value>::type;

    template <class F>
    using Composition_c = typename std::enable_if<is_composition<F>::value>::type;

    template <class F>
    using IsFunctionContainer = void_t<decltype(std::declval<F>().GetFunctions())>;

    template <class F>
    using IsReorderableFunctionContainer = void_t<IsFunctionContainer<F>,
                                                  typename std::enable_if<is_reorderable<F>::value>::type>;

}
#endif
