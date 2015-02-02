#ifndef MANIFOLDS_VARIABLE_INTERFACE_HH
#define MANIFOLDS_VARIABLE_INTERFACE_HH

#include "function.hh"
#include "full_function_defs.hh"
#include "tuple_util.hh"
#include <string>

namespace manifolds {

// Offset of 10000 should be enough to avoid conflicts
template <int N, bool abelian = true, bool complex = false>
struct Variable : Function<int_<10000 + N>, N + 1, 1>,
                  FunctionCommon<Variable<N, abelian, complex> > {
public:
  static const bool stateless = true;
  static const bool abelian_arithmetic = abelian;
  static const ComplexOutputBehaviour complex_output =
      complex ? AlwaysComplex : NeverComplex;

  template <class Arg, class... Args,
            class = typename std::enable_if<!is_function<Arg>::value>::type>
  auto eval(Arg arg, Args... args) const {
    static_assert(N < sizeof...(Args) + 1,
                  "Not enough arguments to extract Nth");
    static_assert(complex ||
                      !is_complex<typename nth<N, Arg, Args...>::type>::value,
                  "Variable declared as real but "
                  "given a complex number as input.");
    return std::get<N>(make_my_tuple(arg, args...));
  }
};

struct DefaultVariableNamer {
  template <int N, bool abelian, bool complex>
  std::string operator()(Variable<N, abelian, complex>) const {
    switch (2 * N + abelian) {
    case 0:
      return "X";
    case 1:
      return "x";
    case 2:
      return "Y";
    case 3:
      return "y";
    case 4:
      return "Z";
    case 5:
      return "z";
    case 6:
      return "T";
    case 7:
      return "t";
    default:
      if (abelian)
        return std::string("Variable<") + std::to_string(N) + ">";
      else
        return std::string("VARIABLE<") + std::to_string(N) + ">";
    }
  }
};

template <int i, bool abelian, bool complex>
std::ostream &operator<<(std::ostream &s, Variable<i, abelian, complex> v) {
  return s << DefaultVariableNamer()(v);
}

#define VARIABLE(i, abelian, name)                                             \
  static const Variable<i, abelian> name = Variable<i, abelian>();

VARIABLE(0, true, x)
VARIABLE(1, true, y)
VARIABLE(2, true, z)
VARIABLE(3, true, t)

// WARNING! The following variables are non-abelian,
// meaning that A*B != B*A, use them for matrices
//(for example)
VARIABLE(0, false, X)
VARIABLE(1, false, Y)
VARIABLE(2, false, Z)
VARIABLE(3, false, T)

template <int i, bool a, bool c, class B>
bool operator==(Variable<i, a, c>, B) {
  return std::is_same<Variable<i, a, c>, B>::value;
}
}

#endif
