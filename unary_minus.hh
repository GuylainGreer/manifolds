#ifndef MANIFOLDS_FUNCTIONS_UNARY_MINUS_HH
#define MANIFOLDS_FUNCTIONS_UNARY_MINUS_HH

#include "full_function_defs.hh"
#include "function.hh"

namespace manifolds {

template <class Func>
struct UnaryMinusImpl : MultiFunction
{

  UnaryMinusImpl(){}
  UnaryMinusImpl(const Func & f):f(f){}

  template <class ... Args>
  auto operator()(Args && ... args) const
  {
    return -f(args...);
  }
  
  auto GetFunction() const
  {
    return f;
  }

  auto GetFunctions() const
  {
    return std::make_tuple(f);
  }

private:
  Func f;
};

  DEF_FF_STEMPLATE(UnaryMinus)

  template <class T>
  std::ostream & operator<<(std::ostream & s,
			    UnaryMinus<T> u)
  {
    return s << '-' << u.GetFunction();
  }

  template <class T, class U>
  bool operator==(UnaryMinus<T>, U)
  {
    return false;
  }

  template <class T>
  bool operator==(UnaryMinus<T> u1, UnaryMinus<T> u2)
  {
    return u1.f == u2.f;
  }
}
#endif
