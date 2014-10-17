#ifndef MANIFOLDS_TESTS_POINTWISE_COMPARISON_HH
#define MANIFOLDS_TESTS_POINTWISE_COMPARISON_HH

#include <random>

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
            BOOST_CHECK_CLOSE(f(x), fr(x), tolerance);
        }
    }
}

#endif
