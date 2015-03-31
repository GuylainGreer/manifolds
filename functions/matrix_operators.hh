#ifndef MANIFOLD_MATRIX_SIMPLICATIONS_HH
#define MANIFOLD_MATRIX_SIMPLICATIONS_HH

#include "addition.hh"
#include "multiplication.hh"

namespace manifolds {

template <std::size_t rows1, std::size_t cols1, class CoeffType1,
          class CoeffType2>
auto & operator+=(Matrix<rows1, cols1, CoeffType1> & a,
               Matrix<rows1, cols1, CoeffType2> b) {
  typedef typename std::common_type<CoeffType1, CoeffType2>::type OutCoeff;

  for (std::size_t i = 0; i < rows1; i++)
    for (std::size_t j = 0; j < cols1; j++)
      a.Coeff(i, j) += b.Coeff(i, j);
  return a;
}

    template <std::size_t rows1, std::size_t cols1, class CoeffType1,
              class CoeffType2>
    auto operator+(Matrix<rows1, cols1, CoeffType1> a,
                   Matrix<rows1, cols1, CoeffType2> b)
    {
        a += b;
        return a;
    }

template <class T> auto MassageMultiplicationOutput(T t, int_<1>, int_<1>) {
  return t.Coeff(0, 0);
}

template <class T, int i, int j>
auto MassageMultiplicationOutput(T t, int_<i>, int_<j>) {
  return t;
}

template <class T> auto MassageMultiplicationOutput(T t) {
  return MassageMultiplicationOutput(t, int_<T::num_rows>(),
                                     int_<T::num_cols>());
}

template <std::size_t rows1, std::size_t cols1, std::size_t cols2,
          class CoeffType1, class CoeffType2>
auto operator*(Matrix<rows1, cols1, CoeffType1> a,
               Matrix<cols1, cols2, CoeffType2> b) {
  Matrix<rows1, cols2, typename std::common_type<CoeffType1, CoeffType2>::type>
  r;
  for (std::size_t i = 0; i < rows1; i++)
    for (std::size_t j = 0; j < cols1; j++)
      for (std::size_t k = 0; k < cols2; k++)
        r.Coeff(i, k) += a.Coeff(i, j) * b.Coeff(j, k);
  return MassageMultiplicationOutput(r);
}

template <std::size_t rows, std::size_t cols, class CoeffType1,
          class CoeffType2>
auto operator*(CoeffType2 value, Matrix<rows, cols, CoeffType1> m) {
  for (unsigned i = 0; i < rows; i++)
    for (unsigned j = 0; j < cols; j++)
      m.Coeff(i, j) *= value;
  return m;
}

template <std::size_t rows, std::size_t cols, class CoeffType1,
          class CoeffType2>
auto operator*(Matrix<rows, cols, CoeffType1> m, CoeffType2 value) {
  return value * m;
}

template <class... Types, std::size_t rows, std::size_t cols, class CoeffType>
auto operator*(const tuple<Types...> &t,
               const Matrix<rows, cols, CoeffType> &m) {
  auto a = ToArray(t);
  return Matrix<sizeof...(Types), 1, typename decltype(a)::value_type>(a) * m;
}

template <class... Types, std::size_t rows, std::size_t cols, class CoeffType>
auto operator*(const Matrix<rows, cols, CoeffType> &m,
               const tuple<Types...> &t) {
  auto a = ToArray(t);
  return m * Matrix<sizeof...(Types), 1, typename decltype(a)::value_type>(a);
}
}

#endif
