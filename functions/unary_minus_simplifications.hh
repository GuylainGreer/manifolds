#ifndef MANIFOLDS_FUNCTIONS_UNARY_MINUS_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_UNARY_MINUS_SIMPLIFICATIONS_HH

#include "unary_minus.hh"
#include "zero.hh"

namespace manifolds {
template <class F>
struct Simplification<UnaryMinus<UnaryMinus<F> >,
                      /*um_um_f*/ 0> {
  typedef F type;
  static type Combine(UnaryMinus<UnaryMinus<F> > u) {
    SIMPLIFY_INFO("Simplifying unary minus of unary "
                  "minus of function\n");
    return u.GetFunction().GetFunction();
  }
};

template <class F> struct Simplification<UnaryMinus<F>, /*um_f*/ 1> {
  static auto Combine(UnaryMinus<F> u) {
    SIMPLIFY_INFO("Simplifying unary minus of function\n");
    auto f = Simplify(u.GetFunction());
    return UnaryMinus<decltype(f)>(f);
  }

  typedef decltype(Combine(std::declval<UnaryMinus<F> >())) type;
};

template <> struct Simplification<UnaryMinus<Zero>, /*um_z*/ 0> {
  static auto Combine(UnaryMinus<Zero>) {
    SIMPLIFY_INFO("Simplifying unary minus of zero\n");
    return zero;
  }
};

template <class F, class... Fs>
struct Simplification<Composition<UnaryMinus<F>, Fs...>, 0> {
  template <class T, std::size_t... indices>
  static auto Process(T t, std::integer_sequence<std::size_t, indices...>) {
    auto tu = t.GetFunctions();
    return Negative(ComposeRaw(std::get<0>(tu).GetFunction(),
                               std::get<indices + 1>(tu)...));
  }
  static auto Combine(Composition<UnaryMinus<F>, Fs...> c) {
    SIMPLIFY_INFO("Simplifying Composition of UnaryMinus and Functions\n");
    return Process(c, std::index_sequence_for<Fs...>());
  };
};
}

#endif
