
#ifndef MANIFOLDS_FUNCTIONS_UNARY_MINUS_HH
#define MANIFOLDS_FUNCTIONS_UNARY_MINUS_HH

#include "full_function_defs.hh"
#include "function.hh"

namespace manifolds {

template <class Func>
struct UnaryMinusImpl :
    Function<
  list<int_<24>, typename Func::indices>,
  Func::input_dim,
  Func::output_dim>
{

  UnaryMinusImpl(){}
  UnaryMinusImpl(const Func & f):f(f){}

  template <class ... Args>
  auto operator()(Args ... args) const
  {
    return -f(args...);
  }
  
  auto GetFunction() const
  {
    return f;
  }

  auto GetFunctions() const
  {
    return make_my_tuple(f);
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

  template <class F>
  struct Simplification<
    UnaryMinus<UnaryMinus<F>>,
    /*um_um_f*/0>
  {
    typedef F type;
    static type Combine(UnaryMinus<UnaryMinus<F>> u)
    {
      SIMPLIFY_INFO("Simplifying unary minus of unary "
		    "minus of function\n");
      return u.GetFunction().GetFunction();
    }
  };

  template <class F>
  struct Simplification<
    UnaryMinus<F>, /*um_f*/1>
  {
    static auto Combine(UnaryMinus<F> u)
    {
      SIMPLIFY_INFO("Simplifying unary minus of function\n");
      auto f = Simplify(u.GetFunction());
      return UnaryMinus<decltype(f)>(f);
    }

    typedef decltype(Combine(std::declval<UnaryMinus<F>>())) type;
  };
}
#endif
