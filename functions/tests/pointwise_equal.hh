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

    template <class F, class FRef>
    void PointwiseEqual(F f, FRef fr, int num_points = 100,
                        double tolerance = 0,
                        double min = -100,
                        double max = 100)
    {
        std::minstd_rand e;
        std::uniform_real_distribution<> r;
        for(int i = 0; i < num_points; i++)
        {
            auto x = MakeInput<F::input_dim>(r, e);
            BOOST_CHECK_CLOSE(f(x), fr(x), tolerance);
        }
    }
}

#endif
