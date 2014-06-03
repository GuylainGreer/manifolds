#ifndef MANIFOLDS_FUNCTIONS_UNARY_MINUS_HH
#define MANIFOLDS_FUNCTIONS_UNARY_MINUS_HH

#include "function.hh"

namespace manifolds {

template <class Func>
struct UnaryMinus : Function, MultiFunction
{

  UnaryMinus(){}
  UnaryMinus(const Func & f):f(f){}

  template <class ... Args>
  auto operator()(Args && ... args)
  {
    return -f(args...);
  }
  
private:
  Func f;
};

}
#endif
