#ifndef MANIFOLDS_TESTS_POINTWISE_COMPARISON_HH
#define MANIFOLDS_TESTS_POINTWISE_COMPARISON_HH

#include <boost/test/unit_test.hpp>
#include <random>
#include "functions/matrix.hh"

namespace manifolds
{
    template <class Eng, class Rand, int ... Ns>
    auto MakeInput(Rand & r, Eng & e,
                   std::integer_sequence<int,Ns...>)
    {
        return make_my_tuple(((void)Ns, r(e))...);
    }

    template <int N, class Eng, class Rand>
    auto MakeInput(Rand & r, Eng & e)
    {
        return MakeInput(r, e, std::make_integer_sequence<
                             int, N>());
    }

    template <class M>
    void ComparePoint(M m1, M m2, double tolerance, std::true_type)
    {
        for(std::size_t r = 0; r < M::num_rows; r++)
            for(std::size_t c = 0; c < M::num_cols; c++)
                BOOST_CHECK_CLOSE(m1.Coeff(r,c),
                                  m2.Coeff(r,c),
                                  tolerance);
    }

    template <class X>
    void ComparePoint(X x1, X x2, double tolerance, std::false_type)
    {
        BOOST_CHECK_CLOSE(x1, x2, tolerance);
    }

    template <class F>
    void ComparePoint(F f1, F f2, double tolerance)
    {
        ComparePoint(f1, f2, tolerance, IsMatrix<F>());
    }

    inline void PrintTupleIter(const tuple<> & ){}

    template <class Tuple>
    void PrintTupleIter(const Tuple & t)
    {
        std::cout << get<0>(t);
        if(tuple_size<Tuple>::value > 1)
            std::cout << ", ";
        PrintTupleIter(remove_element<0>(t));
    }

    template <class Tuple>
    void PrintTuple(const Tuple & t)
    {
        std::cout << "(";
        PrintTupleIter(t);
        std::cout << ")\n";
    }

    static const int pointwise_default_num_points = 100;
    static const double pointwise_default_tolerance = 0;
    static const double pointwise_default_min = -100;
    static const double pointwise_default_max = 100;

    template <class F, class FRef>
    void PointwiseEqual(F f, FRef fr,
                        int num_points = pointwise_default_num_points,
                        double tolerance = pointwise_default_tolerance,
                        double min = pointwise_default_min,
                        double max = pointwise_default_max)
    {
        std::minstd_rand e;
        std::uniform_real_distribution<> r(min,max);
        for(int i = 0; i < num_points; i++)
        {
            auto x = MakeInput<F::input_dim>(r, e);
            //PrintTuple(x);
            ComparePoint(f(x), fr(x), tolerance);
        }
    }
}

#endif
