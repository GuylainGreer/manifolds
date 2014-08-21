#ifndef MANIFOLDS_VARIABLE_INTERFACE_HH
#define MANIFOLDS_VARIABLE_INTERFACE_HH

#include "function.hh"
#include "full_function_defs.hh"
#include "tuple_util.hh"
#include <string>

namespace manifolds {

  //Offset of 10000 should be enough to avoid conflicts
  template <int N, bool abelian>
  struct VariableImpl : Function<int_<10000+N>, N+1,1>
  {
  public:
    static const bool stateless = true;
    static const bool abelian_arithmetic = abelian;
    template <class Arg, class ... Args,
	      class = typename std::enable_if<
		!is_function<Arg>::value>::type>
    auto operator()(Arg arg, Args ... args) const
    {
      static_assert(N < sizeof...(Args)+1,
		    "Not enough arguments to extract Nth");
      return get<N>(make_my_tuple(arg,args...));
    }
  };

  template <int N, bool abelian = true>
  struct Variable : VariableImpl<N,abelian>,
    FunctionCommon<Variable<N,abelian>>
  {
    using FunctionCommon<Variable>::operator();
    using VariableImpl<N, abelian>::operator();
  };

  struct DefaultVariableNamer
  {
    template <int N, bool abelian>
    std::string operator()(Variable<N,abelian>) const
    {
      switch(2 * N + abelian)
        {
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
	  if(abelian)
	    return std::string("Variable<") +
	      std::to_string(N) + ">";
	  else
	    return std::string("VARIABLE<") +
	      std::to_string(N) + ">";
        }
    }
  };

  template <int i, bool abelian>
  std::ostream & operator<<(std::ostream&s, Variable<i,abelian> v)
  {
    return s << DefaultVariableNamer()(v);
  }

#define VARIABLE(i, abelian, name)				\
  static const Variable<i,abelian> name =			\
  Variable<i,abelian>();					\

  VARIABLE(0,true,x)
  VARIABLE(1,true,y)
  VARIABLE(2,true,z)
  VARIABLE(3,true,t)

  //WARNING! The following variables are non-abelian,
  //meaning that A*B != B*A, use them for matrices
  //(for example)
  VARIABLE(0,false,X)
  VARIABLE(1,false,Y)
  VARIABLE(2,false,Z)
  VARIABLE(3,false,T)

template <bool a, class F>
  struct Simplification<
    Composition<Variable<0,a>, F>, 2,
    typename std::enable_if<
      F::output_dim == 1>::type>
  {
    typedef F type;

    static type Combine(Composition<Variable<0,a>, F> c)
    {
#ifdef PRINT_SIMPLIFIES
      std::cout << "Simplifying composition of x and single "
	"dimensional function\n";
#endif
      return get<1>(c.GetFunctions());
    }
  };
}

#endif
