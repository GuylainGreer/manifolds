#ifndef MANIFOLDS_FUNCTIONS_COMPOSITION_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_COMPOSITION_SIMPLIFICATIONS_HH

#include "composition.hh"
#include "simplify_variadic.hh"

namespace manifolds {
template <class C> struct Delay<C, true> {
  C c;
  Delay(const C &c) : c(c) {}
  auto Simplify() const { return manifolds::Simplify(c); }
};

template <class... Fs> auto Compose(Fs... fs) {
  return Simplify(ComposeRaw(fs...));
}

template <class... Functions>
struct Simplification<Composition<Functions...>, /*var_com*/ 4> {
  typedef decltype(SimplifyV<Composition>(std::declval<tuple<Functions...> >(),
                                          std::false_type{})) type;

  static type Combine(Composition<Functions...> a) {
    SIMPLIFY_INFO("Simplifying composition\n");
    return SimplifyV<Composition>(a.GetFunctions(), std::false_type{});
  }
};
}

#endif
