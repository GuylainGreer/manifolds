#ifndef MANIFOLDS_FUNCTIONS_TRIG_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_TRIG_SIMPLIFICATIONS_HH

#include "multiplication.hh"
#include "integral_polynomial.hh"
#include <boost/mpl/map.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/contains.hpp>

namespace manifolds {

template <>
struct Simplification<Multiplication<Cos, Cos>,
                      /*mult_cos_cos*/ 0> {
  typedef Multiplication<Cos, Cos> in_type;
  static auto Combine(in_type) {
    SIMPLIFY_INFO("Simplifying cos**2\n");
    return ComposeRaw(GetIPolynomial<1, 0, -1>(), Sin());
  }
};

typedef boost::mpl::map<
    boost::mpl::pair<Cos, ACos>, boost::mpl::pair<Sin, ASin>,
    boost::mpl::pair<Tan, ATan>, boost::mpl::pair<Cosh, ACosh>,
    boost::mpl::pair<Sinh, ASinh>, boost::mpl::pair<Tanh, ATanh> > RawInverses;

struct AddReverse {
  template <class Map, class Elem> struct apply {
    typedef typename boost::mpl::insert<
        Map, boost::mpl::pair<typename Elem::second,
                              typename Elem::first> >::type type;
  };
};

typedef boost::mpl::fold<RawInverses, RawInverses, AddReverse>::type Inverses;

static_assert(boost::mpl::size<Inverses>::type::value == 12, "");
static_assert(
    boost::is_same<boost::mpl::at<Inverses, ATanh>::type, Tanh>::value, "");
static_assert(
    boost::mpl::contains<Inverses, boost::mpl::pair<Sin, ASin> >::value, "");

template <class F1, class F2>
struct Simplification<
    Composition<F1, F2>, /*com_f_f_inv*/ 0,
    typename std::enable_if<boost::mpl::contains<
        Inverses, boost::mpl::pair<F1, F2> >::type::value>::type> {
  static auto Combine(Composition<F1, F2>) { return GetIPolynomial<0, 1>(); }
};
}

#endif
