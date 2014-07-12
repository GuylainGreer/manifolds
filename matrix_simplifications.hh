#ifndef MANIFOLD_MATRIX_SIMPLICATIONS_HH
#define MANIFOLD_MATRIX_SIMPLICATIONS_HH

#include "simplify.hh"
#include "addition.hh"
#include "multiplication.hh"
#include "function_matrix.hh"

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
	   Matrix<rows2,cols2,CoeffType2>>>
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
    auto ms = a.GetFunctions();
    return std::get<0>(a) + std::get<1>(a);
  }
};

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
  return r;
}  

template <std::size_t rows1, std::size_t cols1,
	  std::size_t rows2, std::size_t cols2,
	  class CoeffType1, class CoeffType2>
struct Simplification<
  Multiplication<Matrix<rows1,cols1,CoeffType1>,
		 Matrix<rows2,cols2,CoeffType2>>>
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
    auto ms = m.GetFunctions();
    return std::get<0>(ms) * std::get<1>(ms);
  }
};

  template <class ... Functions, class ... OtherFunctions>
  struct Simplification<
    Composition<Group<Functions...>, OtherFunctions...>>
  {
    typedef FunctionMatrix<
      sizeof...(Functions), 1, Functions...> MType;
    typedef Composition<MType, OtherFunctions...> type;
    static type Combine(Composition<Group<Functions...>,
			OtherFunctions...> c)
    {
      auto ofs = c.GetFunctions();
      MType m(std::get<0>(ofs).GetFunctions());
      auto new_tuple =
	std::tuple_cat(m, remove_element<0>(ofs));
      return type(new_tuple);
    }
  };
}
#endif
