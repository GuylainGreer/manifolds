#ifndef MANIFOLDS_POLYNOMIAL_HH
#define MANIFOLDS_POLYNOMIAL_HH

#include "function.hh"
#include <ratio>
#include <array>

namespace manifolds
{

  template <class OutputUnits,
	    class InputUnits, class ... Coeffs>
  struct Polynomial : Function
  {
    template <class T>
    auto operator()(T && t) const
    {
      auto unitless = t/InputUnits(1);
      static const std::array<
	double, sizeof...(Coeffs)> coeffs =
	{{to_double(Coeffs())...}};
      double result = 0;
      for(auto i = coeffs.rbegin(); i != coeffs.rend(); i++)
	result = result * unitless + *i;
      return OutputUnits(result);
    }

  private:
    template <long n, long d>
    static double to_double(std::ratio<n,d>)
    {
      return double(n) / d;
    }
  };

}
#endif
