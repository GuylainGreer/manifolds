#ifndef MANIFOLDS_FUNCTIONS_STREAMING_HH
#define MANIFOLDS_FUNCTIONS_STREAMING_HH

#include <functional>
#include <ostream>
#include <map>
#include <algorithm>
#include <iterator>
#include "composition.hh"
#include "addition.hh"
#include "multiplication.hh"
#include "polynomial.hh"
#include "variables.hh"
#include "function_matrix.hh"
#include "division.hh"
#include "transpose.hh"
#include "zero.hh"
#include "integral_polynomial.hh"
#include "unary_minus.hh"
#include "trig.hh"
#include "complex.hh"

namespace manifolds {
struct Stream2Wrapper {
  template <class VNamer>
  static void Stream2(std::ostream &s, Transpose, VNamer) {
    s << "Tr";
  }

#define STD_STREAM2(Func, func)                                                \
  template <class V> static void Stream2(std::ostream &s, Func, V) {           \
    s << #func;                                                                \
  }

  STD_STREAM2(Sin, sin)
  STD_STREAM2(Cos, cos)
  STD_STREAM2(Tan, tan)
  STD_STREAM2(Log, log)
  STD_STREAM2(Sinh, sinh)
  STD_STREAM2(Cosh, cosh)
  STD_STREAM2(Tanh, tanh)
  STD_STREAM2(ASin, asin)
  STD_STREAM2(ACos, acos)
  STD_STREAM2(ATan, atan)
  STD_STREAM2(ASinh, asinh)
  STD_STREAM2(ACosh, acosh)
  STD_STREAM2(ATanh, atanh)
  STD_STREAM2(Exp, exp)
  STD_STREAM2(Sqrt, sqrt)
  STD_STREAM2(Pow, pow)
  STD_STREAM2(Real, real)
  STD_STREAM2(Imag, imag)
  STD_STREAM2(Phase, phase)
  STD_STREAM2(Sign, sign)
  STD_STREAM2(Norm, norm)
  STD_STREAM2(Conjugate, conj)

#undef STD_STREAM2

  template <class V, class T>
  static void Stream2(std::ostream &s, ImagN<T>, V) {
    s << "I";
  }

  template <class... Args, class VNamer>
  static void Stream2(std::ostream &s, Group<Args...> g, VNamer vn) {
    Stream2Tuple(s, g.GetFunctions(), vn);
  }

  template <class N, class D, class VNamer>
  static void Stream2(std::ostream &s, Division<N, D> d, VNamer v) {
    s << '(';
    Stream2(s, d.GetNumerator(), v);
    s << " / ";
    Stream2(s, d.GetDenominator(), v);
    s << ')';
  }

  template <class VNamer> static void Stream2(std::ostream &s, Zero, VNamer) {
    s << "0";
  }

  template <class VNamer>
  static void Stream2Tuple(std::ostream &s, tuple<>, VNamer) {}

  template <
      class Tuple, class VNamer,
      class = typename std::enable_if<std::tuple_size<Tuple>::value != 0>::type>
  static void Stream2Tuple(std::ostream &s, const Tuple &t, VNamer vn) {
    Stream2(s, std::get<0>(t), vn);
    if (std::tuple_size<Tuple>::value > 1)
      s << ", ";
    Stream2Tuple(s, remove_element<0>(t), vn);
  }

  template <class c, class... Functions, class VNamer>
  static void Stream2FMatrix(std::ostream &s,
                             FunctionMatrix<int_<0>, c, Functions...> fm,
                             VNamer vn) {}

  template <class r, class c, class... Functions, class VNamer,
            class = typename std::enable_if<r::value != 0>::type>
  static void Stream2FMatrix(std::ostream &s,
                             FunctionMatrix<r, c, Functions...> fm, VNamer vn) {
    s << "{";
    Stream2Tuple(s, subset<0, c::value>(fm.GetFunctions()), vn);
    s << "}";
    if (r::value > 1)
      s << ", ";
    auto t = GetFunctionMatrix<r::value - 1, c::value>(
        subset<r::value, c::value *(r::value - 1)>(fm.GetFunctions()));
    Stream2FMatrix(s, t, vn);
  }

  template <class... Functions, class VNamer>
  static void Stream2(std::ostream &s, FunctionMatrix<Functions...> fm,
                      VNamer vn) {
    s << "{";
    Stream2FMatrix(s, fm, vn);
    s << "}";
  }

  template <int N, bool abelian, class VNamer>
  static void Stream2(std::ostream &s, Variable<N, abelian> v, VNamer vn) {
    s << vn(v);
  }

  template <class Func, class VNamer>
  static void Stream2(std::ostream &s, Composition<Func> c, VNamer vn) {
    Stream2(s, std::get<0>(c.GetFunctions()), vn);
  }

  template <class CType, class Order, class... Funcs, class VNamer,
            class = typename std::enable_if<(sizeof...(Funcs) > 0)>::type>
  static void Stream2(std::ostream &s,
                      Composition<Polynomial<CType, Order>, Funcs...> c,
                      VNamer vn) {
    std::stringstream ss;
    Stream2(ss, Composition<Funcs...>(remove_element<0>(c.GetFunctions())), vn);
    std::string var = ss.str();
    for (auto c : var) {
      if (!std::isalnum(c)) {
        var = "(" + var + ")";
        break;
      }
    }
    Stream2(s, std::get<0>(c.GetFunctions()), vn, var);
  }

  template <IPInt_t... ts, class... Funcs, class VNamer,
            class = typename std::enable_if<(sizeof...(Funcs) > 0)>::type>
  static void Stream2(std::ostream &s,
                      Composition<IntegralPolynomial<ts...>, Funcs...> c,
                      VNamer vn) {
    auto p = std::get<0>(c.GetFunctions()).ToPoly();
    Composition<Polynomial<IPInt_t, int_<(sizeof...(ts))> >, Funcs...> c2 =
        insert_element<0>(remove_element<0>(c.GetFunctions()), p);
    Stream2(s, c2, vn);
  }

