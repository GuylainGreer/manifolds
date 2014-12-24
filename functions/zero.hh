#ifndef MANIFOLDS_ZERO_FUNCTION_HH
#define MANIFOLDS_ZERO_FUNCTION_HH

#include "full_function_defs.hh"
#include <ostream>

namespace manifolds {
    struct Zero : Function<int_<25>, 1,1>,
                  FunctionCommon<Zero>
    {
        static const bool stateless = true;
        static const ComplexOutputBehaviour complex_output =
            NeverComplex;

        template <class ... Args>
        auto eval(Args...) const
        {
            return 0;
        }
    };

    static const Zero zero = Zero();

    template <class T>
    bool operator==(Zero, T)
    {
        return std::is_same<Zero,T>::value;
    }
}

#endif
