#ifndef MANIFOLDS_FUNCTIONS_MATRICES_HH
#define MANIFOLDS_FUNCTIONS_MATRICES_HH

#include <algorithm>
#include <array>
#include <iostream>

namespace manifolds {

    template <std::size_t rows, std::size_t cols,
              class CoeffType>
    struct Matrix
    {
        static const std::size_t num_rows = rows;
        static const std::size_t num_cols = cols;
        typedef CoeffType coefficient_type;

        Matrix():coeffs{}{}
        template <class T>
        Matrix(const std::array<T, rows*cols> & a): coeffs(a){}

        CoeffType & Coeff(int row, int col)
        {
            return coeffs[row*cols+col];
        }

        const CoeffType & Coeff(int row, int col) const
        {
            return coeffs[row*cols+col];
        }

        auto & GetCoeffs()
        {
            return coeffs;
        }

        const auto & GetCoeffs() const
        {
            return coeffs;
        }

        template <class Type>
        auto operator()(Matrix<rows,1,Type> vect) const
        {
            typename std::common_type<
                Type,CoeffType>::type result = 0;
            for(unsigned i = 0; i < rows; i++)
                for(unsigned j = 0; j < cols; j++)
                    result += coeffs[i * cols + j] * vect.coeffs[j];
            return result;
        }

        template <class ... Args>
        auto operator()(Args ... args) const
        {
            typedef typename std::common_type<
                Args...>::type Type;
            static const std::size_t num_values =
                sizeof...(Args);
            static_assert(num_values == cols,
                          "Matrix evaluation is matrix "
                          "multiplication, requires num_rows "
                          "values to do");
            std::array<Type, cols> vect = {{Type(args)...}};
            return (*this)(Matrix<cols,1,Type>(vect));
        }

    private:
        std::array<CoeffType, rows*cols> coeffs;
    };

    template <std::size_t rows, std::size_t cols,
              class ... Coeffs>
    auto GetRowMatrix(Coeffs...args)
    {
        typedef typename std::common_type<
            Coeffs...>::type Type;
        std::array<Type, rows*cols> a = {{Type(args)...}};
        return Matrix<rows,cols,Type>(a);
    }

    template <std::size_t rows, std::size_t cols,
              class ... Coeffs>
    auto GetColMatrix(Coeffs...args)
    {
        auto matrix =
            GetRowMatrix<cols,rows>(args...);
        typedef typename std::common_type<
            Coeffs...>::type Type;
        Matrix<rows,cols,Type> result;
        for(unsigned i = 0; i < rows; i++)
            for(unsigned j = 0; j < cols; j++)
                result.Coeff(i,j) = matrix.Coeff(j,i);
        return result;
    }

    template <std::size_t rows, std::size_t cols,
              class ... Args>
    auto GetMatrix(Args ... args)
    {
        return GetRowMatrix<rows, cols>(args...);
    }

    template <std::size_t r, std::size_t c, class T>
    std::ostream & operator<<(std::ostream & s,
                              const Matrix<r,c,T> & m)
    {
        s << "Matrix<" << r << ',' << c
          << ">{";
        for(std::size_t i = 0; i < r; i++)
        {
            s << '{' << m.Coeff(i,0);
            for(std::size_t j = 1; j < c; j++)
                s << ", " << m.Coeff(i,j);
            s << '}';
            if(i != r-1) s << ',';
        }
        s << '}';
        return s;
    }

    template <std::size_t length, class ... CoeffTypes>
    auto GetVector(CoeffTypes...cs)
    {
        return GetMatrix<length,1>(cs...);
    }

    template <std::size_t length, class ... CoeffTypes>
    auto GetRowVector(CoeffTypes...cs)
    {
        return GetMatrix<1,length>(cs...);
    }

    template <class F>
    struct IsMatrix : std::false_type{};

    template <std::size_t r,
              std::size_t c,
              class C>
    struct IsMatrix<Matrix<r,c,C>> : std::true_type{};

    template <std::size_t rows, std::size_t cols,
              class CoeffType, class CoeffType2>
    bool operator==(const Matrix<rows,cols,CoeffType> & m1,
                    const Matrix<rows,cols,CoeffType2> & m2)
    {
        return std::equal(m1.GetCoeffs().begin(),
                          m1.GetCoeffs().end(),
                          m2.GetCoeffs().begin());
    }

}

#include "matrix_operators.hh"

#endif
