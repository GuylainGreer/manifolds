#ifndef MANIFOLDS_TRIGONOMETRY_HH
#define MANIFOLDS_TRIGONOMETRY_HH

#include "function.hh"
#include <cmath>
#include <ostream>
#include "full_function_defs.hh"

#define STD_FUNCTION(classname , funcname, index)		\
  struct classname##Impl : Function<int_<index>, 1,1>		\
  {								\
    static const bool stateless = true;				\
    template <class T>						\
      auto operator()(T t) const				\
    {								\
      using std::funcname;					\
      return funcname(t);					\
    }								\
    void Print(std::ostream & s)const{s << #funcname;}		\
  };								\
  DEF_FULL_FUNCTION(classname)					\
  template <class T> bool operator==(classname,T)		\
  {return std::is_same<T,classname>::value;}			\
  static const classname funcname = classname();		\
  inline std::ostream & operator<<(std::ostream & s, classname)	\
  {return s << #funcname;}

namespace manifolds
{
  STD_FUNCTION(Sin, sin, 8)
  STD_FUNCTION(Cos, cos, 9)
  STD_FUNCTION(Tan, tan, 10)
  STD_FUNCTION(Log, log, 11)
  STD_FUNCTION(Sinh, sinh, 12)
  STD_FUNCTION(Cosh, cosh, 13)
  STD_FUNCTION(Tanh, tanh, 14)
  STD_FUNCTION(ASin, asin, 15)
  STD_FUNCTION(ACos, acos, 16)
  STD_FUNCTION(ATan, atan, 17)
  STD_FUNCTION(ASinh, asinh, 18)
  STD_FUNCTION(ACosh, acosh, 19)
  STD_FUNCTION(ATanh, atanh, 20)
  STD_FUNCTION(Exp, exp, 21)
  STD_FUNCTION(Sqrt, sqrt, 22)
#undef STD_FUNCTION

  struct PowImpl : Function<int_<23>, 2,1>
  {
    static const bool stateless = true;
    template <class T, class U>
    auto operator()(T t, U u) const
    {
      return std::pow(t, u);
    }
  };

  DEF_FULL_FUNCTION(Pow)

  inline std::ostream & operator<<(std::ostream & s, Pow p)
  {
    return s << "pow";
  }
}

#include "multiplication.hh"
#include "polynomial.hh"

namespace manifolds {

  template <>
  struct Simplification<
    Multiplication<Cos, Cos>,
    /*mult_cos_cos*/0>
  {
    typedef Multiplication<Cos, Cos> in_type;
    static auto Combine(in_type)
    {
      SIMPLIFY_INFO("Simplifying cos**2\n");
      return ComposeRaw(GetPolynomial(1,0,-1), Sin());
    }

    //typedef decltype(Combine(std::declval<in_type>())) type;
  };

}

#endif