  template <class F, class... Funcs, class VNamer,
            class = typename std::enable_if<(sizeof...(Funcs) > 0) &&
                                            !is_polynomial<F>::value>::type>
  static void Stream2(std::ostream &s, Composition<F, Funcs...> c, VNamer vn) {
    Stream2(s, std::get<0>(c.GetFunctions()), vn);
    s << '(';
    Stream2(s, Composition<Funcs...>(remove_element<0>(c.GetFunctions())), vn);
    s << ')';
  }

  template <class T, class VNamer>
  static void Stream2(std::ostream &s, UnaryMinus<T> u, VNamer vn) {
    s << "-(";
    Stream2(s, u.GetFunction(), vn);
    s << ")";
  }

  template <IPInt_t... ts, class VNamer>
  static void Stream2(std::ostream &s, IntegralPolynomial<ts...> ip,
                      VNamer vn) {
    return Stream2(s, ip.ToPoly(), vn);
  }

  template <class CoeffType, class Order, class VNamer>
  static void Stream2(std::ostream &s, Polynomial<CoeffType, Order> p,
                      VNamer vn, std::string var = "") {
    if (var == "")
      var = "p";
    bool started = false;
    CoeffType z(0);
    for (unsigned i = 0; i < Order::value; i++) {
      CoeffType c = p[i];
      if (c == z)
        continue;
      if (started)
        s << (c > z ? " + " : " - ");
      else {
        started = true;
        if (c < z)
          s << '-';
      }
      c = std::abs(c);
      if (i == 0 || c != 1)
        s << c;
      if (i > 0 && c != 1)
        s << " * ";
      if (i == 1)
        s << var;
      else if (i > 1)
        s << var << " ^ " << i;
    }
    if (!started)
      s << '0';
  }

  template <class Func, class VNamer>
  static void Stream2(std::ostream &s, Addition<Func> a, VNamer vn) {
    Stream2(s, std::get<0>(a.GetFunctions()), vn);
  }

  template <class F, class... Funcs, class VNamer,
            class = typename std::enable_if<(sizeof...(Funcs) > 0)>::type>
  static void Stream2(std::ostream &s, Addition<F, Funcs...> a, VNamer vn) {
    Stream2(s, std::get<0>(a.GetFunctions()), vn);
    s << " + ";
    Stream2(s, Addition<Funcs...>(remove_element<0>(a.GetFunctions())), vn);
  }

  template <class, class = void> struct RequiresParens : bool_<false> {};

  template <class F, class... Fs>
  struct RequiresParens<Composition<F, Fs...> > : RequiresParens<F> {};

  template <class... Fs>
  struct RequiresParens<Addition<Fs...> > : bool_<true> {};

  template <class C, class O>
  struct RequiresParens<
      Polynomial<C, O>,
      typename std::enable_if<(O::value > 1)>::type> : bool_<true> {};

  template <IPInt_t... coeffs>
  struct RequiresParens<
      IntegralPolynomial<coeffs...>,
      typename std::enable_if<(sizeof...(coeffs) > 1)>::type> : bool_<true> {};

  template <class T, class VNamer>
  static void PrintMultElem(std::ostream &s, T t, VNamer vn) {
    static const bool b = RequiresParens<T>::value;
    if (b)
      s << '(';
    Stream2(s, t, vn);
    if (b)
      s << ')';
  }

  template <class F, class VNamer>
  static void Stream2(std::ostream &s, Multiplication<F> m, VNamer vn) {
    PrintMultElem(s, std::get<0>(m.GetFunctions()), vn);
  }

  template <class F, class... Funcs, class VNamer,
            class = typename std::enable_if<(sizeof...(Funcs) > 0)>::type>
  static void Stream2(std::ostream &s, Multiplication<F, Funcs...> m,
                      VNamer vn) {
    PrintMultElem(s, std::get<0>(m.GetFunctions()), vn);
    s << " * ";
    Stream2(s, Multiplication<Funcs...>(remove_element<0>(m.GetFunctions())),
            vn);
  }
};

template <class T, class VariableNamer>
std::ostream &Stream2(std::ostream &s, T t, VariableNamer vn) {
  Stream2Wrapper::Stream2(s, t, std::ref(vn));
  return s;
}

template <class T> std::ostream &Stream2(std::ostream &s, T t) {
  Stream2Wrapper::Stream2(s, t, DefaultVariableNamer());
  return s;
}

struct CustomVariableNamer {
  std::map<std::pair<int, bool>, std::string> names;
  CustomVariableNamer(std::initializer_list<tuple<int, bool, std::string> > l) {
    std::transform(std::begin(l), std::end(l),
                   std::inserter(names, names.begin()), [](auto x) {
      return std::make_pair(std::make_pair(std::get<0>(x), std::get<1>(x)),
                            std::get<2>(x));
    });
  }

  CustomVariableNamer(std::initializer_list<std::pair<int, std::string> > l) {
    for (auto i : l) {
      names[std::make_pair(i.first, false)] =
          names[std::make_pair(i.first, true)] = i.second;
    }
  }

  template <int N, bool a> std::string operator()(Variable<N, a> v) const {
    auto i = names.find(std::make_pair(N, a));
    if (i == names.end())
      return DefaultVariableNamer()(v);
    return i->second;
  }
};

    template <class F, class = typename std::enable_if<is_function<F>::type::value>::type>
    std::ostream &operator<<(std::ostream &s, const F & f) {
        return Stream2(s, f);
    }

}

#endif
