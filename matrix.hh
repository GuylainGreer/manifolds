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

  template <class CoeffType2>
  bool operator==(const Matrix<rows,cols,CoeffType2> & m) const
  {
    return std::equal(coeffs.begin(), coeffs.end(),
		      m.coeffs.begin());
  }

private:
  std::array<CoeffType, rows*cols> coeffs;
};

  enum class MatrixStorage
  {
    RowMajor,
      ColumnMajor,
      ColMajor = ColumnMajor
  };

  template <std::size_t rows, std::size_t cols,
	    class ... Coeffs>
  auto GetMatrix(std::integral_constant<
		 MatrixStorage, MatrixStorage::RowMajor>,
		 Coeffs...args)
  {
    typedef typename std::common_type<
      Coeffs...>::type Type;
    std::array<Type, rows*cols> a = {{Type(args)...}};
    return Matrix<rows,cols,Type>(a);
  }

  template <std::size_t rows, std::size_t cols,
	    class ... Coeffs>
  auto GetMatrix(std::integral_constant<
		 MatrixStorage, MatrixStorage::ColMajor>,
		 Coeffs...args)
  {
    auto matrix =
      GetMatrix<cols,rows>(std::integral_constant<
			   MatrixStorage,
			   MatrixStorage::RowMajor>(),
			   args...);
    typedef typename std::common_type<
      Coeffs...>::type Type;
    Matrix<rows,cols,Type> result;
    for(unsigned i = 0; i < rows; i++)
      for(unsigned j = 0; j < cols; j++)
	result.Coeff(i,j) = matrix.Coeff(j,i);
    return result;
  }

  template <std::size_t rows, std::size_t cols,
	    MatrixStorage storage, class ... Args>
  auto GetMatrix(Args...args)
  {
    return GetMatrix<rows, cols>(std::integral_constant<
				 MatrixStorage, storage>(),
				 args...);
  }

  template <std::size_t rows, std::size_t cols,
	    class ... Args>
  auto GetMatrix(Args ... args)
  {
    return GetMatrix<rows, cols,
		     MatrixStorage::RowMajor>(args...);
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
}

#include "matrix_simplifications.hh"

#endif
