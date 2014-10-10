#ifndef MANIFOLDS_TRIGONOMETRY_HH
#define MANIFOLDS_TRIGONOMETRY_HH

#include "function.hh"
#include <cmath>
#include <ostream>
#include "full_function_defs.hh"

#define STD_FUNCTION(classname , funcname, index)		\
    struct classname : Function<int_<index>, 1,1>,              \
                       FunctionCommon<classname>                \
    {								\
        static const bool stateless = true;                     \
        static const ComplexOutputBehaviour complex_output =	\
            SameAsInput;                                        \
        template <class T>                                      \
        auto eval(T t) const                                    \
        {                                                       \
            using std::funcname;                                \
            return funcname(t);					\
        }                                                       \
        void Print(std::ostream & s)const{s << #funcname;}      \
    };								\
    template <class T> bool operator==(classname,T)		\
    {return std::is_same<T,classname>::value;}			\
    static const classname funcname = classname();		\
    inline std::ostream & operator<<(std::ostream & s,          \
                                     classname)                 \
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

  struct Pow : Function<int_<23>, 2,1>,
      FunctionCommon<Pow>
  {
    static const bool stateless = true;
    static const ComplexOutputBehaviour complex_output =
      SameAsInput;
    template <class T, class U>
    auto eval(T t, U u) const
    {
      return std::pow(t, u);
    }
  };

  inline std::ostream & operator<<(std::ostream & s, Pow p)
  {
    return s << "pow";
  }
}

#endif
