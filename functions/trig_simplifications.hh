#ifndef MANIFOLDS_FUNCTIONS_TRIG_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_TRIG_SIMPLIFICATIONS_HH

#include "multiplication.hh"
#include "integral_polynomial.hh"
#include "addition.hh"
#include "division.hh"
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

template <> struct Simplification<Composition<Sin, ACos>, 0> {
  static auto Combine(Composition<Sin, ACos>) {
    return Sqrt()(GetIPolynomial<1, 0, -1>());
  }
};

template <> struct Simplification<Composition<Sin, ATan>, 0> {
  static auto Combine(Composition<Sin, ATan>) {
    auto hypo = Sqrt()(GetIPolynomial<1, 0, 1>());
    return DivideRaw(GetIPolynomial<0, 1>(), hypo);
  }
};

template <> struct Simplification<Composition<Cos, ASin>, 0> {
  static auto Combine(Composition<Cos, ASin>) {
    return Sqrt()(GetIPolynomial<1, 0, -1>());
  }
};

template <> struct Simplification<Composition<Cos, ATan>, 0> {
  static auto Combine(Composition<Cos, ATan>) {
    auto hypo = Sqrt()(GetIPolynomial<1, 0, 1>());
    return DivideRaw(GetIPolynomial<1>(), hypo);
  }
};

template <> struct Simplification<Composition<Tan, ASin>, 0> {
  static auto Combine(Composition<Tan, ASin>) {
    return DivideRaw(GetIPolynomial<0, 1>(), GetIPolynomial<1, 0, -1>());
  }
};

template <> struct Simplification<Composition<Tan, ACos>, 0> {
  static auto Combine(Composition<Tan, ACos>) {
    return DivideRaw(GetIPolynomial<1>(), GetIPolynomial<1, 0, -1>());
  }
};

template <IPInt_t i, IPInt_t... cs> struct is_all_but_last_0 {
  static const bool value = i == 0 && is_all_but_last_0<cs...>::value;
  typedef bool_<value> type;
};

template <IPInt_t i> struct is_all_but_last_0<i> {
  static const bool value = true;
  typedef bool_<value> type;
};

template <IPInt_t i> using ic = std::integral_constant<IPInt_t, i>;

template <IPInt_t... coeffs>
struct Simplification<
    Composition<Sin, IntegralPolynomial<coeffs...> >, 0,
    typename std::enable_if<is_all_but_last_0<coeffs...>::value &&(
        last<ic<coeffs>...>::type::value < 0)>::type> {

  static auto Combine(Composition<Sin, IntegralPolynomial<coeffs...> >) {
    return Negative(Sin()(Negative(IntegralPolynomial<coeffs...>())));
  }
};

template <IPInt_t... coeffs>
struct Simplification<
    Composition<Sin, IntegralPolynomial<coeffs...> >, 0,
    typename std::enable_if<
        is_all_but_last_0<coeffs...>::value &&(sizeof...(coeffs) > 1) &&
        (last<ic<coeffs>...>::type::value > 1)>::type> {
  static const IPInt_t c = last<ic<coeffs>...>::type::value;
  static const IPInt_t left = c / 2;
  static const IPInt_t right = c - left;

  template <std::size_t... indices>
  static auto Combine(std::integer_sequence<std::size_t, indices...>) {
    return std::make_pair(GetIPolynomial<(indices, 0)..., left>(),
                          GetIPolynomial<(indices, 0)..., right>());
  }

  static auto Combine(Composition<Sin, IntegralPolynomial<coeffs...> >) {
    auto subs = Combine(std::make_index_sequence<sizeof...(coeffs) - 1>());
    auto left = subs.first;
    auto right = subs.second;
    return AddRaw(MultiplyRaw(Sin()(left), Cos()(right)),
                  MultiplyRaw(Cos()(left), Sin()(right)));
  }
};

template <IPInt_t... coeffs>
struct Simplification<
    Composition<Cos, IntegralPolynomial<coeffs...> >, 0,
    typename std::enable_if<is_all_but_last_0<coeffs...>::value &&(
        last<ic<coeffs>...>::type::value < 0)>::type> {

  static auto Combine(Composition<Cos, IntegralPolynomial<coeffs...> >) {
    return Cos()(Negative(IntegralPolynomial<coeffs...>()));
  }
};

template <IPInt_t... coeffs>
struct Simplification<
    Composition<Cos, IntegralPolynomial<coeffs...> >, 0,
    typename std::enable_if<
        is_all_but_last_0<coeffs...>::value &&(sizeof...(coeffs) > 1) &&
        (last<ic<coeffs>...>::type::value > 1)>::type> {
  static const IPInt_t c = last<ic<coeffs>...>::type::value;
  static const IPInt_t left = c / 2;
  static const IPInt_t right = c - left;

  template <std::size_t... indices>
  static auto Combine(std::integer_sequence<std::size_t, indices...>) {
    return std::make_pair(GetIPolynomial<(indices, 0)..., left>(),
                          GetIPolynomial<(indices, 0)..., right>());
  }

  static auto Combine(Composition<Cos, IntegralPolynomial<coeffs...> >) {
    auto subs = Combine(std::make_index_sequence<sizeof...(coeffs) - 1>());
    auto left = subs.first;
    auto right = subs.second;
    return SubRaw(MultiplyRaw(Cos()(left), Cos()(right)),
                  MultiplyRaw(Sin()(left), Sin()(right)));
  }
};
}

#endif
