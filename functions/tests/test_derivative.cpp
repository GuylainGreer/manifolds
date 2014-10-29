#include <boost/test/unit_test.hpp>
#include "functions/derivatives.hh"
#include "functions/streaming.hh"
#include "pointwise_equal.hh"

BOOST_AUTO_TEST_CASE(derivative_test)
{
    return;
    using namespace manifolds;
    BOOST_CHECK_EQUAL(Derivative(Sin()), Cos());
    auto f = Cos()(x*x) * y;
    auto d = Derivative(f);
    auto check = 2_c * -x * Sin()(x * x) * y;
    PointwiseEqual(d, check);

    D<2> d2;
    auto ddf = d2(f);
    auto ddf2 = Derivative(d);
    PointwiseEqual(ddf, ddf2);
    auto fd = FullDerivative(f);
    auto fd_check =
        GetFunctionMatrix(GetRow(d, Cos()(x*x)));
    PointwiseEqual(fd, fd_check);
    auto fd_check_d = Derivative(fd);
    auto fd_check_check =
        GetFunctionMatrix(GetRow(-2_c * Sin()(x*x)*y +
                                 -4_c * x * x * Cos()(x*x)*y,
                                 -2_c * x * Sin()(x * x)));
    PointwiseEqual(fd_check, fd_check_check);
    PointwiseEqual(fd_check_d, fd_check_check);
    auto p = Pow()(x, x);
    auto pd = Derivative(p);
    auto pd_check = (Log()(x)+1_c)*Pow()(x,x);
    PointwiseEqual(pd, pd_check, pointwise_default_num_points,
                   pointwise_default_tolerance, 0.001);

    auto m = D<3>()(Log()(x));
    auto m_check =
        Derivative(Derivative(Derivative(Log()(x))));
    BOOST_CHECK_EQUAL(m, m_check);
}
