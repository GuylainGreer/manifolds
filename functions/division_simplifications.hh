#ifndef MANIFOLDS_FUNCTIONS_RECIPROCAL_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_RECIPROCAL_SIMPLIFICATIONS_HH

#include "simplify.hh"
#include "division.hh"
#include "multiplication.hh"

namespace manifolds {

    template <class N1, class N2, class D1, class D2>
    struct Simplification<
        Multiplication<Division<N1,D1>,
                       Division<N2,D2>>, /*mult_div_div*/0>
    {
        typedef Division<
            Multiplication<N1,N2>,
            Multiplication<D1,D2>> type;

        static type Combine(Multiplication<Division<N1,D1>,
                            Division<N2,D2>> a)
        {
            SIMPLIFY_INFO("Simplifying multiplication of division "
                          "by division\n");
            Multiplication<N1,N2>
                mn(get<0>(a.GetFunctions()).GetNumerator(),
                   get<1>(a.GetFunctions()).GetNumerator());

            Multiplication<D1,D2>
                md(get<0>(a.GetFunctions()).GetDenominator(),
                   get<1>(a.GetFunctions()).GetDenominator());

            return {mn, md};
        }
    };

    template <class N, class D, class F>
    struct Simplification<
        Multiplication<Division<N,D>,F>, /*mult_div_f*/1>
    {
        typedef Division<
            Multiplication<N,F>, D> type;

        static type Combine(Multiplication<Division<N,D>,F> a)
        {
            SIMPLIFY_INFO("Simplifying multiplication of division "
                          "by function\n");
            Multiplication<N,F>
                m(get<0>(a.GetFunctions()).GetNumerator(),
                  get<1>(a.GetFunctions()));
            return {m, get<0>(a.GetFunctions()).GetDenominator()};
        }
    };

    template <class N1, class D1, class N2, class D2>
    struct Simplification<
        Division<Division<N1,D1>,Division<N2,D2> >,
                 /*div_div_div*/0>
    {
        static auto Combine(Division<
                                Division<N1,D1>,
                                Division<N2,D2> > d)
        {
            auto n = d.GetNumerator();
            auto de = d.GetDenominator();
            return DivideRaw(MultiplyRaw(n.GetNumerator(),
                                         de.GetDenominator()),
                             MultiplyRaw(n.GetDenominator(),
                                         de.GetNumerator()));
        }
    };

    template <class N, class D, class F>
    struct Simplification<Division<Division<N,D>,F>,
                          /*div_div_f*/1>
    {
        typedef Division<N, Multiplication<D,F>> type;

        static type Combine(Division<Division<N,D>,F> a)
        {
            SIMPLIFY_INFO("Simplifying division of "
                          "division by function\n");
            return {a.GetNumerator().GetNumerator(),
                    Multiplication<D,F>(a.GetNumerator().GetDenominator(),
                                        a.GetDenominator())};
        }
    };

template <class N, class D, class F>
struct Simplification<Division<F, Division<N,D>>,
                      /*div_f_div*/2>
{
    typedef Division<Multiplication<D,F>, N> type;

    static type Combine(Division<Division<N,D>,F> a)
    {
        SIMPLIFY_INFO("Simplifying division of "
                      "division by function\n");
        return {Multiplication<D,F>(a.GetNumerator().
                                    GetDenominator(),
                                    a.GetDenominator()),
                a.GetNumerator().GetNumerator()};
    }
};

template <class N, class D, class ... Fs>
struct Simplification<
    Composition<Division<N,D>, Fs...>, /*com_div_fs*/0>
{
    typedef Division<
        Composition<N, Fs...>,
        Composition<D, Fs...>> type;

    static type Combine(Composition<Division<N,D>, Fs...> c)
    {
        auto t = c.GetFunctions();
        auto ds = get<0>(t).GetFunctions();
        auto fs = remove_element<0>(t);
        auto n = Composition<N, Fs...>
            (tuple_cat(remove_element<1>(ds), fs));
        auto d = Composition<D, Fs...>
            (tuple_cat(remove_element<0>(ds), fs));
        return {n, d};
    }
};
}

#endif
