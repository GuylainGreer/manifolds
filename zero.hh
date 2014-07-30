#ifndef MANIFOLDS_ZERO_FUNCTION_HH
#define MANIFOLDS_ZERO_FUNCTION_HH

#include "full_function_defs.hh"
#include <ostream>

namespace manifolds {
  struct ZeroImpl : MultiFunction
{
  static const bool stateless = true;

  template <class ... Args>
  auto operator()(Args...) const
  {
    return 0;
  }
};

  DEF_FULL_FUNCTION(Zero)

  static const Zero zero = Zero();

  inline std::ostream & operator<<(std::ostream & s, Zero z)
  {
    return s << "Zero";
  }
}

#include "simplify.hh"
#include "addition.hh"
#include "multiplication.hh"

namespace manifolds {
  template <class T>
  struct Simplification<Addition<T,Zero>>
  {
    typedef T type;

    static type Combine(Addition<T,Zero> t)
    {
      return std::get<0>(t.GetFunctions());
    }
  };

  template <class T>
  struct Simplification<
    Addition<Zero,T>,
    typename std::enable_if<
      !std::is_same<Zero,T>::value>::type>
  {
    typedef T type;

    static type Combine(Addition<Zero,T> t)
    {
      return std::get<1>(t.GetFunctions());
    }
  };

  template <class T>
  struct Simplification<Multiplication<T,Zero>>
  {
    typedef Zero type;
    static type Combine(Multiplication<T,Zero>)
    {
      return zero;
    }
  };

  template <class T>
  struct Simplification<Multiplication<Zero,T>,
		  typename std::enable_if<
		    !std::is_same<T,Zero>::value>::type>
  {
    typedef Zero type;

    static type Combine(Multiplication<Zero,T> )
    {
      return zero;
    }
  };
}

#endif
