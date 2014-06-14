#ifndef MANIFOLDS_FUNCTIONS_UNARY_MINUS_HH
#define MANIFOLDS_FUNCTIONS_UNARY_MINUS_HH

#include "full_function_defs.hh"
#include "function.hh"

namespace manifolds {

template <class Func>
struct UnaryMinusImpl : Function, MultiFunction
{

  UnaryMinusImpl(){}
  UnaryMinusImpl(const Func & f):f(f){}

  template <class ... Args>
  auto operator()(Args && ... args) const
  {
    return -f(args...);
  }
  
private:
  Func f;
};

  DEF_FF_STEMPLATE(UnaryMinus)
}
#endif
