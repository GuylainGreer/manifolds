#ifndef MANIFOLDS_VARIABLE_INTERFACE_HH
#define MANIFOLDS_VARIABLE_INTERFACE_HH

#include "function.hh"
#include "full_function_defs.hh"
#include <tuple>

namespace manifolds {

  template <int N, bool abelian>
  struct VariableImpl : MultiFunction
  {
  private:
    template<class>
    struct not_tuple:std::true_type{};
    template <class...As>
    struct not_tuple<std::tuple<As...>>:std::false_type{};
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
      typedef decltype(std::tie(arg,args...)) tuple_type;
      return std::get<N>(tuple_type(arg,args...));
    }
  };

  template <int N, bool abelian = true>
  struct Variable : VariableImpl<N,abelian>,
    FunctionCommon<Variable<N,abelian>>
  {
    using FunctionCommon<Variable>::operator();
    using VariableImpl<N, abelian>::operator();
  };

#define VARIABLE(i, abelian, name)				\
  static const Variable<i,abelian> name =			\
  Variable<i,abelian>();					\
  inline std::ostream & operator<<(std::ostream&s,		\
				   Variable<i,abelian>)		\
  {return s << #name;}

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

}

#endif
