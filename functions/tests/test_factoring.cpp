#include <boost/test/unit_test.hpp>
#include "functions/addition.hh"
#include "functions/polynomial.hh"
#include "functions/trig.hh"
#include "functions/factor.hh"
#include <boost/mpl/size.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/for_each.hpp>
#include "functions/streaming.hh"
#include "functions/operators.hh"

BOOST_AUTO_TEST_CASE(factoring_test)
{
    using namespace manifolds;
    static_assert(HighestVariable<Variable<3> >::value == 3, "");
    static_assert(HighestVariable<std::tuple<Variable<0>, Variable<2> > >::value == 2, "");
    static_assert(HighestVariable<Addition<Variable<0>, Sin, Variable<1> > >::value == 1, "");
    static_assert(HighestVariable<Addition<Variable<1>, Addition<Variable<2>, UnaryMinus<Variable<5> > >,
                                           Addition<Sin, Variable<3> > > >::value == 5, "");
    //Check that bug in Boost's m_item class
    using namespace boost::mpl;
static_assert(size<insert<map<>, pair<int,int> >::type::type>::value == 1, "");

static_assert(equal<
              Factors<Sin>::type,
              map<pair<Sin, manifolds::int_<1> > > >::type::value, "");

//static_assert(size<
//              Factors<decltype(sin_(x) + cos_(x))>::type>::type::value == 3, "");

//static_assert(size<
//              Factors<decltype(tan_(x) + GetPolynomial(1.0)(x))>::type>::type::value == 1, "");

 for_each<Factors<decltype(x * sin_(x) + cos_(x) * sin_(x) + tan_(x))>::type>([](auto x){
         std::cout << "(" << typename decltype(x)::first{} << ", " << decltype(x)::second::value << "), ";});
 std::cout << '\n';
 for_each<Factors<Composition<Tan, Sin, Variable<0> > >::type>([](auto x){
         std::cout << "(" << typename decltype(x)::first{} << ", " << decltype(x)::second::value << "), ";});
 std::cout << '\n';
 static_assert(is_reorderable<Addition<Sin,Cos> >::value, "");
}
