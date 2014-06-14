#ifndef MANIFOLDS_TRIGONOMETRY_HH
#define MANIFOLDS_TRIGONOMETRY_HH

#include "function.hh"
#include <cmath>
#include <ostream>
#include "full_function_defs.hh"

#define STD_FUNCTION(classname , funcname)		\
  struct classname##Impl : Function			\
  {							\
    inline friend std::ostream &			\
      operator<<(std::ostream & s, classname##Impl){	\
      return s << #funcname;}				\
    template <class T>					\
      auto operator()(T && t) const			\
    {							\
      return std::funcname(t);				\
    }							\
  };							\
  DEF_FULL_FUNCTION(classname)

namespace manifolds
{
  STD_FUNCTION(Sin, sin)
  STD_FUNCTION(Cos, cos)
  STD_FUNCTION(Tan, tan)
  STD_FUNCTION(Log, log)
  STD_FUNCTION(Sinh, sinh)
  STD_FUNCTION(Cosh, cosh)
  STD_FUNCTION(Tanh, tanh)
  STD_FUNCTION(ASin, asin)
  STD_FUNCTION(ACos, acos)
  STD_FUNCTION(ATan, atan)
  STD_FUNCTION(ASinh, asinh)
  STD_FUNCTION(ACosh, acosh)
  STD_FUNCTION(ATanh, atanh)
  STD_FUNCTION(Exp, exp)
}

#undef STD_FUNCTION

struct PowImpl
{
  template <class T, class U>
  auto operator()(T && t, U && u) const
  {
    return std::pow(t, u);
  }
};

#endif
