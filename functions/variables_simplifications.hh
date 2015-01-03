#ifndef MANIFOLDS_FUNCTIONS_VARIABLES_SIMPLIFICATIONS_HH
#define MANIFOLDS_FUNCTIONS_VARIABLES_SIMPLIFICATIONS_HH

namespace manifolds {
template <bool a, class F>
struct Simplification<Composition<Variable<0, a>, F>,
                      /*com_v_0_f*/ 2,
                      typename std::enable_if<F::output_dim == 1>::type> {
  typedef F type;

  static type Combine(Composition<Variable<0, a>, F> c) {
    SIMPLIFY_INFO("Simplifying composition of x and single "
                  "dimensional function\n");
    return get<1>(c.GetFunctions());
  }
};
}
#endif
