#ifndef MANIFOLD_MATRIX_SIMPLICATIONS_HH
#define MANIFOLD_MATRIX_SIMPLICATIONS_HH

#include "simplify.hh"
#include "addition.hh"
#include "multiplication.hh"

namespace manifolds {

template <std::size_t rows1, std::size_t cols1,
	  class CoeffType1, class CoeffType2>
auto operator+(Matrix<rows1,cols1,CoeffType1> a,
	       Matrix<rows1,cols1,CoeffType2> b)
{
  typedef typename std::common_type<
    CoeffType1, CoeffType2>::type OutCoeff;

  Matrix<rows1,cols1,OutCoeff> r;
  for(std::size_t i = 0; i < rows1; i++)
    for(std::size_t j = 0; j < cols1; j++)
      r.Coeff(i,j) = a.Coeff(i,j) + b.Coeff(i,j);
  return r;
}


template <std::size_t rows1, std::size_t cols1,
	  class CoeffType1,
	  std::size_t rows2, std::size_t cols2,
	  class CoeffType2>
struct Simplification<
  Addition<Matrix<rows1,cols1,CoeffType1>,
	   Matrix<rows2,cols2,CoeffType2>>,0>
{
  static_assert(rows1 == rows2 && cols1 == cols2,
		"Matrix addition requires the two "
		"matrices have the same dimensions");

  typedef typename std::common_type<
    CoeffType1, CoeffType2>::type OutCoeff;

  typedef Matrix<rows1, cols1, OutCoeff> type;

  static type Combine(Addition<Matrix<rows1,cols1,CoeffType1>,
		      Matrix<rows2,cols2,CoeffType2>> a)
  {
#ifdef PRINT_SIMPLIFIES
    std::cout << "Simplifying matrix addition\n";
#endif
    auto ms = a.GetFunctions();
    return get<0>(a) + get<1>(a);
  }
};

  template <class T>
    auto MassageMultiplicationOutput(T t,
				     int_<1>,
				     int_<1>)
  {
    return t.Coeff(0,0);
  }

  template <class T, int i, int j>
  auto MassageMultiplicationOutput(T t,
				   int_<i>,
				   int_<j>)
  {
    return t;
  }

  template <class T>
  auto MassageMultiplicationOutput(T t)
  {
    return MassageMultiplicationOutput
      (t, int_<T::num_rows>(), int_<T::num_cols>());
  }

template <std::size_t rows1, std::size_t cols1,
	  std::size_t cols2,
	  class CoeffType1, class CoeffType2>
auto operator*(Matrix<rows1,cols1,CoeffType1> a,
	       Matrix<cols1,cols2,CoeffType2> b)
{
  Matrix<rows1, cols2,
	 typename std::common_type<
	   CoeffType1, CoeffType2>::type> r;
  for(std::size_t i = 0; i < rows1; i++)
    for(std::size_t j = 0; j < cols1; j++)
      for(std::size_t k = 0; k  < cols2; k++)
	r.Coeff(i,k) += a.Coeff(i,j) * b.Coeff(j,k);
  return MassageMultiplicationOutput(r);
}

  template <std::size_t rows, std::size_t cols,
	    class CoeffType1, class CoeffType2>
  auto operator*(CoeffType2 value,
		 Matrix<rows,cols,CoeffType1> m)
  {
    for(unsigned i = 0; i < rows; i++)
      for(unsigned j = 0; j < cols; j++)
	m.Coeff(i,j) *= value;
    return m;
  }

  template <std::size_t rows, std::size_t cols,
	    class CoeffType1, class CoeffType2>
  auto operator*(Matrix<rows,cols,CoeffType1> m,
		 CoeffType2 value)
  {
    return value * m;
  }

template <std::size_t rows1, std::size_t cols1,
	  std::size_t rows2, std::size_t cols2,
	  class CoeffType1, class CoeffType2>
struct Simplification<
  Multiplication<Matrix<rows1,cols1,CoeffType1>,
		 Matrix<rows2,cols2,CoeffType2>>,0>
{
  static_assert(rows2 == cols1,
		"Number of columns of left matrix "
		"must equal number of rows of right matrix");
  typedef typename std::common_type<
    CoeffType1, CoeffType2>::type OutCoeff;

  typedef Matrix<rows1, cols2, OutCoeff> type;

  static type Combine(Multiplication<
		      Matrix<rows1,cols1,CoeffType1>,
		      Matrix<rows2,cols2,CoeffType2>> m)
  {
#ifdef PRINT_SIMPLIFIES
    std::cout << "Simplifying matrix multiplication\n";
#endif
    auto ms = m.GetFunctions();
    return get<0>(ms) * get<1>(ms);
  }
};
}

#endif
