#ifndef MANIFOLDS_POLYNOMIAL_HH
#define MANIFOLDS_POLYNOMIAL_HH

#include "full_function_defs.hh"
#include "function.hh"
#include <array>
#include <type_traits>
#include <ostream>
#include <iostream>
#include <algorithm>
#include "typename.hh"

namespace manifolds
{
    //Somewhat confusingly,
    //order here refers to the number of coefficients
    template <class CoeffType, class Order>
    struct PolynomialImpl :
        Function<int_<6>, 1, 1>
    {
        static const int num_coeffs = Order::value;
        static const bool stateless = false;
        static const ComplexOutputBehaviour complex_output =
            is_complex<CoeffType>::value ?
            AlwaysComplex : SameAsInput;

        template <class Type, std::size_t N>
        PolynomialImpl(std::array<Type, N> t):
            coeffs(t){}

        template <class Type, class N>
        PolynomialImpl(const PolynomialImpl<Type, N> & t)
        {
            std::copy(t.GetCoeffs().begin(),
                      t.GetCoeffs().end(),
                      coeffs.begin());
        }

        template <class T, class ... Ts>
        auto operator()(T t, Ts ...) const
        {
            static_assert(sizeof...(Ts) == 0 ||
                          Order::value == 1,
                          "Can only call polynomial "
                          "with multiple arguments when "
                          "polynomial is a constant");
            typename common_type<
                T,CoeffType>::type result = 0, t2 = t;
            for(auto i = coeffs.rbegin(); i != coeffs.rend(); i++)
            {
                result = result * t2 + *i;
            }
            return result;
        }

        const auto & GetCoeffs() const
        {
            return coeffs;
        }

        void Print(std::ostream & s) const
        {
            s << "Polynomial" << Order::value << "(";
            for(int i = 0; i < Order::value; i++)
            {
                s << coeffs[i];
                if(i != Order::value-1)
                    s << ' ';
            }
            s << ")";
        }
    private:
        std::array<CoeffType,Order::value> coeffs;
    };

    DEF_FF_TEMPLATE(Polynomial)

    template <class>
    struct is_polynomial : std::false_type{};

    template <class C, class O>
    struct is_polynomial<Polynomial<C, O>> : std::true_type{};

    template <class>
    struct PolynomialOrder : int_<-1>{};

    template <class C, class O>
    struct PolynomialOrder<Polynomial<C, O>> : O {};

    inline Polynomial<long double,int_<1>>
                         operator""_c(long double x)
    {
        std::array<long double, 1> a{{x}};
        return {a};
    }

    inline Polynomial<long long,int_<1>>
                         operator""_c(long long unsigned x)
    {
        std::array<long long, 1> a{{(long long)x}};
        return {a};
    }

    template <class ... InputTypes>
    auto GetPolynomial(InputTypes ... coeffs)
    {
        typedef typename std::common_type<
            InputTypes...>::type coeff_type;
        std::array<coeff_type, sizeof...(InputTypes)>
            coeff_arr{{coeff_type(coeffs)...}};
        return Polynomial<
            coeff_type,
            int_<sizeof...(InputTypes)>>(coeff_arr);
    }

    template <class T, class C, class U>
    bool operator==(const Polynomial<T,C> & p1,
                      const Polynomial<U,C> & p2)
    {
        return std::equal(p1.GetCoeffs().begin(),
                          p1.GetCoeffs().end(),
                          p2.GetCoeffs().begin(),
                          p2.GetCoeffs().end());
    }
}

#endif
