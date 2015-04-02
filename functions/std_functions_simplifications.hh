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
struct Simplification<Composition<IntegralPolynomial<0, 0, 1>, Cos>,
                      /*mult_cos_cos*/ 0> {
  static auto Combine(Composition<IntegralPolynomial<0, 0, 1>, Cos>) {
    SIMPLIFY_INFO("Simplifying cos**2\n");
    return ComposeRaw(IP<1, 0, -1>(), sin_);
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
  static auto Combine(Composition<F1, F2>) { return IP<0, 1>(); }
};

template <> struct Simplification<Composition<Sin, ACos>, 0> {
  static auto Combine(Composition<Sin, ACos>) {
    SIMPLIFY_INFO("Simplifying Sin(ACos)\n");
    return sqrt_(IP<1, 0, -1>());
  }
};

template <> struct Simplification<Composition<Sin, ATan>, 0> {
  static auto Combine(Composition<Sin, ATan>) {
    SIMPLIFY_INFO("Simplifying Sin(ATan)\n");
    auto hypo = sqrt_(IP<1, 0, 1>());
    return DivideRaw(IP<0, 1>(), hypo);
  }
};

template <> struct Simplification<Composition<Cos, ASin>, 0> {
  static auto Combine(Composition<Cos, ASin>) {
    SIMPLIFY_INFO("Simplifying Cos(ASin)\n");
    return sqrt_(IP<1, 0, -1>());
  }
};

template <> struct Simplification<Composition<Cos, ATan>, 0> {
  static auto Combine(Composition<Cos, ATan>) {
    SIMPLIFY_INFO("Simplifying Cos(ATan)\n");
    auto hypo = sqrt_(IP<1, 0, 1>());
    return DivideRaw(IP<1>(), hypo);
  }
};

template <> struct Simplification<Composition<Tan, ASin>, 0> {
  static auto Combine(Composition<Tan, ASin>) {
    SIMPLIFY_INFO("Simplifying Tan(ASin)\n");
    return DivideRaw(IP<0, 1>(), IP<1, 0, -1>());
  }
};

template <> struct Simplification<Composition<Tan, ACos>, 0> {
  static auto Combine(Composition<Tan, ACos>) {
    SIMPLIFY_INFO("Simplifying Tan(ACos)\n");
    return DivideRaw(IP<1>(), IP<1, 0, -1>());
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
    SIMPLIFY_INFO("Simplifying Sin(-n*x^m)\n");
    return Negative(sin_(Negative(IntegralPolynomial<coeffs...>())));
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
    SIMPLIFY_INFO("Simplifying Sin(n*x^m)\n");
    return std::make_pair(IP<(indices * 0)..., left>(),
                          IP<(indices * 0)..., right>());
  }

  static auto Combine(Composition<Sin, IntegralPolynomial<coeffs...> >) {
    SIMPLIFY_INFO("Simplifying Sin(n*x^m)\n");
    auto subs = Combine(std::make_index_sequence<sizeof...(coeffs) - 1>());
    auto left = subs.first;
    auto right = subs.second;
    return AddRaw(MultiplyRaw(sin_(left), cos_(right)),
                  MultiplyRaw(cos_(left), sin_(right)));
  }
};

template <IPInt_t... coeffs>
struct Simplification<
    Composition<Cos, IntegralPolynomial<coeffs...> >, 0,
    typename std::enable_if<is_all_but_last_0<coeffs...>::value &&(
        last<ic<coeffs>...>::type::value < 0)>::type> {

  static auto Combine(Composition<Cos, IntegralPolynomial<coeffs...> >) {
    SIMPLIFY_INFO("Simplifying Cos(-n*x^m)\n");
    return cos_(Negative(IntegralPolynomial<coeffs...>()));
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
    return std::make_pair(IP<(indices * 0)..., left>(),
                          IP<(indices * 0)..., right>());
  }

  static auto Combine(Composition<Cos, IntegralPolynomial<coeffs...> >) {
    SIMPLIFY_INFO("Simplifying Cos(n*x^m)\n");
    auto subs = Combine(std::make_index_sequence<sizeof...(coeffs) - 1>());
    auto left = subs.first;
    auto right = subs.second;
    return SubRaw(MultiplyRaw(cos_(left), cos_(right)),
                  MultiplyRaw(sin_(left), sin_(right)));
  }
};

template <IPInt_t... coeffs> struct SplitPoly {
  template <int i>
  using ith = typename nth<i, std::integral_constant<IPInt_t, coeffs>...>::type;

  template <int i, int DUMMY = 0> struct NonZero {
    static const int index = sizeof...(coeffs) - 1 - i;
    static const int value =
        ith<index>::value == 0 ? NonZero<i - 1, 0>::value : i;
  };

  template <int DUMMY> struct NonZero<-1, DUMMY> {
    static const int value = -1;
  };

  static const int first = NonZero<sizeof...(coeffs) - 1>::value;
  static_assert(first != -1, "");

  template <class> struct LeftPoly;

  template <std::size_t... indices>
  struct LeftPoly<std::integer_sequence<std::size_t, indices...> > {
    typedef IntegralPolynomial<ith<indices>::value...> type;
  };

  template <class> struct RightPoly;

  template <std::size_t... indices>
  struct RightPoly<std::integer_sequence<std::size_t, indices...> > {
    typedef std::tuple<std::integral_constant<IPInt_t, coeffs>...> tup;
    typedef std::integral_constant<IPInt_t, 0> zero;
    typedef decltype(replace_element<first>(tup(), zero())) tup_z;
    typedef IntegralPolynomial<
        std::tuple_element<indices, tup_z>::type::value...> type;
  };

  typedef boost::mpl::pair<
      typename LeftPoly<std::make_index_sequence<first + 1> >::type,
      typename RightPoly<std::make_index_sequence<sizeof...(coeffs)> >::type>
  type;
};

template <IPInt_t... coeffs>
struct Simplification<
    Composition<Sin, IntegralPolynomial<coeffs...> >, 0,
    typename std::enable_if<!is_all_but_last_0<coeffs...>::value>::type> {
  typedef typename SplitPoly<coeffs...>::type Splits;
  typedef typename Splits::first Left;
  typedef typename Splits::second Right;

  static auto Combine(Composition<Sin, IntegralPolynomial<coeffs...> >) {
    SIMPLIFY_INFO("Simplifying Sin(poly + poly)\n");
    return AddRaw(MultiplyRaw(Sin {}(Left{}), Cos {}(Right{})),
                  MultiplyRaw(Cos {}(Left{}), Sin {}(Right{})));
  }
};

template <IPInt_t... coeffs>
struct Simplification<
    Composition<Cos, IntegralPolynomial<coeffs...> >, 0,
    typename std::enable_if<!is_all_but_last_0<coeffs...>::value>::type> {
  typedef typename SplitPoly<coeffs...>::type Splits;
  typedef typename Splits::first Left;
  typedef typename Splits::second Right;

  static auto Combine(Composition<Cos, IntegralPolynomial<coeffs...> >) {
    SIMPLIFY_INFO("Simplifying Cos(poly + poly)\n");
    return SubRaw(MultiplyRaw(Cos {}(Left{}), Cos {}(Right{})),
                  MultiplyRaw(Sin {}(Left{}), Sin {}(Right{})));
  }
};

template <IPInt_t... coeffs>
struct Simplification<
    Composition<Tan, IntegralPolynomial<coeffs...> >, 0,
    typename std::enable_if<is_all_but_last_0<coeffs...>::value &&(
        last<ic<coeffs>...>::type::value < 0)>::type> {
  static auto Combine(Composition<Tan, IntegralPolynomial<coeffs...> >) {
    return Negative(tan_(Negative(IP<coeffs...>())));
  }
};

template <IPInt_t... coeffs>
struct Simplification<
    Composition<Tan, IntegralPolynomial<coeffs...> >, 1,
    typename std::enable_if<is_all_but_last_0<coeffs...>::value &&(
        last<ic<coeffs>...>::type::value > 1)>::type> {
  template <class T, std::size_t... indices>
  static auto Process(T, std::integer_sequence<std::size_t, indices...>) {
    return IP<std::tuple_element<indices, T>::type::value...>();
  }
  static auto Combine(Composition<Tan, IntegralPolynomial<coeffs...> >) {
    static const IPInt_t last_c = last<ic<coeffs>...>::type::value;
    static const int last_i = sizeof...(coeffs) - 1;
    static const IPInt_t left = last_c / 2;
    static const IPInt_t right = last_c - left;
    typedef std::tuple<std::integral_constant<IPInt_t, coeffs>...> tup;
    typedef std::integral_constant<IPInt_t, left> Left;
    typedef std::integral_constant<IPInt_t, right> Right;
    std::make_index_sequence<sizeof...(coeffs)> indices;
    auto left_p = Process(replace_element<last_i>(tup(), Left()), indices);
    auto right_p = Process(replace_element<last_i>(tup(), Right()), indices);
    return DivideRaw(AddRaw(tan_(left_p), tan_(right_p)),
                     IP<1, 1>()(MultiplyRaw(tan_(left_p), tan_(right_p))));
  }
};

template <IPInt_t... coeffs>
struct Simplification<
    Composition<Tan, IntegralPolynomial<coeffs...> >, 0,
    typename std::enable_if<!is_all_but_last_0<coeffs...>::value>::type> {
  static auto Combine(Composition<Tan, IntegralPolynomial<coeffs...> >) {
    typedef typename SplitPoly<coeffs...>::type Pair;
    typename Pair::first left;
    typename Pair::second right;
    return DivideRaw(AddRaw(tan_(left), tan_(right)),
                     IP<1, 1>()(MultiplyRaw(tan_(left), tan_(right))));
  }
};

template <IPInt_t... coeffs>
struct Simplification<Composition<IntegralPolynomial<coeffs...>, Sqrt>, 0,
                      typename std::enable_if<(sizeof...(coeffs) > 2)>::type> {
  template <int i>
  using ith = typename nth<i, std::integral_constant<IPInt_t, coeffs>...>::type;

  template <int, class, class> struct Process;

  template <int i, IPInt_t... evens, IPInt_t... odds>
  struct Process<i, std::integer_sequence<IPInt_t, evens...>,
                 std::integer_sequence<IPInt_t, odds...> > {
    typedef typename std::conditional<
        i % 2 == 0, std::integer_sequence<IPInt_t, ith<i>::value, evens...>,
        std::integer_sequence<IPInt_t, evens...> >::type nes;

    typedef typename std::conditional<
        i % 2 == 1, std::integer_sequence<IPInt_t, ith<i>::value, odds...>,
        std::integer_sequence<IPInt_t, odds...> >::type nos;

    typedef typename Process<i - 1, nes, nos>::type type;
  };

  template <IPInt_t... evens, IPInt_t... odds>
  struct Process<-1, std::integer_sequence<IPInt_t, evens...>,
                 std::integer_sequence<IPInt_t, odds...> > {
    typedef boost::mpl::pair<IntegralPolynomial<evens...>,
                             IntegralPolynomial<odds...> > type;
  };

  static auto Combine(Composition<IntegralPolynomial<coeffs...>, Sqrt>) {
    typedef typename Process<sizeof...(coeffs) - 1,
                             std::integer_sequence<IPInt_t>,
                             std::integer_sequence<IPInt_t> >::type Pair;
    typedef typename Pair::first Evens;
    typedef typename Pair::second Odds;
    return AddRaw(Evens(), MultiplyRaw(Odds(), sqrt_));
  }
};
}

#endif
