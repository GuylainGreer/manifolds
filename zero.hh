#ifndef MANIFOLDS_ZERO_FUNCTION_HH
#define MANIFOLDS_ZERO_FUNCTION_HH

#include "full_function_defs.hh"
#include <ostream>

namespace manifolds {
  struct ZeroImpl : Function<int_<25>, 1,1>
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
  struct Simplification<Addition<T,Zero>, /*add_f_z*/0>
  {
    typedef T type;

    static type Combine(Addition<T,Zero> t)
    {
      SIMPLIFY_INFO("Simplifying adding zero\n");
      return get<0>(t.GetFunctions());
    }
  };

  template <class T>
  struct Simplification<
    Addition<Zero,T>, /*add_z_f*/1,
    typename std::enable_if<
      !std::is_same<Zero,T>::value>::type>
  {
    typedef T type;

    static type Combine(Addition<Zero,T> t)
    {
#ifdef PRINT_SIMPLFIES
      std::cout << "Simplifying adding zero inverse\n";
#endif
      return get<1>(t.GetFunctions());
    }
  };

  template <class T>
  struct Simplification<Multiplication<T,Zero>, /*mult_f_z*/1>
  {
    typedef Zero type;
    static type Combine(Multiplication<T,Zero>)
    {
      SIMPLIFY_INFO("Simplifying multiplication by zero\n");
      return zero;
    }
  };

  template <class T>
  struct Simplification<Multiplication<Zero,T>,
			/*mult_z_f*/0,
		  typename std::enable_if<
		    !std::is_same<T,Zero>::value>::type>
  {
    typedef Zero type;

    static type Combine(Multiplication<Zero,T> )
    {
      SIMPLIFY_INFO("Simplifying multiplication by "
		    "zero inverse\n");
      return zero;
    }
  };

  template <class T>
  struct Simplification<Composition<Zero, T>, /*com_z_f*/0>
  {
    typedef Zero type;
    static type Combine(Composition<Zero, T>)
    {
      SIMPLIFY_INFO("Simpifying composition of zero "
		    "and function\n");
      return zero;
    }
  };
}

#endif
