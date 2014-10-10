#ifndef MANIFOLDS_FUNCTION_ADDITION_HH
#define MANIFOLDS_FUNCTION_ADDITION_HH

#include "function.hh"
#include "full_function_defs.hh"
#include <utility>
#include "equals.hh"

namespace manifolds {

    template <class ... Functions>
    struct Addition : Function<
        list<int_<0>, typename Functions::indices...>,
        max<Functions::input_dim...>::value,
        max<Functions::output_dim...>::value>,
        FunctionCommon<Addition<Functions...>>
    {
        using FunctionCommon<Addition>::operator();
        static const bool stateless =
            and_<is_stateless<Functions>...>::value;

        static const ComplexOutputBehaviour complex_output =
            VariadicComplexOutput<Functions...>::value;

        Addition(Functions ... fs):
            functions(fs...){}

        Addition(){}
        Addition(tuple<Functions...> f):
            functions(f){}

        template <class ... Args>
        auto eval(Args ... args) const
        {
            return eval(int_<0>(), args...);
        }

        template <class ... Args>
        auto eval(int_<sizeof...(Functions)-1>,
                  Args ... args) const
        {
            return get<sizeof...(Functions)-1>
                (functions)(args...);
        }

        template <int N, class ... Args>
        auto eval(int_<N>, Args ... args) const
        {
            return get<N>(functions)(args...) +
                eval(int_<N+1>(), args...);
        }

        auto GetFunctions() const
        {
            return functions;
        }

        template <class Other>
        bool operator==(const Other &) const
        {
            return false;
        }

        template <class ... Rights>
        bool operator==(const Addition<Rights...> & r) const
        {
            return VariadicEqual(*this, r);
        }

    private:
        tuple<Functions...> functions;
    };


    template <class ... Fs>
    auto AddRaw(Fs ... fs)
    {
        return Addition<Fs...>(fs...);
    }
}

#endif
