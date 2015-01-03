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

  template <class Tuple, class VNamer, class = typename std::enable_if<
                                           tuple_size<Tuple>::value != 0>::type>
  static void Stream2Tuple(std::ostream &s, const Tuple &t, VNamer vn) {
    Stream2(s, get<0>(t), vn);
    if (tuple_size<Tuple>::value > 1)
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
    Stream2(s, get<0>(c.GetFunctions()), vn);
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
    Stream2(s, get<0>(c.GetFunctions()), vn, var);
  }

  template <class Seq, class... Funcs, class VNamer,
            class = typename std::enable_if<(sizeof...(Funcs) > 0)>::type>
  static void Stream2(std::ostream &s,
                      Composition<IntegralPolynomial<Seq>, Funcs...> c,
                      VNamer vn) {
    auto p = get<0>(c.GetFunctions()).ToPoly();
    Composition<Polynomial<typename Seq::value_type, int_<Seq::size()> >,
                Funcs...> c2 =
        insert_element<0>(remove_element<0>(c.GetFunctions()), p);
    Stream2(s, c2, vn);
  }

  template <class F, class... Funcs, class VNamer,
            class = typename std::enable_if<(sizeof...(Funcs) > 0) &&
                                            !is_polynomial<F>::value>::type>
  static void Stream2(std::ostream &s, Composition<F, Funcs...> c, VNamer vn) {
    Stream2(s, get<0>(c.GetFunctions()), vn);
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

  template <class Seq, class VNamer>
  static void Stream2(std::ostream &s, IntegralPolynomial<Seq> ip, VNamer vn) {
    return Stream2(s, ip.ToPoly(), vn);
  }

  template <class CoeffType, class Order, class VNamer>
  static void Stream2(std::ostream &s, Polynomial<CoeffType, Order> p,
                      VNamer vn, std::string var = "") {
    if (var == "")
      var = "p";
    auto coeffs = p.GetCoeffs();
    bool is_first = coeffs[0] == CoeffType(0);
    if (!is_first)
      s << coeffs[0];
    for (unsigned i = 1; i < coeffs.size(); i++) {
      if (coeffs[i] == CoeffType(0))
        continue;
      if (is_first) {
        if (coeffs[i] == CoeffType(1))
          ;
        else if (coeffs[i] == CoeffType(-1))
          s << '-';
        else
          s << coeffs[i] << " * ";
        is_first = false;
      } else if (coeffs[i] > 0) {
        s << " + ";
        if (coeffs[i] != 1)
          s << coeffs[i] << " * ";
      } else {
        s << " - ";
        if ((double)coeffs[i] != -1)
          s << -coeffs[i] << " * ";
      }
      s << var;
      if (i != 1)
        s << "**" << i;
    }
  }

  template <class Func, class VNamer>
  static void Stream2(std::ostream &s, Addition<Func> a, VNamer vn) {
    Stream2(s, get<0>(a.GetFunctions()), vn);
  }

  template <class F, class... Funcs, class VNamer,
            class = typename std::enable_if<(sizeof...(Funcs) > 0)>::type>
  static void Stream2(std::ostream &s, Addition<F, Funcs...> a, VNamer vn) {
    Stream2(s, get<0>(a.GetFunctions()), vn);
    s << " + ";
    Stream2(s, Addition<Funcs...>(remove_element<0>(a.GetFunctions())), vn);
  }

  template <class T, class VNamer>
  static void PrintMultElem(std::ostream &s, T t, VNamer vn) {
    static const bool b =
        IsVariadic<Addition, T>::value ||
        (is_polynomial<T>::value && PolynomialOrder<T>::value != 1);
    if (b)
      s << '(';
    Stream2(s, t, vn);
    if (b)
      s << ')';
  }

  template <class F, class VNamer>
  static void Stream2(std::ostream &s, Multiplication<F> m, VNamer vn) {
    PrintMultElem(s, get<0>(m.GetFunctions()), vn);
  }

  template <class F, class... Funcs, class VNamer,
            class = typename std::enable_if<(sizeof...(Funcs) > 0)>::type>
  static void Stream2(std::ostream &s, Multiplication<F, Funcs...> m,
                      VNamer vn) {
    PrintMultElem(s, get<0>(m.GetFunctions()), vn);
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
      return std::make_pair(std::make_pair(get<0>(x), get<1>(x)), get<2>(x));
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

template <class Arg, class... Args>
std::ostream &operator<<(std::ostream &s, Composition<Arg, Args...> c) {
  return Stream2(s, c);
}

template <class A, class B>
std::ostream &operator<<(std::ostream &s, Division<A, B> d) {
  return Stream2(s, d);
}

template <class rows, class cols, class... Functions>
std::ostream &operator<<(std::ostream &s,
                         FunctionMatrix<rows, cols, Functions...> f) {
  return Stream2(s, f);
}

template <class F, class... Funcs>
std::ostream &operator<<(std::ostream &s, Group<F, Funcs...> g) {
  return Stream2(s, g);
}

template <class CType, int N>
std::ostream &operator<<(std::ostream &s, Polynomial<CType, int_<N> > p) {
  return Stream2(s, p);
}

template <class Seq>
std::ostream &operator<<(std::ostream &s, IntegralPolynomial<Seq> ip) {
  return s << ip.ToPoly();
}

template <class Func, class... Functions>
std::ostream &operator<<(std::ostream &s,
                         Multiplication<Func, Functions...> m) {
  return Stream2(s, m);
}

inline std::ostream &operator<<(std::ostream &s, Transpose t) {
  return Stream2(s, t);
}

template <class T> std::ostream &operator<<(std::ostream &s, UnaryMinus<T> u) {
  return Stream2(s, u);
}

inline std::ostream &operator<<(std::ostream &s, Zero z) {
  return Stream2(s, z);
}

template <class Func, class... Functions>
std::ostream &operator<<(std::ostream &s, Addition<Func, Functions...> m) {
  return Stream2(s, m);
}

#define EASY_STREAM(Func, name)                                                \
  inline std::ostream &operator<<(std::ostream &s, Func) { return s << #name; }

EASY_STREAM(Real, real)
EASY_STREAM(Imag, imag)
EASY_STREAM(Phase, phase)
EASY_STREAM(Sign, sign)
EASY_STREAM(Norm, norm)
template <class T> EASY_STREAM(ImagN<T>, I)
}

#endif
