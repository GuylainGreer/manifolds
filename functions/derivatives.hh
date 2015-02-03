#ifndef MANIFOLDS_FUNCTIONS_DERIVATIVES_HH
#define MANIFOLDS_FUNCTIONS_DERIVATIVES_HH

#include "addition.hh"
#include "multiplication.hh"
#include "variables.hh"
#include "division.hh"
#include "polynomial.hh"
#include "trig.hh"
#include "function_matrix.hh"
#include "all_simplifications.hh"
#include "operators.hh"

#ifdef PRINT_SIMPLIFIES
#define PRINT_DERIV(object, variable, name, result)                            \
  std::cout << "Taking derivative of " << name << " (" << object               \
            << ") with respect to " << variable << " to get " << result        \
            << '\n'
#else
#define PRINT_DERIV(o, v, n, r)
#endif

namespace manifolds {

template <int iter> struct DerivativeWrapper {
  template <class... Funcs, int i, std::size_t... is>
  static auto GroupDeriv(const Group<Funcs...> &g, Variable<i> v,
                         std::integer_sequence<std::size_t, is...>) {
    return GetGroup(DerivativeWrapper<iter + 1>::Derivative(
        std::get<is>(g.GetFunctions()), v)...);
  }

  template <class... Funcs, int i>
  static auto Derivative(const Group<Funcs...> &g, Variable<i> v) {
    return GroupDeriv(g, v, std::index_sequence_for<Funcs...>());
  }

  template <class... Funcs, int i, std::size_t... indices>
  static auto Derivative(const Addition<Funcs...> &a, Variable<i> v,
                         std::integer_sequence<std::size_t, indices...>) {
    return AddRaw(DerivativeWrapper<iter + 1>::Derivative(
        std::get<indices>(a.GetFunctions()), v)...);
  }

  template <class... Funcs, int i>
  static auto Derivative(const Addition<Funcs...> &a, Variable<i> v) {
    auto r = DerivativeWrapper<iter + 1>::Derivative(
        a, v, std::index_sequence_for<Funcs...>());
    PRINT_DERIV(a, v, "addition", r);
    return r;
  }

  template <class F, int i>
  static auto Derivative(UnaryMinus<F> f, Variable<i> v) {
    auto r = -DerivativeWrapper<iter + 1>::Derivative(f.GetFunction(), v);
    PRINT_DERIV(f, v, "negative of function", r);
    return r;
  }

  template <class Func, int i>
  static auto Derivative(const Composition<Func> &c, Variable<i> v) {
    auto r = DerivativeWrapper<iter + 1>::Derivative(
        std::get<0>(c.GetFunctions()), v);
    PRINT_DERIV(c, v, "last composition", r);
    return r;
  }

  template <class Func, class... Funcs, int i,
            class = typename std::enable_if<(sizeof...(Funcs) > 0)>::type>
  static auto Derivative(const Composition<Func, Funcs...> &c, Variable<i> v) {
    auto funcs = remove_element<0>(c.GetFunctions());
    auto d = FullDeriv(std::get<0>(c.GetFunctions()));
    auto next = tuple_cat(make_my_tuple(d), funcs);
    auto r = Composition<decltype(d), Funcs...>(next) *
             DerivativeWrapper<iter + 1>::Derivative(
                 Composition<Funcs...>(funcs), v);
    PRINT_DERIV(c, v, "composition", r);
    return r;
  }

  template <class F1, class F2, int i>
  static auto Derivative(const Division<F1, F2> &d, Variable<i> v) {
    auto n = d.GetNumerator();
    auto de = d.GetDenominator();
    auto r = (DerivativeWrapper<iter + 1>::Derivative(n, v) * de -
              n * DerivativeWrapper<iter + 1>::Derivative(de, v)) /
             (de * de);
    PRINT_DERIV(d, v, "division", r);
    return r;
  }

  template <int i, class... Funcs, int d_i, std::size_t... lefts,
            std::size_t... rights>
  static auto TimesDerivHelper(const Multiplication<Funcs...> &a,
                               Variable<d_i> v,
                               std::integer_sequence<std::size_t, lefts...>,
                               std::integer_sequence<std::size_t, rights...>) {
    auto t = a.GetFunctions();
    return Multiply(std::get<lefts>(t)...,
                    DerivativeWrapper<iter + 1>::Derivative(std::get<i>(t), v),
                    std::get<rights + i + 1>(t)...);
  }

  template <int i, class... Funcs, int d_i>
  static auto TimesDerivHelper(const Multiplication<Funcs...> &a,
                               Variable<d_i> v) {
    return TimesDerivHelper<i>(
        a, v, std::make_index_sequence<i>(),
        std::make_index_sequence<sizeof...(Funcs) - i - 1>());
  }

  template <class... Funcs, int i, std::size_t... indices>
  static auto Derivative(const Multiplication<Funcs...> &a, Variable<i> v,
                         std::integer_sequence<std::size_t, indices...>) {
    return Add(TimesDerivHelper<indices>(a, v)...);
  }

  template <class... Funcs, int i>
  static auto Derivative(const Multiplication<Funcs...> &a, Variable<i> v) {
    auto r = DerivativeWrapper<iter + 1>::Derivative(
        a, v, std::index_sequence_for<Funcs...>());
    PRINT_DERIV(a, v, "multiplication", r);
    return r;
  }

  template <class CoeffType, class Order, int i>
  static auto Derivative(const Polynomial<CoeffType, Order> &p, Variable<i> v) {
    PRINT_DERIV(p, v, "non-constant polynomial", zero);
    return zero;
  }

  template <class CoeffType, int i>
  static auto Derivative(Polynomial<CoeffType, int_<1> > p, Variable<i> v) {
    PRINT_DERIV(p, v, "constant polynomial", zero);
    return zero;
  }

  template <class CoeffType, class Order,
            class = typename std::enable_if<Order::value != 1>::type>
  static auto Derivative(const Polynomial<CoeffType, Order> &p, Variable<0> v) {
    std::array<CoeffType, Order::value - 1> d;
    for (unsigned i = 0; i < Order::value - 1; i++)
      d[i] = (i + 1) * p.GetCoeffs()[i + 1];
    auto r = Polynomial<CoeffType, int_<Order::value - 1> >{ d };
    PRINT_DERIV(p, v, "non-constant polynomial", r);
    return r;
  }

  template <class, class, int> struct IPDeriv;

  template <IPInt_t... output, int t>
  struct IPDeriv<std::integer_sequence<IPInt_t>,
                 std::integer_sequence<IPInt_t, output...>, t> {
    typedef IntegralPolynomial<output...> type;
  };

  template <IPInt_t next, IPInt_t... rest, IPInt_t... output, int term_order>
  struct IPDeriv<std::integer_sequence<IPInt_t, next, rest...>,
                 std::integer_sequence<IPInt_t, output...>, term_order> {
    typedef typename IPDeriv<
        std::integer_sequence<IPInt_t, rest...>,
        std::integer_sequence<IPInt_t, output..., next *term_order>,
        term_order + 1>::type type;
  };

  template <IPInt_t f, IPInt_t... coeffs,
            class = typename std::enable_if<(sizeof...(coeffs) > 0)>::type>
  static auto Derivative(IntegralPolynomial<f, coeffs...> ip, Variable<0> v) {
    typename IPDeriv<std::integer_sequence<IPInt_t, coeffs...>,
                     std::integer_sequence<IPInt_t>, 1>::type r;
    PRINT_DERIV(ip, v, "non-constant integral polynomial", r);
    return r;
  }

  template <IPInt_t f>
  static auto Derivative(IntegralPolynomial<f> ip, Variable<0> v) {
    PRINT_DERIV(ip, v, "constant integral polynomial", zero);
    return zero;
  }

  template <IPInt_t... ts, int i>
  static auto Derivative(IntegralPolynomial<ts...> ip, Variable<i> v) {
    PRINT_DERIV(ip, v, "non-constant integral polynomial", zero);
    return zero;
  }

#define TRIG_DERIV(func, deriv)                                                \
  static auto Derivative(func f, Variable<0> v) {                              \
    PRINT_DERIV(f, v, #func, deriv);                                           \
    return deriv;                                                              \
  }                                                                            \
  template <int i> static auto Derivative(func f, Variable<i> v) {             \
    PRINT_DERIV(f, v, #func, zero);                                            \
    return zero;                                                               \
  }

  TRIG_DERIV(Sin, Cos())
  TRIG_DERIV(Cos, -Sin())
  TRIG_DERIV(Tan, 1_c / (Cos() * Cos()))
  TRIG_DERIV(Log, 1_c / x)
  TRIG_DERIV(Sinh, Cosh())
  TRIG_DERIV(Cosh, Sinh())
  TRIG_DERIV(Tanh, 1_c / (Cosh() * Cosh()))
  TRIG_DERIV(ASin, 1_c / Sqrt()(1_c - x *x))
  TRIG_DERIV(ACos, -1_c / Sqrt()(1_c - x *x))
  TRIG_DERIV(ATan, 1_c / (1_c + x *x))
  TRIG_DERIV(ASinh, 1_c / Sqrt()(1_c + x *x))
  TRIG_DERIV(ACosh, 1_c / Sqrt()(-1_c + x *x))
  TRIG_DERIV(ATanh, 1_c / (1_c - x *x))
  TRIG_DERIV(Exp, Exp())
  TRIG_DERIV(Sqrt, 0.5_c / Sqrt())
  TRIG_DERIV(Zero, zero)

#undef TRIG_DERIV

  template <int i> static auto Derivative(Pow p, Variable<i> v) {
    PRINT_DERIV(p, v, "Pow", zero);
    return zero;
  }

  static auto Derivative(Pow p, Variable<0> v) {
    auto r = y * Pow()(x, y - 1_c);
    PRINT_DERIV(p, v, "Pow", r);
    return r;
  }

  static auto Derivative(Pow p, Variable<1> v) {
    auto r = (Log()(x)) * Pow()(x, y);
    PRINT_DERIV(p, v, "Pow", r);
    return r;
  }

  template <int i> static auto Derivative(Variable<i> v1, Variable<i> v2) {
    IntegralPolynomial<1> r;
    PRINT_DERIV(v1, v2, "variable", r);
    return r;
  }

  template <int i, int j>
  static auto Derivative(Variable<i> v1, Variable<j> v2) {
    PRINT_DERIV(v1, v2, "variable", zero);
    return zero;
  }

  template <class... Functions, int i, std::size_t... indices>
  static auto Derivative(FunctionMatrix<Functions...> m, Variable<i> v,
                         std::integer_sequence<std::size_t, indices...>) {
    typedef FunctionMatrix<Functions...> M;
    return GetFunctionMatrix<M::num_rows, M::num_cols>(
        make_my_tuple(Derivative(std::get<indices>(m.GetFunctions()), v)...));
  }

  template <class... Functions, int i>
  static auto Derivative(FunctionMatrix<Functions...> m, Variable<i> v) {
    static const int size =
        std::tuple_size<decltype(m.GetFunctions())>::type::value;
    auto r = Derivative(m, v, std::make_index_sequence<size>());
    PRINT_DERIV(m, v, "function matrix", r);
    return r;
  }

  template <class T, std::size_t... ins>
  static auto FullDerivHelper(T t, int_<T::output_dim>,
                              std::integer_sequence<std::size_t, ins...>) {
    return tuple<>();
  }

  template <class T> static auto GetOutputs(T t, int_<1>) {
    return make_my_tuple(t);
  }

  template <class T, int i> static auto GetOutputs(T t, int_<i>) {
    return t.GetOutputs();
  }

  template <class T> static auto GetOutputs(T t) {
    return GetOutputs(t, int_<T::output_dim>());
  }

  template <int col, class T, std::size_t... ins,
            class = typename std::enable_if<col != T::output_dim>::type>
  static auto FullDerivHelper(T t, int_<col>,
                              std::integer_sequence<std::size_t, ins...> b) {
    auto r = make_my_tuple(
        Derivative(std::get<col>(GetOutputs(t)), Variable<ins>())...);
    return tuple_cat(
        r, DerivativeWrapper<iter + 1>::FullDerivHelper(t, int_<col + 1>(), b));
  }

  template <class T> static auto FullDerivOutput(tuple<T> t, int_<1>, int_<1>) {
    return std::get<0>(t);
  }

  template <class... Ts, int rows, int cols>
  static auto FullDerivOutput(tuple<Ts...> t, int_<rows>, int_<cols>) {
    return GetFunctionMatrix<rows, cols>(t);
  }

  template <class T> static auto FullDeriv(T t) {
#ifdef PRINT_SIMPLIFIES
    std::cout << "Taking full derivative of " << t << '\n';
#endif
    return FullDerivOutput(
        DerivativeWrapper<iter + 1>::FullDerivHelper(
            t, int_<0>(), std::make_index_sequence<T::input_dim>()),
        int_<T::output_dim>(), int_<T::input_dim>());
  }
};

template <class T, int i> auto Derivative(T t, Variable<i> v) {
  return DerivativeWrapper<0>::Derivative(t, v);
}

template <class T> auto Derivative(T t) {
  return DerivativeWrapper<0>::Derivative(t, x);
}

template <class T> auto FullDerivative(T t) {
  return DerivativeWrapper<0>::FullDeriv(t);
}

template <int order, int i = 0, bool ABELIAN = true> struct D {
  static const bool stateless = true;
  static const bool abelian = ABELIAN;

  template <class F> auto operator()(F f) const {
    return D<order - 1, i>()(Derivative(f, Variable<i>()));
  }
};

template <int i> struct D<0, i> {
  template <class F> auto operator()(F f) const { return f; }
};
}

#endif
